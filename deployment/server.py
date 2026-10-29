"""Portable Windows deployment for the bundled Java game server. Standard library only.

Never builds source, imports SQL into an unrelated database, or stops an unowned process.
"""
import argparse
from contextlib import contextmanager
import ctypes
from ctypes import wintypes
from datetime import datetime
import errno
import gzip
import hashlib
import json
import os
from pathlib import Path
import re
import secrets
import shutil
import socket
import subprocess
import sys
import tempfile
import time
import urllib.request
import xml.etree.ElementTree as ET
from xml.sax.saxutils import escape
import zipfile

HERE = Path(__file__).resolve().parent
REPO = HERE.parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / "src/tools"))
from project_paths import DEFAULT_INSTALL_DIR, project_path
from runtime_paths import ensure_alias, release_alias
MANIFEST = json.loads((HERE / "runtime-manifest.json").read_text(encoding="utf-8"))
CACHE = HERE / "download-cache"
STATE_NAME = ".onepiece-server.json"


def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + ".writing")
    temporary.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    os.replace(temporary, path)


def timestamp():
    return datetime.now().strftime("%Y%m%d-%H%M%S-%f")


def digest(path):
    value = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def download(name, offline):
    item = MANIFEST[name]
    CACHE.mkdir(parents=True, exist_ok=True)
    archive = CACHE / item["file"]
    if archive.exists():
        if digest(archive) != item["sha256"]:
            raise RuntimeError("Checksum mismatch; replace cached archive: " + str(archive))
        return archive
    if offline:
        raise RuntimeError("Offline archive missing: " + str(archive))
    print("Downloading", item["file"], flush=True)
    request = urllib.request.Request(item["url"], headers={"User-Agent": "OnePiece-Windows-Deployment/1"})
    temporary = archive.with_suffix(archive.suffix + ".partial")
    with urllib.request.urlopen(request, timeout=60) as response, temporary.open("wb") as output:
        shutil.copyfileobj(response, output, 1024 * 1024)
    if digest(temporary) != item["sha256"]:
        raise RuntimeError("Downloaded archive checksum mismatch: " + str(temporary))
    os.replace(temporary, archive)
    return archive


def process_info(pid):
    """Use creation time and executable to protect against PID reuse on Windows."""
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    kernel.OpenProcess.restype = wintypes.HANDLE
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    kernel.GetProcessTimes.argtypes = [wintypes.HANDLE] + [ctypes.POINTER(wintypes.FILETIME)] * 4
    kernel.GetExitCodeProcess.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
    kernel.QueryFullProcessImageNameW.argtypes = [wintypes.HANDLE, wintypes.DWORD, wintypes.LPWSTR,
                                                ctypes.POINTER(wintypes.DWORD)]
    handle = kernel.OpenProcess(0x1000, False, int(pid))
    if not handle:
        return None
    try:
        exit_code = wintypes.DWORD()
        if not kernel.GetExitCodeProcess(handle, ctypes.byref(exit_code)) or exit_code.value != 259:
            return None
        created, exited, kernel_time, user_time = (wintypes.FILETIME() for _ in range(4))
        size = wintypes.DWORD(32768)
        buffer = ctypes.create_unicode_buffer(size.value)
        if not kernel.GetProcessTimes(handle, ctypes.byref(created), ctypes.byref(exited),
                                      ctypes.byref(kernel_time), ctypes.byref(user_time)):
            return None
        if not kernel.QueryFullProcessImageNameW(handle, 0, buffer, ctypes.byref(size)):
            return None
        return {"pid": int(pid), "created": (created.dwHighDateTime << 32) | created.dwLowDateTime,
                "exe": os.path.normcase(buffer.value)}
    finally:
        kernel.CloseHandle(handle)


def port_open(port):
    try:
        with socket.create_connection(("127.0.0.1", port), timeout=0.5):
            return True
    except OSError:
        return False


