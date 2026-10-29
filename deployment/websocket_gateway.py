"""Loopback-only resource server and bounded RFC 6455 binary-to-TCP bridge.

The invitation gates personal testing, not individual player authorization. Never
publish the Java port, database, source tree or legacy PHP administration pages.
"""
import argparse
import base64
import hashlib
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import mimetypes
from pathlib import Path
import secrets
import selectors
import socket
import struct
import sys
import threading
import time
from urllib.parse import unquote, urlsplit
import xml.etree.ElementTree as ET

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "src/tools"))
from project_paths import DEFAULT_INSTALL_DIR, project_path

PREFIX = "/gameserver/dzm/1.8/"
PROTOCOL = "nrb-tcp-v1"
MAX_MESSAGE = 1024 * 1024
MAX_BUFFER = 2 * MAX_MESSAGE


def configuration(root):
    value = json.loads((root / "private/gateway.json").read_text(encoding="utf-8"))
    origin = value.get("publicOrigin", "")
    parsed = urlsplit(origin)
    if origin and (parsed.scheme != "https" or not parsed.hostname or parsed.username
                   or parsed.password or parsed.path or parsed.query or parsed.fragment):
        raise ValueError("Invalid gateway public origin")
    if len(value.get("invite", "")) < 32:
        raise ValueError("Missing gateway invitation")
    return value


def check_runtime(root):
    state = json.loads((root / ".onepiece-server.json").read_text(encoding="utf-8"))
    if not state.get("installed") or (root / "private/restore-in-progress").exists():
        raise ValueError("Complete server installation/restoration first")
    xml = ET.parse(root / "server/xml/config.xml")
    if xml.findtext("CanUseGm") != "0":
        raise ValueError("External testing requires CanUseGm=0")
    return state


def frame(opcode, payload=b""):
    size = len(payload)
    header = bytes([0x80 | opcode])
    if size < 126:
        return header + bytes([size]) + payload
    if size <= 65535:
        return header + b"\x7e" + struct.pack("!H", size) + payload
    return header + b"\x7f" + struct.pack("!Q", size) + payload


class WebSocketEnd(Exception):
    def __init__(self, code=1000):
        self.code = code


