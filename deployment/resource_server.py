"""Serve deployed LAN resources and the public game-account API, never admin APIs."""
import argparse
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import json
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from account_proxy import account_request


class ResourceHandler(SimpleHTTPRequestHandler):
    def setup(self):
        super().setup()
        self.connection.settimeout(20)

    def log_message(self, *_):
        # Account request lines and payloads must never appear in resource logs.
        pass

    def do_POST(self):
        self.close_connection = True
        origin = self.headers.get('Origin')
        expected = 'http://' + self.headers.get('Host', '')
        if origin and origin != expected:
            status, value = 403, {'ok': False, 'message': 'Origin rejected'}
        else:
            status, value = account_request(self, self.server.runtime)
        data = json.dumps(value, ensure_ascii=False).encode('utf-8')
        self.send_response(status)
        self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Content-Length', str(len(data)))
        self.send_header('Cache-Control', 'no-store')
        self.send_header('Connection', 'close')
        self.end_headers()
        self.wfile.write(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--install-dir', type=Path, required=True)
    parser.add_argument('--port', type=int, required=True)
    args = parser.parse_args()
    root = args.install_dir.resolve()
    server = ThreadingHTTPServer(('0.0.0.0', args.port), partial(ResourceHandler, directory=str(root / 'web')))
    server.daemon_threads = True
    server.runtime = root
    server.serve_forever()


if __name__ == '__main__':
    main()