def port_available(port):
    """Check wildcard listeners too; connecting to loopback misses LAN-only binds."""
    families = [(socket.AF_INET, "0.0.0.0")]
    if socket.has_ipv6:
        families.append((socket.AF_INET6, "::"))
    for family, address in families:
        try:
            with socket.socket(family, socket.SOCK_STREAM) as listener:
                if hasattr(socket, "SO_EXCLUSIVEADDRUSE"):
                    listener.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)
                if family == socket.AF_INET6:
                    listener.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 1)
                listener.bind((address, port))
        except OSError as error:
            # Python can have IPv6 support while the Windows adapter has it disabled.
            unsupported = (errno.EAFNOSUPPORT, errno.EPROTONOSUPPORT, errno.EADDRNOTAVAIL, 10047, 10049)
            if family == socket.AF_INET6 and error.errno in unsupported:
                continue
            return False
    return True


def find_available_port(preferred, reserved):
    for first, last in ((preferred, 65536), (1024, preferred)):
        for candidate in range(first, last):
            if candidate not in reserved and port_available(candidate):
                return candidate
    raise RuntimeError("No available TCP port found; provide explicit deployment ports")


@contextmanager
def operation_lock(root):
    """Reject concurrent management commands for the same runtime directory."""
    import msvcrt
    identity = hashlib.sha256(os.path.normcase(str(project_path(root).resolve())).encode("utf-8")).hexdigest()[:24]
    path = Path(tempfile.gettempdir()) / ("onepiece-deployment-" + identity + ".lock")
    with path.open("a+b") as stream:
        if stream.seek(0, os.SEEK_END) == 0:
            stream.write(b"\0")
            stream.flush()
        stream.seek(0)
        try:
            msvcrt.locking(stream.fileno(), msvcrt.LK_NBLCK, 1)
        except OSError:
            raise RuntimeError("Another deployment command is managing this runtime; wait for it to finish")
        try:
            yield
        finally:
            stream.seek(0)
            msvcrt.locking(stream.fileno(), msvcrt.LK_UNLCK, 1)