class BinaryBridge:
    """Keep byte order, bound memory, and fail closed instead of replaying packets."""
    def __init__(self, browser, backend):
        self.browser, self.backend = browser, backend
        self.incoming = bytearray()
        self.to_tcp, self.to_ws = bytearray(), bytearray()
        self.fragment = None
        self.ping = None
        self.ping_time = time.monotonic()

    def parse(self):
        while len(self.incoming) >= 2:
            first, second = self.incoming[:2]
            opcode, final = first & 15, bool(first & 128)
            if first & 112 or not second & 128 or opcode not in (0, 2, 8, 9, 10):
                raise WebSocketEnd(1002 if opcode != 1 else 1003)
            size, offset = second & 127, 2
            if size in (126, 127):
                count = 2 if size == 126 else 8
                if len(self.incoming) < offset + count:
                    return
                extended = int.from_bytes(self.incoming[offset:offset + count], "big")
                if extended < (126 if count == 2 else 65536) or extended >> 63:
                    raise WebSocketEnd(1002)
                size, offset = extended, offset + count
            if opcode >= 8 and (not final or size > 125):
                raise WebSocketEnd(1002)
            if size > MAX_MESSAGE:
                raise WebSocketEnd(1009)
            if len(self.incoming) < offset + 4 + size:
                return
            mask = self.incoming[offset:offset + 4]
            payload = bytes(byte ^ mask[i % 4] for i, byte in
                            enumerate(self.incoming[offset + 4:offset + 4 + size]))
            del self.incoming[:offset + 4 + size]
            if opcode == 8:
                if len(payload) == 1:
                    raise WebSocketEnd(1002)
                if payload:
                    code = int.from_bytes(payload[:2], "big")
                    if code not in (1000, 1001, 1002, 1003, 1007, 1008, 1009, 1010, 1011,
                                    1012, 1013, 1014) and not 3000 <= code <= 4999:
                        raise WebSocketEnd(1002)
                    try:
                        payload[2:].decode("utf-8")
                    except UnicodeError:
                        raise WebSocketEnd(1007)
                raise WebSocketEnd()
            if opcode == 9:
                self.to_ws.extend(frame(10, payload))
            elif opcode == 10:
                if payload == self.ping:
                    self.ping = None
                    self.ping_time = time.monotonic()
            else:
                if opcode == 2:
                    if self.fragment is not None:
                        raise WebSocketEnd(1002)
                    self.fragment = bytearray()
                elif self.fragment is None:
                    raise WebSocketEnd(1002)
                self.fragment.extend(payload)
                if len(self.fragment) > MAX_MESSAGE:
                    raise WebSocketEnd(1009)
                if final:
                    self.to_tcp.extend(self.fragment)
                    self.fragment = None
            if len(self.to_ws) > MAX_BUFFER or len(self.to_tcp) > MAX_BUFFER:
                raise WebSocketEnd(1009)

    def run(self):
        self.browser.setblocking(False)
        self.backend.setblocking(False)
        try:
            with selectors.DefaultSelector() as selector:
                while True:
                    now = time.monotonic()
                    if self.ping is not None and now - self.ping_time > 40:
                        raise WebSocketEnd(1001)
                    if self.ping is None and now - self.ping_time > 20:
                        self.ping = secrets.token_bytes(8)
                        self.ping_time = now
                        self.to_ws.extend(frame(9, self.ping))
                    for sock, output, readable in (
                        (self.browser, self.to_ws, len(self.to_tcp) < MAX_MESSAGE),
                        (self.backend, self.to_tcp, len(self.to_ws) < MAX_MESSAGE),
                    ):
                        events = (selectors.EVENT_READ if readable else 0) | (
                            selectors.EVENT_WRITE if output else 0)
                        try:
                            selector.unregister(sock)
                        except KeyError:
                            pass
                        if events:
                            selector.register(sock, events)
                    for key, events in selector.select(1):
                        sock = key.fileobj
                        output = self.to_ws if sock is self.browser else self.to_tcp
                        if events & selectors.EVENT_WRITE:
                            try:
                                sent = sock.send(output)
                                if not sent:
                                    raise WebSocketEnd(1011)
                                del output[:sent]
                            except BlockingIOError:
                                pass
                        if events & selectors.EVENT_READ:
                            try:
                                data = sock.recv(16384)
                            except BlockingIOError:
                                continue
                            if not data:
                                raise WebSocketEnd(1001)
                            if sock is self.browser:
                                self.incoming.extend(data)
                                if len(self.incoming) > MAX_MESSAGE + 16398:
                                    raise WebSocketEnd(1009)
                                self.parse()
                            else:
                                self.to_ws.extend(frame(2, data))
        except (WebSocketEnd, OSError) as error:
            try:
                self.browser.settimeout(0.2)
                self.browser.sendall(frame(8, struct.pack("!H", getattr(error, "code", 1011))))
            except OSError:
                pass


class Gateway(ThreadingHTTPServer):
    daemon_threads = True
    request_queue_size = 32

    def __init__(self, root, port, slots):
        self.root = root
        self.state = check_runtime(root)
        self.resources = (root / "web" / PREFIX.lstrip("/")).resolve()
        self.requests = threading.BoundedSemaphore(48)
        self.games = threading.BoundedSemaphore(slots)
        super().__init__(("127.0.0.1", port), Handler)

    def process_request(self, request, address):
        if not self.requests.acquire(False):
            self.shutdown_request(request)
            return
        try:
            super().process_request(request, address)
        except BaseException:
            self.requests.release()
            raise

    def process_request_thread(self, request, address):
        try:
            super().process_request_thread(request, address)
        finally:
            self.requests.release()

    def handle_error(self, request, address):
        # HTTP request lines and exception text can contain user-supplied secrets.
        print("gateway: request failed; connection closed", flush=True)


