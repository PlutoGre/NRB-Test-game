"""Manage a browser gateway and Quick Tunnel without replacing the LAN/iOS entries."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import socket
import subprocess
import sys
import time
import urllib.request
from urllib.parse import urlsplit

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import server
from web_client import FILES
from websocket_gateway import PROTOCOL, check_runtime

server.MANIFEST.update(server.read_json(HERE / "tunnel-manifest.json"))
WEB = "web_external"
TUNNEL = "web_tunnel"
STATE = "webExternal"
URL_FILE = "web-external-url.txt"
ORIGIN = re.compile(r"https://[a-z0-9]+(?:-[a-z0-9]+)*\.trycloudflare\.com\b")


def build_manifest(build):
    manifest = server.read_json(build / "web-client.json")
    if (not isinstance(manifest, dict) or manifest.get("platform") != "web"
            or manifest.get("buildComplete") is not True or manifest.get("accountAuth") != 1):
        raise RuntimeError("Web build is incomplete; run deployment/build-web.cmd")
    for name in FILES:
        path = build / name
        if not path.is_file() or path.stat().st_size == 0:
            raise RuntimeError("Missing Web output: " + str(path))
    return manifest


def origin_from_state(value):
    origin = value.get("origin", "")
    return origin if isinstance(origin, str) and ORIGIN.fullmatch(origin) else ""


def read_health(url, host=None):
    headers = {"Cache-Control": "no-cache"}
    if host:
        headers["Host"] = host
    # Local readiness and the tunnel should work without sending game sessions
    # through an unrelated proxy. Certificate verification remains enabled.
    opener = urllib.request.build_opener(urllib.request.ProxyHandler({}))
    request = urllib.request.Request(url, headers=headers)
    with opener.open(request, timeout=4) as response:
        value = json.loads(response.read(8192).decode("utf-8"))
    if not isinstance(value, dict) or value.get("protocol") != PROTOCOL or not value.get("instanceId"):
        raise ValueError("Unexpected Web gateway response")
    return value


def local_health(deployment, settings):
    if not deployment.owned(WEB) or not deployment.owned(TUNNEL):
        raise RuntimeError("Web gateway or tunnel exited; inspect logs/web_external and logs/web_tunnel")
    return read_health("http://127.0.0.1:" + str(settings["port"]) + "/web/config",
                       urlsplit(settings["origin"]).netloc)


def public_error_kind(error):
    """urllib wraps Windows name-resolution errors in URLError.reason."""
    reason = getattr(error, "reason", error)
    return "dns" if isinstance(reason, socket.gaierror) else "request"


def confirm_public(deployment, settings, seconds):
    """Compare public readiness with the local gateway, without logging in a player."""
    expected = local_health(deployment, settings)
    settings["localReady"] = True
    settings["downloads"] = expected.get("downloads", {})
    deadline, last_error, last_kind = time.monotonic() + seconds, "not checked", "request"
    while True:
        if not deployment.owned(WEB) or not deployment.owned(TUNNEL):
            raise RuntimeError("Web gateway or tunnel exited during public readiness check")
        try:
            actual = read_health(settings["origin"] + "/web/config")
            if any(actual.get(key) != expected.get(key) for key in ("instanceId", "build", "protocol")):
                raise ValueError("Public URL does not match this Web deployment")
            settings["publicReady"] = True
            settings.pop("publicError", None)
            settings.pop("publicErrorKind", None)
            break
        except (OSError, ValueError) as error:
            last_error = str(error)
            last_kind = public_error_kind(error)
        if time.monotonic() >= deadline:
            settings["publicReady"] = False
            settings["publicError"] = last_error
            settings["publicErrorKind"] = last_kind
            break
        time.sleep(1)
    settings["checkedAt"] = int(time.time())
    deployment.state[STATE] = settings
    deployment.save()
    return settings["publicReady"]


def tunnel_diagnostics(deployment):
    """Show recent connection evidence without treating old registrations as live health."""
    path = deployment.root / "logs/web_tunnel.stderr.log"
    print("Tunnel connection details: " + str(path), flush=True)
    try:
        # Repeated reconnects can grow this file; status should only read its tail.
        with path.open("rb") as stream:
            stream.seek(0, os.SEEK_END)
            stream.seek(max(0, stream.tell() - 65536))
            lines = stream.read().decode("utf-8", errors="replace").splitlines()
    except OSError as error:
        print("Cannot read tunnel log: " + str(error), flush=True)
        return
    markers = (" ERR ", " WRN ", "precheck ", "precheck complete", "SUMMARY:",
               "Registered tunnel connection", "connection terminated", "Connection terminated")
    relevant = [line for line in lines if any(marker in line for marker in markers)]
    print("Recent tunnel log (historical records; public access remains unconfirmed):", flush=True)
    for line in (relevant or [line for line in lines if line.strip()])[-12:]:
        print("  " + line[:1200], flush=True)


def details(deployment):
    settings = deployment.state.get(STATE, {})
    running = deployment.owned(WEB) and deployment.owned(TUNNEL)
    print("External Web gateway:", "owned/running" if deployment.owned(WEB) else "not running", flush=True)
    print("Web tunnel process:", "owned/running" if deployment.owned(TUNNEL) else "not running", flush=True)
    origin = origin_from_state(settings)
    if running and origin:
        print("Web HTTPS URL: " + origin + "/", flush=True)
        print("URL saved: " + str(deployment.root / URL_FILE), flush=True)
        downloads = settings.get("downloads", {})
        if downloads.get("originalBytes") and downloads.get("gzipBytes"):
            print("Web bundle: {:.1f} MiB original; {:.1f} MiB gzip transfer estimate."
                  .format(downloads["originalBytes"] / 1048576, downloads["gzipBytes"] / 1048576), flush=True)
            print("Repeat downloads use browser cache after ETag revalidation.", flush=True)
        if settings.get("publicReady"):
            print("Public readiness: confirmed at " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(settings["checkedAt"])), flush=True)
        else:
            print("Public readiness: NOT confirmed from this computer. The processes remain running.", flush=True)
            print("Last public check: " + settings.get("publicError", "not checked"), flush=True)
            if settings.get("publicErrorKind") == "dns":
                print("DNS lookup failed for " + urlsplit(origin).hostname + " on this computer.", flush=True)
                if settings.get("localReady"):
                    print("The local Web gateway answered the readiness check.", flush=True)
                print("Keep the tunnel running. Try this URL using a phone's mobile data, then run the status shortcut again.", flush=True)
                print("On the hosting PC, clear cached DNS with: ipconfig /flushdns", flush=True)
                print("Check hostname resolution with: nslookup " + urlsplit(origin).hostname, flush=True)
                print("This is a DNS check failure; public access is still unconfirmed. Reinstalling the game is not required.", flush=True)
            print("If the browser shows Cloudflare error 1033, Cloudflare cannot find a healthy tunnel connection.", flush=True)
            tunnel_diagnostics(deployment)
    else:
        print("No active Web HTTPS URL. Start this entry to obtain an address.", flush=True)


def stop(deployment):
    # Separate names preserve the existing invitation-protected iOS tunnel and
    # LAN browser gateway. Creation-time/executable checks protect other apps.
    for name in (TUNNEL, WEB):
        deployment.terminate_owned(name)
        deployment.state.get("processes", {}).pop(name, None)
    deployment.state.pop(STATE, None)
    deployment.save()
    (deployment.root / URL_FILE).unlink(missing_ok=True)


def select_port(deployment, requested, preferred, releasing=None):
    reserved = {deployment.state[key] for key in ("webPort", "gamePort", "dbPort")}
    reserved.add(deployment.state.get("adminPort", 18090))
    reserved.add(deployment.state.get("browser", {}).get("port", 18084))
    reserved.add(deployment.state.get("external", {}).get("port", 18082))
    port = requested or preferred
    while port in reserved or (port != releasing and not server.port_available(port)):
        if requested:
            raise RuntimeError("External Web port is unavailable: " + str(port))
        port += 1
        if port > 65535:
            raise RuntimeError("No free external Web port")
    return port


def launch_tunnel(deployment, executable, port, protocol):
    # cloudflared does not listen on the target HTTP port, so it cannot use the
    # regular listener-based launcher. Keep its process identity in the same state.
    log = deployment.root / "logs/web_tunnel.stderr.log"
    with (deployment.root / "logs/web_tunnel.stdout.log").open("wb") as stdout, log.open("wb") as stderr:
        process = subprocess.Popen([str(executable), "tunnel", "--no-autoupdate", "--protocol", protocol,
                                    "--url", "http://127.0.0.1:" + str(port)],
                                   cwd=deployment.root, stdin=subprocess.DEVNULL, stdout=stdout, stderr=stderr,
                                   creationflags=subprocess.CREATE_NO_WINDOW)
    record = server.process_info(process.pid)
    if not record:
        if process.poll() is None:
            process.kill()
            process.wait(timeout=10)
        raise RuntimeError("cloudflared exited; inspect logs/web_tunnel.stderr.log")
    deployment.state.setdefault("processes", {})[TUNNEL] = record
    deployment.save()
    deadline = time.monotonic() + 120
    while time.monotonic() < deadline:
        if not deployment.owned(TUNNEL):
            raise RuntimeError("cloudflared exited; inspect logs/web_tunnel.stderr.log")
        match = ORIGIN.search(log.read_text(encoding="utf-8", errors="replace"))
        if match:
            return match.group()
        time.sleep(1)
    raise RuntimeError("No Quick Tunnel URL within 120 seconds; inspect logs/web_tunnel.stderr.log and outbound networking")


def start(deployment, args):
    check_runtime(deployment.root)
    build_manifest(args.build_dir)
    previous = deployment.state.get(STATE, {})
    requested_port = args.port or previous.get("port", 18085)
    origin = origin_from_state(previous)
    reuse = (deployment.owned(TUNNEL) and origin and previous.get("port") == requested_port
             and previous.get("protocol") == args.protocol)
    fingerprint = server.digest(args.build_dir / "web-client.json")
    gateway_digest = hashlib.sha256()
    for source in (HERE / "web_client.py", HERE / "web_assets.py", HERE / "websocket_gateway.py", HERE / "account_proxy.py", HERE / "server.py",
                   HERE.parent / "scripts/project_paths.py"):
        gateway_digest.update(source.read_bytes())
    gateway_fingerprint = gateway_digest.hexdigest()
    if reuse:
        settings = dict(previous)
        # Restart only the HTTP gateway after a client update, preserving the
        # live tunnel/domain and the browser's existing guest identity.
        if (previous.get("build") != str(args.build_dir) or previous.get("buildDigest") != fingerprint
                or previous.get("gatewayDigest") != gateway_fingerprint):
            deployment.terminate_owned(WEB)
        print("Reusing the current Quick Tunnel URL.", flush=True)
    else:
        executable = server.download("cloudflared", args.offline)
        # Reject an invalid requested port before stopping a working tunnel.
        releasing = previous.get("port") if deployment.owned(WEB) else None
        port = select_port(deployment, args.port, requested_port, releasing)
        stop(deployment)
        settings = {"port": port, "protocol": args.protocol, "build": str(args.build_dir),
                    "origin": "", "publicReady": False}
        deployment.state[STATE] = settings
        deployment.save()
        try:
            print("Creating the Web Quick Tunnel...", flush=True)
            origin = launch_tunnel(deployment, executable, port, args.protocol)
            settings["origin"] = origin
            deployment.state[STATE] = settings
            deployment.save()
        except BaseException:
            stop(deployment)
            raise
    settings["build"] = str(args.build_dir)
    settings["buildDigest"] = fingerprint
    settings["gatewayDigest"] = gateway_fingerprint
    settings["publicReady"] = False
    deployment.state[STATE] = settings
    deployment.save()
    try:
        deployment.launch(WEB, [sys.executable, "-u", HERE / "web_client.py", "--install-dir", deployment.root,
            "--build-dir", args.build_dir, "--port", settings["port"], "--bind", "127.0.0.1",
            "--public-origin", settings["origin"]], deployment.root, settings["port"])
        local_health(deployment, settings)
    except BaseException:
        stop(deployment)
        raise
    (deployment.root / URL_FILE).write_text(settings["origin"] + "/\n", encoding="utf-8")
    print("Checking the public Web endpoint...", flush=True)
    try:
        confirmed = confirm_public(deployment, settings, 30)
    except (OSError, ValueError, RuntimeError):
        stop(deployment)
        raise
    details(deployment)
    if not confirmed:
        print("The URL is allocated; try opening it from your external network, or run status to retry the check.", flush=True)
    print("This entry runs in the background. Stop it with the external Web stop shortcut.", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("start", "stop", "status"))
    parser.add_argument("--install-dir", type=Path, default=server.DEFAULT_INSTALL_DIR)
    parser.add_argument("--build-dir", type=Path, default=Path("outputs/web"))
    parser.add_argument("--port", type=int, default=0, help="0 reuses the previous port or selects a free port from 18085")
    parser.add_argument("--protocol", choices=("auto", "http2", "quic"), default="http2")
    parser.add_argument("--offline", action="store_true")
    args = parser.parse_args()
    if os.name != "nt" or (args.port != 0 and not 1024 <= args.port <= 65535):
        parser.error("Windows and a port of 0 or 1024..65535 are required")
    args.install_dir = server.project_path(args.install_dir).resolve()
    args.build_dir = server.project_path(args.build_dir).resolve()
    deployment = server.Deployment(args.install_dir)
    with server.operation_lock(deployment.root):
        deployment.require_installed()
        if args.action == "start":
            start(deployment, args)
        elif args.action == "stop":
            stop(deployment)
            print("External Web access stopped; the LAN/iOS entries and game server are retained.")
        else:
            settings = deployment.state.get(STATE, {})
            if deployment.owned(WEB) and deployment.owned(TUNNEL) and origin_from_state(settings):
                try:
                    confirm_public(deployment, settings, 4)
                except (OSError, ValueError, RuntimeError) as error:
                    settings.update(publicReady=False, localReady=False, publicError=str(error),
                                    publicErrorKind=public_error_kind(error))
                    deployment.state[STATE] = settings
                    deployment.save()
            details(deployment)


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError) as error:
        sys.exit("External Web startup failed: " + str(error))
