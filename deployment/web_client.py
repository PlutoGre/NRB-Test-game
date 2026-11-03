"""Serve the WebAssembly client and a same-origin, cookie-gated game bridge.

Game accounts are verified by the shared Java account service. The signed cookie
only gates browser transport; every game login also requires its account token.
"""
import argparse
import hashlib
import hmac
from http.cookies import SimpleCookie, CookieError
import json
import mimetypes
from pathlib import Path
import re
import secrets
import socket
import sys
import threading
import time
import xml.etree.ElementTree as ET
from urllib.parse import unquote, urlsplit

# Portable embedded Python does not put this script's directory on sys.path.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from websocket_gateway import Gateway, Handler, PREFIX, PROTOCOL, check_runtime
from web_assets import WebAssets, accepts_gzip, unchanged
from account_proxy import ACCOUNT_ROUTES, account_request
from account_proxy import OPENER
from urllib.request import Request
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "scripts"))
from project_paths import DEFAULT_INSTALL_DIR, project_path

COOKIE = "nrb_web_identity"
MAX_AGE = 30 * 86400
FILES = {"index.html", "web.js", "accounts.js", "style.css", "game.js", "game.wasm", "game.data", "web-client.json"}


def public_origin(value):
    parsed = urlsplit(value)
    if (parsed.scheme not in {"http", "https"} or not parsed.hostname
            or parsed.username or parsed.password or parsed.path not in {"", "/"}
            or parsed.query or parsed.fragment):
        raise ValueError("Public origin must be an http(s) origin without a path")
    # An external reverse proxy must terminate TLS. Direct HTTP is for LAN testing.
    return value.rstrip("/")


class WebGateway(Gateway):
    def __init__(self, root, build, bind, port, origin):
        # Reuse bounded worker/connection management, but replace the HTTP handler.
        self.root = root
        self.state = check_runtime(root)
        self.resources = (root / "web" / PREFIX.lstrip("/")).resolve()
        self.build = build
        self.requests = threading.BoundedSemaphore(48)
        self.games = threading.BoundedSemaphore(8)
        self.origin = public_origin(origin)
        self.origins = {self.origin}
        if urlsplit(self.origin).scheme == "http":
            self.origins.update({"http://localhost:" + str(port), "http://127.0.0.1:" + str(port)})
        self.hosts = {urlsplit(item).netloc.lower() for item in self.origins}
        manifest = json.loads((build / "web-client.json").read_text(encoding="utf-8"))
        if manifest.get("platform") != "web" or manifest.get("buildComplete") is not True or manifest.get("accountAuth") != 1:
            raise ValueError("Web build is incomplete; run deployment/build-web.cmd first")
        self.manifest = manifest
        for name in FILES:
            if not (build / name).is_file():
                raise ValueError("Missing Web output: " + name)
        self.assets = WebAssets(build, root / "cache/web-assets", FILES)
        secret = root / "private/web-session.key"
        try:
            with secret.open("xb") as stream:
                stream.write(secrets.token_bytes(32))
        except FileExistsError:
            pass
        self.key = secret.read_bytes()
        if len(self.key) != 32:
            raise ValueError("Invalid browser identity signing key")
        # Bypass Gateway.__init__, which intentionally binds only the iOS tunnel.
        from http.server import ThreadingHTTPServer
        ThreadingHTTPServer.__init__(self, (bind, port), WebHandler)