class Handler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"
    server_version = "NRB-Gateway/1"
    rbufsize = 0  # Do not consume early WebSocket frames into an HTTP-only buffer.

    def setup(self):
        super().setup()
        self.connection.settimeout(15)

    def log_message(self, *_):
        pass

    def reply(self, status, value):
        data = json.dumps(value, separators=(",", ":")).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("Connection", "close")
        self.end_headers()
        self.close_connection = True
        self.wfile.write(data)

    def do_GET(self):
        self.close_connection = True
        try:
            self.get_request()
        except (OSError, ValueError, KeyError, ET.ParseError):
            self.reply(503, {"error": "Gateway configuration or backend unavailable"})

    def get_request(self):
        path = urlsplit(self.path).path
        if self.headers.get("Transfer-Encoding") or self.headers.get("Content-Length", "0") != "0":
            self.reply(400, {"error": "Request body not supported"})
            return
        if path == "/ping":
            self.reply(200, {"gateway": "nrb", "protocol": PROTOCOL})
            return
        config = configuration(self.server.root)
        expected = "Bearer " + config["invite"]
        if not secrets.compare_digest(self.headers.get("Authorization", "").encode(), expected.encode()):
            self.reply(401, {"error": "Invitation required"})
            return
        if not config.get("publicOrigin"):
            self.reply(503, {"error": "Tunnel is starting"})
            return
        if path == "/check":
            check_runtime(self.server.root)
            with socket.create_connection(("127.0.0.1", self.server.state["gamePort"]), timeout=3):
                pass
            self.reply(200, {"gateway": "nrb", "protocol": PROTOCOL, "backend": True})
        elif path == "/game":
            self.websocket()
        elif path.startswith(PREFIX):
            self.resource(path, config["publicOrigin"])
        else:
            self.reply(404, {"error": "Not found"})

    def websocket(self):
        key = self.headers.get("Sec-WebSocket-Key", "")
        try:
            valid_key = len(base64.b64decode(key, validate=True)) == 16
        except ValueError:
            valid_key = False
        if (self.headers.get("Upgrade", "").lower() != "websocket"
                or "upgrade" not in [s.strip().lower() for s in self.headers.get("Connection", "").split(",")]
                or self.headers.get("Sec-WebSocket-Version") != "13" or not valid_key
                or PROTOCOL not in [s.strip() for s in self.headers.get("Sec-WebSocket-Protocol", "").split(",")]):
            self.reply(400, {"error": "Binary WebSocket protocol required"})
            return
        if not self.server.games.acquire(False):
            self.reply(503, {"error": "All game connections are in use"})
            return
        try:
            check_runtime(self.server.root)
            with socket.create_connection(("127.0.0.1", self.server.state["gamePort"]), timeout=5) as backend:
                accept = base64.b64encode(hashlib.sha1((key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").encode("ascii")).digest()).decode()
                self.send_response(101)
                self.send_header("Upgrade", "websocket")
                self.send_header("Connection", "Upgrade")
                self.send_header("Sec-WebSocket-Accept", accept)
                self.send_header("Sec-WebSocket-Protocol", PROTOCOL)
                self.end_headers()
                self.wfile.flush()
                print("gateway: game connection opened", flush=True)
                BinaryBridge(self.connection, backend).run()
                print("gateway: game connection closed", flush=True)
        finally:
            self.server.games.release()

    def resource(self, path, origin):
        name = unquote(path[len(PREFIX):])
        target = (self.server.resources / name).resolve()
        if (not target.is_relative_to(self.server.resources) or not target.is_file()
                or "summary_dzm" in target.parts
                or target.suffix.lower() in {".sql", ".xml", ".jar", ".exe", ".pem", ".key", ".html", ".js"}
                or (target.suffix.lower() == ".php" and name not in {"servers_list.php", "update.php"})):
            self.reply(404, {"error": "Not found"})
            return
        if name == "servers_list.php":
            value = json.loads(target.read_text(encoding="utf-8"))
            value["rootAddress"] = origin + PREFIX
            value["updateAddress"] = origin + PREFIX + "update.php"
            for server in value["severs"]:
                server["address"], server["port"] = urlsplit(origin).hostname, 443
            self.reply(200, value)
            return
        # Keep resource/config bytes unchanged so the existing manifest CRCs match.
        size = target.stat().st_size
        self.send_response(200)
        self.send_header("Content-Type", mimetypes.guess_type(target.name)[0] or "application/octet-stream")
        self.send_header("Content-Length", str(size))
        self.send_header("Cache-Control", "no-store")
        self.send_header("Connection", "close")
        self.end_headers()
        with target.open("rb") as stream:
            while block := stream.read(65536):
                self.wfile.write(block)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--install-dir", type=Path, default=DEFAULT_INSTALL_DIR)
    parser.add_argument("--port", type=int, default=18082)
    parser.add_argument("--connections", type=int, default=8)
    args = parser.parse_args()
    args.install_dir = project_path(args.install_dir)
    if not 1024 <= args.port <= 65535 or not 1 <= args.connections <= 16:
        parser.error("Use port 1024..65535 and 1..16 game connections")
    configuration(args.install_dir)
    with Gateway(args.install_dir.resolve(), args.port, args.connections) as gateway:
        print("gateway: listening on loopback port", args.port, flush=True)
        gateway.serve_forever()


if __name__ == "__main__":
    main()
