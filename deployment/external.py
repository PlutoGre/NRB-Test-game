"""Manage an owned Quick Tunnel and loopback gateway for the installed server."""
import argparse
import os
from pathlib import Path
import re
import secrets
import subprocess
import sys
import time

# Embedded Python deliberately does not add the script directory to sys.path.
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import server
from websocket_gateway import check_runtime

server.MANIFEST.update(server.read_json(HERE / "tunnel-manifest.json"))


def details(deployment):
    gateway, tunnel = deployment.owned("gateway"), deployment.owned("tunnel")
    print("gateway:", "owned/running" if gateway else "not running")
    print("tunnel:", "owned/running" if tunnel else "not running")
    path = deployment.root / "private/gateway.json"
    if path.exists() and gateway and tunnel:
        config = server.read_json(path)
        print("iPhone HTTPS URL:", config.get("publicOrigin", "starting"))
        # Only an explicit local management command displays the invitation.
        print("Invitation (keep private):", config["invite"])
    print("Runtime:", deployment.root)


def stop(deployment):
    deployment.terminate_owned("tunnel")
    deployment.terminate_owned("gateway")


def start(deployment, args):
    deployment.require_installed()
    check_runtime(deployment.root)
    if args.port in (deployment.state["gamePort"], deployment.state["webPort"], deployment.state["dbPort"]):
        raise RuntimeError("Gateway port must differ from game, resource and database ports")
    if deployment.owned("gateway") and deployment.owned("tunnel"):
        if deployment.state.get("external", {}).get("port") != args.port:
            raise RuntimeError("Stop the external gateway before changing its port")
        if not server.port_open(args.port):
            raise RuntimeError("Gateway port unavailable; inspect gateway logs, then stop/start external access")
        details(deployment)
        return
    # Clean up partial previous launches, checking executable and creation time.
    stop(deployment)
    executable = server.download("cloudflared", args.offline)
    deployment.start()
    path = deployment.root / "private/gateway.json"
    old = server.read_json(path) if path.exists() else {}
    config = {"invite": old.get("invite") or secrets.token_urlsafe(32), "publicOrigin": ""}
    server.write_json(path, config)
    deployment.state["external"] = {"port": args.port}
    deployment.save()
    try:
        deployment.launch("gateway", [sys.executable, "-u", HERE / "websocket_gateway.py",
                          "--install-dir", deployment.root, "--port", args.port], deployment.root, args.port)
        log = deployment.root / "logs/tunnel.stderr.log"
        with (deployment.root / "logs/tunnel.stdout.log").open("wb") as stdout, log.open("wb") as stderr:
            process = subprocess.Popen([str(executable), "tunnel", "--no-autoupdate", "--protocol", args.protocol,
                                        "--url", "http://127.0.0.1:" + str(args.port)],
                                       cwd=deployment.root, stdin=subprocess.DEVNULL, stdout=stdout, stderr=stderr,
                                       creationflags=subprocess.CREATE_NO_WINDOW)
        record = server.process_info(process.pid)
        if not record:
            raise RuntimeError("cloudflared exited; inspect logs/tunnel.stderr.log")
        deployment.state.setdefault("processes", {})["tunnel"] = record
        deployment.save()
        deadline = time.monotonic() + 120
        while time.monotonic() < deadline:
            if not deployment.owned("tunnel") or not deployment.owned("gateway"):
                raise RuntimeError("Gateway or tunnel exited; inspect gateway/tunnel logs")
            content = log.read_text(encoding="utf-8", errors="replace")
            # Do not infer the public origin from a caller-controlled Host header.
            match = re.search(r"https://[a-z0-9]+(?:-[a-z0-9]+)*\.trycloudflare\.com\b", content)
            if match:
                config["publicOrigin"] = match.group()
                server.write_json(path, config)
                deployment.state["external"]["publicOrigin"] = match.group()
                deployment.save()
                print("Tunnel address allocated; use the iPhone connection page to check end-to-end access.")
                details(deployment)
                return
            time.sleep(1)
        raise RuntimeError("No Quick Tunnel address within 120 seconds; inspect tunnel logs/outbound networking")
    except BaseException:
        stop(deployment)
        raise


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("start", "stop", "status", "download", "rotate-invite"))
    parser.add_argument("--install-dir", required=True, type=Path)
    parser.add_argument("--port", type=int, default=18082)
    parser.add_argument("--protocol", choices=("auto", "http2", "quic"), default="auto")
    parser.add_argument("--offline", action="store_true")
    args = parser.parse_args()
    if os.name != "nt" or not 1024 <= args.port <= 65535:
        parser.error("Windows x64 and a gateway port in 1024..65535 are required")
    if args.action == "download":
        server.download("cloudflared", args.offline)
        return
    deployment = server.Deployment(args.install_dir)
    with server.operation_lock(deployment.root):
        deployment.require_installed()
        if args.action == "start":
            start(deployment, args)
        elif args.action == "stop":
            stop(deployment)
            print("External access stopped; game server and player data retained.")
        elif args.action == "rotate-invite":
            stop(deployment)
            server.write_json(deployment.root / "private/gateway.json",
                              {"invite": secrets.token_urlsafe(32), "publicOrigin": ""})
            print("Invitation rotated and external sessions closed. Run start to get the new invitation and URL.")
        else:
            details(deployment)


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, ValueError) as error:
        print("ERROR:", str(error), file=sys.stderr)
        sys.exit(1)
