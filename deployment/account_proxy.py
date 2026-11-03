"""Forward only game-account JSON operations to the loopback Java endpoint."""
import json
import re
from urllib.error import HTTPError, URLError
from urllib.request import Request, ProxyHandler, build_opener

ACCOUNT_ROUTES = frozenset('/account/v1/' + name for name in ('login', 'register', 'resume', 'logout'))
MAX_BODY = 4096
OPENER = build_opener(ProxyHandler({}))


def read_exact(stream, size):
    # WebSocket handlers deliberately use an unbuffered SocketIO. A single
    # read(size) may return a short TCP segment even for a complete HTTP body.
    chunks = []
    remaining = size
    while remaining:
        block = stream.read(remaining)
        if not block:
            raise ValueError('账号请求不完整')
        chunks.append(block)
        remaining -= len(block)
    return b''.join(chunks)


def read_account_body(handler):
    """Accept bounded JSON, including the framing used by Cloudflare HTTP/2."""
    if not handler.headers.get('Content-Type', '').lower().startswith('application/json'):
        raise ValueError('请求必须使用 application/json')
    lengths = handler.headers.get_all('Content-Length', [])
    transfers = handler.headers.get_all('Transfer-Encoding', [])
    if len(lengths) > 1 or len(transfers) > 1 or (lengths and transfers):
        raise ValueError('请求长度无效')
    if transfers:
        if transfers[0].strip().lower() != 'chunked':
            raise ValueError('请求编码无效')
        chunks, size = [], 0
        while True:
            line = handler.rfile.readline(64)
            if not re.fullmatch(rb'[0-9a-fA-F]+\r\n', line):
                raise ValueError('请求编码无效')
            count = int(line.strip(), 16)
            if size + count > MAX_BODY:
                raise ValueError('账号请求过长')
            if not count:
                if handler.rfile.readline(2) != b'\r\n':
                    raise ValueError('请求尾部无效')
                break
            block = read_exact(handler.rfile, count)
            if read_exact(handler.rfile, 2) != b'\r\n':
                raise ValueError('账号请求不完整')
            chunks.append(block)
            size += count
        data = b''.join(chunks)
    else:
        if not lengths or not re.fullmatch(r'[0-9]+', lengths[0]):
            raise ValueError('缺少请求长度')
        size = int(lengths[0])
        if not 0 < size <= MAX_BODY:
            raise ValueError('账号请求过长或为空')
        data = read_exact(handler.rfile, size)
    value = json.loads(data)
    if not isinstance(value, dict):
        raise ValueError('请求必须是 JSON 对象')
    return data


def account_request(handler, root):
    """Return a status/JSON pair. Never forward administrator routes or headers."""
    if handler.path not in ACCOUNT_ROUTES:
        return 404, {'ok': False, 'message': '未知账号接口'}
    try:
        data = read_account_body(handler)
        state = json.loads((root / '.onepiece-server.json').read_text(encoding='utf-8'))
        if not state.get('installed') or (root / 'private/restore-in-progress').exists():
            raise OSError('server unavailable')
        port = int(state['adminPort'])
        request = Request('http://127.0.0.1:' + str(port) + handler.path, data=data,
                          headers={'Content-Type': 'application/json'}, method='POST')
        try:
            response = OPENER.open(request, timeout=15)
        except HTTPError as error:
            response = error
        with response:
            result = response.read(8193)
            if len(result) > 8192:
                raise OSError('oversized account response')
            return response.code, json.loads(result)
    except (ValueError, UnicodeError):
        return 400, {'ok': False, 'message': '账号请求格式错误'}
    except (OSError, URLError, KeyError):
        return 503, {'ok': False, 'message': '账号服务不可用，请先启动或更新游戏服务端'}