class Deployment:
    def __init__(self, root):
        self.root = project_path(root).resolve()
        self.marker = self.root / STATE_NAME
        self.state = read_json(self.marker) if self.marker.is_file() else None
        if self.state and self.state.get("format") != 1:
            raise RuntimeError("Unsupported deployment marker")
        self.credentials = None
        path = self.root / "private/credentials.json"
        if path.is_file():
            self.credentials = read_json(path)

    def save(self):
        write_json(self.marker, self.state)

    def require_installed(self):
        if not self.state or not self.credentials:
            raise RuntimeError("No deployment at this location. Run install first.")

    def runtime_directory(self):
        previous = self.state.get("pathAlias")
        record = ensure_alias(self.root, previous, any(self.owned(name) for name in ("database", "game")))
        if record != previous:
            self.state["pathAlias"] = record
            self.save()
        return Path(record["device"] + "/")

    def log(self, text):
        if self.credentials:
            for value in self.credentials.values():
                if isinstance(value, str) and len(value) > 12:
                    text = text.replace(value, "[redacted]")
        self.root.joinpath("logs").mkdir(exist_ok=True)
        with (self.root / "logs/deployment.log").open("a", encoding="utf-8") as stream:
            stream.write(datetime.now().isoformat() + " " + text + "\n")

    def run(self, command, label, input_data=None, output=None, timeout=180, input_stream=None):
        print(label, flush=True)
        result = subprocess.run([str(value) for value in command], input=input_data,
                                stdin=input_stream,
                                cwd=self.runtime_directory(),
                                stdout=output if output is not None else subprocess.PIPE,
                                stderr=subprocess.PIPE, timeout=timeout, creationflags=subprocess.CREATE_NO_WINDOW)
        # Command arguments may contain the initial root password; do not log the command line.
        data = (result.stdout or b"") if output is None else b""
        text = (data + (result.stderr or b"")).decode("utf-8", errors="replace")
        self.log(label + "\n" + text)
        if result.returncode:
            raise RuntimeError(label + " failed; see logs/deployment.log (exit " + str(result.returncode) + ")")
        return result

    def owned(self, name):
        record = self.state.get("processes", {}).get(name) if self.state else None
        return bool(record and process_info(record["pid"]) == record)

    def launch(self, name, command, cwd, port):
        if self.owned(name):
            if not port_open(port):
                raise RuntimeError(name + " is running but its configured port is unavailable; inspect its logs")
            print(name, "already running")
            return
        if port_open(port):
            raise RuntimeError("Port " + str(port) + " is occupied by an unowned process; choose another port")
        flags = subprocess.DETACHED_PROCESS | subprocess.CREATE_NEW_PROCESS_GROUP
        with (self.root / ("logs/" + name + ".stdout.log")).open("ab") as stdout, \
                (self.root / ("logs/" + name + ".stderr.log")).open("ab") as stderr:
            process = subprocess.Popen([str(value) for value in command], cwd=cwd, stdout=stdout,
                                       stderr=stderr, stdin=subprocess.DEVNULL, creationflags=flags)
        record = process_info(process.pid)
        if not record:
            raise RuntimeError(name + " exited immediately; inspect logs and Windows runtime prerequisites")
        self.state.setdefault("processes", {})[name] = record
        self.save()
        deadline = time.monotonic() + 60
        while time.monotonic() < deadline:
            if not self.owned(name):
                raise RuntimeError(name + " exited; inspect logs/" + name + ".stderr.log")
            if port_open(port):
                print(name, "started: PID", process.pid, "port", port)
                return
            time.sleep(1)
        raise RuntimeError(name + " startup timeout; process retained for inspection")

    def extract_runtime(self, name, offline):
        target = self.root / "runtimes" / name
        stamp = target / ".source-sha256"
        if stamp.is_file():
            if stamp.read_text().strip() != MANIFEST[name]["sha256"]:
                raise RuntimeError("Runtime version changed; use a fresh installation directory")
            return target
        if target.exists():
            raise RuntimeError("Unrecognized runtime directory: " + str(target))
        archive = download(name, offline)
        staging = self.root / "runtimes" / (name + "-extract-" + timestamp())
        staging.mkdir(parents=True)
        # Validate resolved Windows paths before extracting any archive contents.
        with zipfile.ZipFile(archive) as package:
            for item in package.infolist():
                if not (staging / item.filename).resolve().is_relative_to(staging.resolve()):
                    raise RuntimeError("Unsafe archive member")
            package.extractall(staging)
        if name == "java":
            candidates = list(staging.glob("*/bin/javac.exe"))
        else:
            candidates = list(staging.glob("*/bin/mariadbd.exe"))
        if len(candidates) != 1:
            raise RuntimeError("Unexpected runtime archive layout: " + str(staging))
        extracted = candidates[0].parent.parent
        if not extracted.resolve().is_relative_to(self.root) or not target.resolve().is_relative_to(self.root):
            raise RuntimeError("Runtime move escaped installation directory")
        os.replace(extracted, target)
        stamp.write_text(MANIFEST[name]["sha256"] + "\n", encoding="ascii")
        return target

    def database_command(self, tool="mariadb.exe"):
        directory = self.runtime_directory()
        return [directory / "runtimes/mariadb/bin" / tool,
                "--defaults-file=" + str(directory / "private/db-client.ini")]

    def ensure_db(self):
        directory = self.runtime_directory()
        if not self.owned("database"):
            # Paths are derived again after a stopped checkout/runtime is moved.
            self.write_database_options()
        self.launch("database", [directory / "runtimes/mariadb/bin/mariadbd.exe",
                                 "--defaults-file=" + str(directory / "dbdata/my.ini"), "--console"],
                    directory, self.state["dbPort"])
        deadline = time.monotonic() + 45
        while time.monotonic() < deadline:
            result = subprocess.run([str(x) for x in self.database_command()] + ["--connect-timeout=2", "-e", "SELECT 1"],
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                    cwd=directory,
                                    timeout=5, creationflags=subprocess.CREATE_NO_WINDOW)
            if result.returncode == 0:
                return
            if not self.owned("database"):
                break
            time.sleep(1)
        raise RuntimeError("Database did not accept configured credentials; inspect logs/database.stderr.log")

    def database_settings(self):
        directory = self.runtime_directory()
        return ("[mysqld]\n"
                    + 'basedir="' + (directory / "runtimes/mariadb").as_posix() + '"\n'
                    + 'datadir="' + (directory / "dbdata").as_posix() + '"\n'
                    + "port=" + str(self.state["dbPort"]) + "\nbind-address=127.0.0.1\n"
                    + "character-set-server=utf8\ncollation-server=utf8_bin\n"
                    + "innodb-buffer-pool-size=128M\nmax-connections=80\nsql-mode=\n")

    def write_database_options(self):
        if not (self.root / "dbdata/mysql").is_dir() or not self.credentials:
            raise RuntimeError("Database is not initialized; complete installation first")
        (self.root / "dbdata/my.ini").write_text(self.database_settings(), encoding="utf-8")
        (self.root / "private/db-client.ini").write_text(
            "[client]\nprotocol=tcp\nhost=127.0.0.1\nport=" + str(self.state["dbPort"])
            + "\nuser=root\npassword=" + self.credentials["rootPassword"]
            + "\ndefault-character-set=utf8\n", encoding="utf-8")

    def prepare_database(self):
        data = self.root / "dbdata"
        private = self.root / "private"
        private.mkdir(exist_ok=True)
        if self.credentials is None:
            if data.exists() and any(data.iterdir()):
                raise RuntimeError("Existing data directory has no owned credentials; refusing to initialize it")
            self.credentials = {"rootPassword": secrets.token_hex(24), "appPassword": secrets.token_hex(24)}
            write_json(private / "credentials.json", self.credentials)
        root_password = self.credentials["rootPassword"]
        if not (data / "mysql").is_dir():
            if data.exists() and any(data.iterdir()):
                raise RuntimeError("Partial database initialization detected; inspect it or choose a new installation directory")
            template = private / "db-template.ini"
            template.write_text(self.database_settings(), encoding="utf-8")
            directory = self.runtime_directory()
            initializer = directory / "runtimes/mariadb/bin/mariadb-install-db.exe"
            if not initializer.exists():
                initializer = initializer.with_name("mysql_install_db.exe")
            self.run([initializer, "--datadir=" + str(directory / "dbdata"), "--port=" + str(self.state["dbPort"]),
                      "--password=" + root_password, "--config=" + str(directory / "private/db-template.ini")],
                     "Initialize private database", timeout=240)
        self.ensure_db()
        imported = private / "schema-imported.json"
        importing = private / "schema-importing"
        if imported.exists():
            return
        if importing.exists():
            raise RuntimeError("An earlier SQL import did not finish. No re-import was attempted; inspect the database")
        importing.touch()
        self.run(self.database_command() + ["-e", "CREATE DATABASE IF NOT EXISTS dzm CHARACTER SET utf8 COLLATE utf8_bin"],
                 "Create application database")
        seed = REPO / "src/server/dzm.sql"
        self.run(self.database_command() + ["--database=dzm"], "Import bundled game schema", seed.read_bytes())
        stat_seed = REPO / "src/server/Code_Server/dzm_stat.sql"
        self.run(self.database_command(), "Import bundled statistics schema", stat_seed.read_bytes())
        password = self.credentials["appPassword"]
        grant = ""
        for host in ("localhost", "127.0.0.1"):
            grant += "CREATE USER IF NOT EXISTS 'onepiece'@'" + host + "' IDENTIFIED BY '" + password + "';"
            for database in ("dzm", "dzm_stat"):
                grant += "GRANT ALL ON `" + database + "`.* TO 'onepiece'@'" + host + "';"
        self.run(self.database_command(), "Create scoped game database account", grant.encode("utf-8"))
        write_json(imported, {"seedSha256": digest(seed), "importedAt": datetime.now().isoformat()})
        importing.unlink()

    def configure_server(self):
        source = REPO / "src/server/Code_Server/server"
        target = self.root / "server"
        target.mkdir(exist_ok=True)
        shutil.copy2(source / "DZM.jar", target / "DZM.jar")
        for name in ("xml", "hibernateconfig"):
            shutil.copytree(source / name, target / name, dirs_exist_ok=True)
        config = target / "xml/config.xml"
        tree = ET.parse(config)
        for query, value in (("connection/address", "0.0.0.0"), ("connection/port", str(self.state["gamePort"])),
                             ("Platform", "lo"), ("ServerId", "01"), ("CanUseGm", "0"),
                             ("ResourceVersion", self.state["resourceVersion"])):
            node = tree.find(query)
            if node is None:
                raise RuntimeError("Missing server configuration field: " + query)
            node.text = value
        tree.write(config, encoding="UTF-8", xml_declaration=True)
        hibernate = target / "hibernateconfig/hibernate_lo_01.cfg.xml"
        text = hibernate.read_text(encoding="utf-8-sig")
        values = {"connection.url": "jdbc:mysql://127.0.0.1:" + str(self.state["dbPort"]) + "/dzm?useUnicode=true&characterEncoding=UTF-8",
                  "connection.username": "onepiece", "connection.password": self.credentials["appPassword"]}
        # Hibernate 3 validates its DTD. ElementTree serialization would discard
        # the DOCTYPE, so replace property values while preserving the template.
        for name, value in values.items():
            pattern = r'(<property\s+name=["\']' + re.escape(name) + r'["\']\s*>).*?(</property>)'
            text, count = re.subn(pattern, lambda match: match.group(1) + escape(value) + match.group(2),
                                  text, flags=re.DOTALL)
            if count != 1:
                raise RuntimeError("Hibernate template is missing or duplicates property: " + name)
        hibernate.write_text(text, encoding="utf-8")

    def configure_resources(self):
        command = [sys.executable, REPO / "src/tools/configure_deployment.py", "--host", self.state["host"],
                   "--web-port", str(self.state["webPort"]), "--game-port", str(self.state["gamePort"]),
                   "--resource-version", self.state["resourceVersion"], "--channel", self.state["channel"],
                   "--local-login", "--web-root", self.root / "web"]
        self.run(command, "Generate public resources and Android deployment overlays", timeout=300)
        configs = self.root / "client-config"
        configs.mkdir(exist_ok=True)
        resources = self.root / "web/gameserver/dzm/1.8"
        for name in ("version_android.cfg", "version_ios_local.cfg"):
            shutil.copy2(resources / name, configs / name)
        (configs / "game.properties").write_text("localDebugLogin=true\n", encoding="utf-8")
        write_json(configs / "deployment.json", {key: self.state[key] for key in
                   ("host", "webPort", "gamePort", "resourceVersion", "channel")})

    def select_install_ports(self, args):
        items = (("web_port", "webPort", 18081), ("game_port", "gamePort", 25524),
                 ("db_port", "dbPort", 13306))
        selected, automatic = {}, []
        for argument, field, preferred in items:
            value = getattr(args, argument)
            if value is None and self.state:
                if field not in self.state:
                    raise RuntimeError("Deployment marker is missing " + field + "; inspect its configuration")
                value = self.state[field]
            if value is None:
                automatic.append((argument, preferred))
            elif not isinstance(value, int) or not 1024 <= value <= 65535:
                raise RuntimeError("Deployment ports must be integers in 1024..65535")
            else:
                selected[argument] = value
        if len(set(selected.values())) != len(selected):
            raise RuntimeError("Use different resource, game and database ports")
        # Leave the default WebSocket gateway port free when selecting new ports.
        reserved = set(selected.values()) | {18082}
        for argument, preferred in automatic:
            chosen = find_available_port(preferred, reserved)
            selected[argument] = chosen
            reserved.add(chosen)
            if chosen != preferred:
                print("Port", preferred, "unavailable; selected", chosen, "for", argument.replace("_", " "), flush=True)
        for argument, value in selected.items():
            setattr(args, argument, value)
        if not self.state:
            print("Deployment ports: resources", args.web_port, "game", args.game_port,
                  "database", args.db_port, "(loopback only)", flush=True)

    def install(self, args):
        if (self.root == REPO or REPO.is_relative_to(self.root)
                or (self.root.is_relative_to(REPO) and not self.root.is_relative_to(REPO / "runtime"))):
            raise RuntimeError("Use runtime/server (or runtime/another-name) inside the checkout, or a separate data directory outside it")
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]*", args.host or "") or args.host in ("localhost", "127.0.0.1", "10.0.2.2"):
            raise RuntimeError("Provide a phone-reachable LAN IPv4 address or DNS hostname")
        if self.root.exists() and any(self.root.iterdir()) and not self.state:
            raise RuntimeError("Install directory contains unowned files; choose an empty directory")
        self.select_install_ports(args)
        ports = (args.web_port, args.game_port, args.db_port)
        if not re.fullmatch(r"\d+\.\d+\.\d+", args.resource_version):
            raise RuntimeError("Resource version must have three numeric components")
        for name in ("src/server/Code_Server/server/DZM.jar", "src/server/dzm.sql", "src/server/Code_Server/dzm_stat.sql",
                     "src/tools/configure_deployment.py", "src/server/ResourcesAndroid_Encrypt"):
            if not (REPO / name).exists():
                raise RuntimeError("Copy the full src directory; missing " + name)
        requested = {"host": args.host, "webPort": args.web_port, "gamePort": args.game_port,
                     "dbPort": args.db_port, "resourceVersion": args.resource_version, "channel": args.channel}
        if self.state and any(self.owned(name) for name in ("database", "web", "game", "gateway", "tunnel")):
            if all(self.state.get(key) == value for key, value in requested.items()) and self.state.get("installed"):
                self.start()
                return
            raise RuntimeError("Stop this deployment before changing its configuration")
        for port in ports:
            if not port_available(port):
                raise RuntimeError("Port " + str(port) + " is occupied or reserved; saved/explicit ports are not reassigned. "
                                   "Stop the conflicting service, or stop this deployment and specify different ports")
        self.root.mkdir(parents=True, exist_ok=True)
        (self.root / "logs").mkdir(exist_ok=True)
        self.state = dict(self.state or {"format": 1, "processes": {}}, **requested)
        self.state["installed"] = False
        self.save()
        self.extract_runtime("java", args.offline)
        self.extract_runtime("mariadb", args.offline)
        self.prepare_database()
        self.configure_server()
        self.configure_resources()
        self.state["installed"] = True
        self.save()
        self.start()
        print("Client configuration:", self.root / "client-config")

    def start(self):
        self.require_installed()
        if (self.root / "private/restore-in-progress").exists():
            raise RuntimeError("Finish the interrupted database restore before starting gameplay")
        if not self.state.get("installed"):
            raise RuntimeError("Installation is incomplete; inspect logs and re-run install")
        self.ensure_db()
        directory = self.runtime_directory()
        self.launch("web", [sys.executable, "-m", "http.server", str(self.state["webPort"]),
                            "--bind", "0.0.0.0", "--directory", self.root / "web"], self.root, self.state["webPort"])
        self.launch("game", [directory / "runtimes/java/bin/java.exe", "-Xms64m", "-Xmx512m",
                             "-Dfile.encoding=UTF-8", "-jar", "DZM.jar"], directory / "server", self.state["gamePort"])
        self.status()

    def terminate_owned(self, name):
        if not self.owned(name):
            return
        record = self.state["processes"][name]
        kernel = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
        kernel.OpenProcess.restype = wintypes.HANDLE
        kernel.TerminateProcess.argtypes = [wintypes.HANDLE, wintypes.UINT]
        kernel.CloseHandle.argtypes = [wintypes.HANDLE]
        handle = kernel.OpenProcess(0x1001, False, record["pid"])
        if not handle:
            raise RuntimeError("Cannot open owned process: " + name)
        try:
            # Recheck identity while holding a handle so PID reuse cannot redirect termination.
            if process_info(record["pid"]) != record:
                raise RuntimeError("Process identity changed; refusing to stop " + name)
            if not kernel.TerminateProcess(handle, 0):
                raise RuntimeError("Cannot stop " + name)
        finally:
            kernel.CloseHandle(handle)
        deadline = time.monotonic() + 15
        while self.owned(name) and time.monotonic() < deadline:
            time.sleep(0.2)
        if self.owned(name):
            raise RuntimeError(name + " did not exit")
        self.state["processes"].pop(name, None)
        self.save()

    def stop(self):
        self.require_installed()
        self.terminate_owned("tunnel")
        self.terminate_owned("gateway")
        self.terminate_owned("game")
        self.terminate_owned("web")
        if self.owned("database"):
            self.run(self.database_command("mariadb-admin.exe") + ["shutdown"], "Shut down owned database", timeout=60)
            deadline = time.monotonic() + 60
            while self.owned("database") and time.monotonic() < deadline:
                time.sleep(0.5)
            if self.owned("database"):
                raise RuntimeError("Database is still running; no forced database termination was attempted")
            self.state["processes"].pop("database", None)
            self.save()
        if self.state.get("pathAlias"):
            release_alias(self.root, self.state["pathAlias"])
            self.state.pop("pathAlias")
            self.save()
        print("Owned deployment stopped. Player data has been retained.")

    def status(self):
        self.require_installed()
        if self.state.get("pathAlias"):
            print("Runtime drive:", self.state["pathAlias"]["device"], "->", self.root)
        for name, port in (("database", self.state["dbPort"]), ("web", self.state["webPort"]), ("game", self.state["gamePort"])):
            owned = self.owned(name)
            print(name + ":", "owned/running" if owned else "not running", "port", port,
                  "listening" if port_open(port) else "closed")
        print("Phone server:", self.state["host"] + ":" + str(self.state["gamePort"]))
        print("Resource URL: http://" + self.state["host"] + ":" + str(self.state["webPort"]) + "/gameserver/dzm/1.8/servers_list.php")
        if self.state.get("external"):
            port = self.state["external"]["port"]
            print("gateway:", "owned/running" if self.owned("gateway") else "not running", "loopback port", port)
            print("tunnel:", "owned/running" if self.owned("tunnel") else "not running")
            print("External URL/invitation: use Start-External.ps1 -Action status with the same InstallDir")
        print("Runtime:", self.root)

    def backup(self):
        self.require_installed()
        if any(self.owned(name) for name in ("game", "gateway", "tunnel")):
            raise RuntimeError("Stop the deployment before backup so gameplay writes cannot change during export")
        self.ensure_db()
        folder = self.root / "backups"
        folder.mkdir(exist_ok=True)
        output = folder / ("onepiece-" + timestamp() + ".sql.gz")
        temporary = output.with_suffix(output.suffix + ".partial")
        # Stream the dump to disk, then compress; no PowerShell text pipeline alters SQL encoding.
        raw = folder / (output.stem + ".sql.partial")
        try:
            with raw.open("wb") as stream:
                self.run(self.database_command("mariadb-dump.exe") + ["--single-transaction", "--routines", "--events",
                    "--triggers", "--hex-blob", "--databases", "dzm", "dzm_stat"], "Export database backup", output=stream, timeout=300)
            with raw.open("rb") as source, gzip.open(temporary, "wb") as compressed:
                shutil.copyfileobj(source, compressed, 1024 * 1024)
            os.replace(temporary, output)
        finally:
            if raw.exists():
                raw.unlink()
        output.with_suffix(output.suffix + ".sha256").write_text(digest(output) + "  " + output.name + "\n", encoding="ascii")
        print("Backup:", output)
        print("Database remains running; game and web were not started.")
        return output

    def restore(self, args):
        self.require_installed()
        if not args.confirm_restore or not args.restore_file:
            raise RuntimeError("Restore replaces game data. Supply -RestoreFile and -ConfirmRestore explicitly.")
        source = project_path(args.restore_file).resolve()
        if not source.is_file() or not source.name.endswith(".sql.gz"):
            raise RuntimeError("Use a .sql.gz backup created by this deployment tool")
        checksum = source.with_suffix(source.suffix + ".sha256")
        if not checksum.is_file() or digest(source) != checksum.read_text().split()[0]:
            raise RuntimeError("Restore backup is missing its checksum file or is corrupted")
        if any(self.owned(name) for name in ("game", "web", "gateway", "tunnel")):
            raise RuntimeError("Stop game and web before restore")
        self.ensure_db()
        self.backup()
        flag = self.root / "private/restore-in-progress"
        flag.write_text(str(source), encoding="utf-8")
        # An interrupted restore leaves a marker that blocks starting gameplay.
        expanded = self.root / "private/restore.sql.partial"
        try:
            with gzip.open(source, "rb") as data, expanded.open("wb") as output:
                shutil.copyfileobj(data, output, 1024 * 1024)
            with expanded.open("rb") as data:
                self.run(self.database_command(), "Restore game database", input_stream=data, timeout=600)
        finally:
            if expanded.exists():
                expanded.unlink()
        flag.unlink()
        print("Restore completed. Run start when ready.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("install", "start", "stop", "status", "backup", "restore", "download"))
    parser.add_argument("--install-dir", type=Path, default=DEFAULT_INSTALL_DIR)
    parser.add_argument("--host")
    parser.add_argument("--web-port", type=int, help="Use a fixed port; omitted: reuse saved port or choose an available port")
    parser.add_argument("--game-port", type=int, help="Use a fixed port; omitted: reuse saved port or choose an available port")
    parser.add_argument("--db-port", type=int, help="Use a fixed loopback port; omitted: reuse saved port or choose an available port")
    parser.add_argument("--resource-version", default="1.78.6")
    parser.add_argument("--channel", choices=("anzhi", "all"), default="anzhi")
    parser.add_argument("--offline", action="store_true")
    parser.add_argument("--restore-file", type=Path)
    parser.add_argument("--confirm-restore", action="store_true")
    args = parser.parse_args()
    args.install_dir = project_path(args.install_dir)
    if os.name != "nt":
        parser.error("This server deployment is for Windows x64")
    if args.action == "download":
        for name in MANIFEST:
            download(name, args.offline)
        print("Offline download cache prepared:", CACHE)
        return
    with operation_lock(args.install_dir):
        deployment = Deployment(args.install_dir)
        if args.action in ("start", "install") and (deployment.root / "private/restore-in-progress").exists():
            raise RuntimeError("An earlier restore did not finish; inspect or complete restore before starting gameplay")
        if args.action == "install":
            deployment.install(args)
        elif args.action == "restore":
            deployment.restore(args)
        else:
            getattr(deployment, args.action)()


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError, zipfile.BadZipFile) as error:
        print("ERROR:", str(error), file=sys.stderr)
        sys.exit(1)