class WebHandler(Handler):
    server_version = "NRB-Web/1"

    def end_headers(self):
        self.send_header("X-Content-Type-Options", "nosniff")
        self.send_header("Referrer-Policy", "same-origin")
        self.send_header("X-Frame-Options", "SAMEORIGIN")
        super().end_headers()

    def valid_host(self):
        return self.headers.get("Host", "").lower() in self.server.hosts

    def request_origin(self):
        # Never use client-controlled X-Forwarded-Host/Proto to construct cookies.
        return self.server.origin if urlsplit(self.server.origin).scheme == "https" else "http://" + self.headers["Host"]

    def identity(self):
        try:
            cookies = SimpleCookie()
            cookies.load(self.headers.get("Cookie", ""))
            value = cookies[COOKIE].value
            uid, issued, signature = value.split(".")
            if not re.fullmatch(r"acct_[0-9a-f]{32}", uid):
                return None
            if not 0 <= time.time() - int(issued) <= MAX_AGE:
                return None
            expected = hmac.new(self.server.key, (uid + "." + issued).encode(), hashlib.sha256).hexdigest()
            return uid if hmac.compare_digest(signature, expected) else None
        except (KeyError, ValueError, CookieError):
            return None

    def ready(self):
        state = check_runtime(self.server.root)
        # Check both resources and game listeners before granting a browser session.
        for name in ("webPort", "gamePort"):
            with socket.create_connection(("127.0.0.1", state[name]), timeout=2):
                pass
        with OPENER.open(Request('http://127.0.0.1:' + str(state['adminPort']) + '/admin/v1/health'), timeout=5) as response:
            if json.loads(response.read(8192)).get('accountAuth') != 1:
                raise ValueError('Restart the updated game server to enable managed accounts')
        return state

    def empty_request_body(self):
        """Accept an empty body even when a tunnel forwards it using chunked framing."""
        lengths = self.headers.get_all("Content-Length", [])
        transfers = self.headers.get_all("Transfer-Encoding", [])
        # Do not allow ambiguous framing or consume an arbitrary request body.
        # HTTP responses close the connection, including rejections.
        if len(lengths) > 1 or len(transfers) > 1 or (lengths and transfers):
            return False
        if not transfers:
            return not lengths or re.fullmatch(r"0+", lengths[0].strip()) is not None
        if transfers[0].strip().lower() != "chunked":
            return False
        try:
            # An empty chunked body contains only its zero chunk and final CRLF.
            # Bound both reads; nonempty chunks and trailers are not gateway input.
            return (re.fullmatch(rb"0+\r\n", self.rfile.readline(64)) is not None
                    and self.rfile.readline(2) == b"\r\n")
        except OSError:
            return False

    def do_POST(self):
        self.close_connection = True
        if not self.valid_host() or self.headers.get("Origin") not in self.server.origins:
            return self.reply(403, {"error": "Origin rejected"})
        if self.path not in ACCOUNT_ROUTES:
            return self.reply(400, {"error": "请更新网页客户端后使用账号登录"})
        try:
            self.ready()
            status, result = account_request(self, self.server.root)
            cookie = None
            if status == 200 and self.path != "/account/v1/logout":
                uid = result.get("uid", "")
                if not re.fullmatch(r"acct_[0-9a-f]{32}", uid):
                    raise ValueError("Invalid account response")
                signed = uid + "." + str(int(time.time()))
                value = signed + "." + hmac.new(self.server.key, signed.encode(), hashlib.sha256).hexdigest()
                cookie = COOKIE + "=" + value + "; Path=/; HttpOnly; SameSite=Strict; Max-Age=" + str(MAX_AGE)
            elif self.path == "/account/v1/logout" or status == 401:
                cookie = COOKIE + "=; Path=/; HttpOnly; SameSite=Strict; Max-Age=0"
            payload = json.dumps(result).encode()
            self.send_response(status)
            if cookie:
                self.send_header("Set-Cookie", cookie + ("; Secure" if self.request_origin().startswith("https:") else ""))
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(payload)))
            self.send_header("Cache-Control", "no-store")
            self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(payload)
        except (OSError, ValueError, KeyError, ET.ParseError):
            self.reply(503, {"error": "Start the deployed game server before logging in"})

    def get_request(self):
        if not self.valid_host():
            return self.reply(403, {"error": "Host rejected; configure --public-origin for this URL"})
        if not self.empty_request_body():
            return self.reply(400, {"error": "Unexpected body"})
        path = unquote(urlsplit(self.path).path)
        if path == "/web/config":
            self.ready()
            return self.reply(200, {"serverName": "男人帮1区", "protocol": PROTOCOL,
                                    "accountAuth": 1,
                                    "build": self.server.manifest.get("sourceRevision", "unknown"),
                                    "instanceId": hashlib.sha256(self.server.key).hexdigest()[:24],
                                    "downloads": self.server.assets.summary})
        if path == "/web/game":
            if self.headers.get("Origin") not in self.server.origins or not self.identity():
                return self.reply(403, {"error": "Browser session required"})
            self.ready()
            return self.websocket()
        if path.startswith(PREFIX):
            if not self.identity():
                return self.reply(401, {"error": "Browser session required"})
            return self.resource(path, self.request_origin())
        name = "index.html" if path == "/" else path.lstrip("/")
        if name not in FILES:
            return self.reply(404, {"error": "Not found"})
        asset = self.server.assets.get(name)
        encoded = asset["gzipPath"] is not None and accepts_gzip(
            ",".join(self.headers.get_all("Accept-Encoding", [])))
        cached = unchanged(",".join(self.headers.get_all("If-None-Match", [])), asset["etag"])
        self.send_response(304 if cached else 200)
        types = {".wasm": "application/wasm", ".js": "text/javascript; charset=utf-8", ".data": "application/octet-stream"}
        if not cached:
            self.send_header("Content-Type", types.get(Path(name).suffix, mimetypes.guess_type(name)[0] or "application/octet-stream"))
            self.send_header("Content-Length", str(asset["gzipSize"] if encoded else asset["size"]))
            if encoded:
                self.send_header("Content-Encoding", "gzip")
        # These URLs are not versioned. Check cached bytes against the current
        # files on every load instead of pinning an old build after an update.
        self.send_header("Cache-Control", "public, no-cache")
        self.send_header("ETag", asset["etag"])
        self.send_header("Vary", "Accept-Encoding")
        self.send_header("Connection", "close")
        self.end_headers()
        if cached:
            return
        target = asset["gzipPath"] if encoded else asset["path"]
        with target.open("rb") as stream:
            while block := stream.read(65536):
                self.wfile.write(block)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--install-dir", type=Path, default=DEFAULT_INSTALL_DIR)
    parser.add_argument("--build-dir", type=Path, default=Path("outputs/web"))
    parser.add_argument("--port", type=int, default=18084)
    parser.add_argument("--bind", default="0.0.0.0")
    parser.add_argument("--public-origin")
    managed = parser.add_mutually_exclusive_group()
    managed.add_argument("--managed-start", action="store_true")
    managed.add_argument("--managed-stop", action="store_true")
    managed.add_argument("--managed-status", action="store_true")
    args = parser.parse_args()
    root, build = project_path(args.install_dir).resolve(), project_path(args.build_dir).resolve()
    if not 1024 <= args.port <= 65535:
        parser.error("Port must be between 1024 and 65535")
    if args.managed_stop or args.managed_status:
        from server import Deployment, operation_lock
        with operation_lock(root):
            deployment = Deployment(root)
            deployment.require_installed()
            if args.managed_stop:
                deployment.terminate_owned("browser")
                print("LAN browser gateway stopped; game server and other entries retained.")
            else:
                print("LAN browser gateway:", "owned/running" if deployment.owned("browser") else "not running")
                if deployment.owned("browser"):
                    print("Open this URL: " + deployment.state["browser"]["origin"] + "/")
        return
    state = check_runtime(root)
    origin = public_origin(args.public_origin or "http://" + state["host"] + ":" + str(args.port))
    if args.managed_start:
        from server import Deployment, operation_lock
        with operation_lock(root):
            deployment = Deployment(root)
            deployment.require_installed()
            previous = deployment.state.get("browser", {})
            settings = {"port": args.port, "origin": origin, "build": str(build), "bind": args.bind}
            if deployment.owned("browser") and previous != settings:
                raise RuntimeError("Stop the running browser gateway before changing its settings")
            deployment.state["browser"] = settings
            deployment.save()
            deployment.launch("browser", [sys.executable, Path(__file__).resolve(), "--install-dir", root,
                "--build-dir", build, "--port", args.port, "--bind", args.bind, "--public-origin", origin], root, args.port)
            print("Open this URL: " + origin + "/")
        return
    with WebGateway(root, build, args.bind, args.port, origin) as gateway:
        print("Web client listening: " + origin, flush=True)
        gateway.serve_forever()


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError) as error:
        sys.exit("Web client failed: " + str(error))
