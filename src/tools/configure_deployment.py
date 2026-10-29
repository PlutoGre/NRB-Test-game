"""Generate matching client overlays, public web resources and server settings.

Requires Python 3. Does not compile, package, sign or start any program.
"""
import argparse
import json
from pathlib import Path
import re
import shutil
import sys
import xml.etree.ElementTree as ET

sys.path.insert(0, str(Path(__file__).resolve().parent))
from project_paths import project_path, project_root


SOURCE = project_root() / "src"
ANDROID = SOURCE / "client/Game/proj.android"
RESOURCE_SOURCE = SOURCE / "server/ResourcesAndroid_Encrypt"
CRC_NIBBLES = (0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
               0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400)


def crc16(data):
    # Match GameMaths::GetCRC16, including the initial value of zero.
    value = 0
    for byte in data:
        value = (value >> 4) ^ CRC_NIBBLES[value & 15] ^ CRC_NIBBLES[byte & 15]
        value = (value >> 4) ^ CRC_NIBBLES[value & 15] ^ CRC_NIBBLES[byte >> 4]
    return value


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", required=True, help="Host reachable from the client; emulator: 10.0.2.2")
    parser.add_argument("--web-port", type=int, default=18081)
    parser.add_argument("--game-port", type=int, default=25524)
    parser.add_argument("--web-root", type=Path, default=Path("runtime/server/web"), help="Project-relative public web directory")
    parser.add_argument("--channel", default="anzhi", help="Ant channel directory name, or all")
    parser.add_argument("--platform", choices=("android", "ios"), default="android")
    parser.add_argument("--ios-overlay", type=Path, help="iOS asset overlay output directory")
    parser.add_argument("--local-login", action="store_true", help="Explicitly enable local device login")
    parser.add_argument("--resource-version", default="1.78.6", help="Bump when replacing cached APK assets")
    parser.add_argument("--server-dir", type=Path, help="Optional game server directory to configure")
    args = parser.parse_args()
    if args.server_dir:
        args.server_dir = project_path(args.server_dir)
    if args.ios_overlay:
        args.ios_overlay = project_path(args.ios_overlay)
    if not re.fullmatch(r"[A-Za-z0-9_.-]+", args.host):
        parser.error("host must be an IPv4 address or DNS hostname, without URL/port/path")
    if not (1 <= args.web_port <= 65535 and 1 <= args.game_port <= 65535):
        parser.error("ports must be between 1 and 65535")
    if not re.fullmatch(r"\d+\.\d+\.\d+", args.resource_version):
        parser.error("resource-version must have three numeric components")
    root = project_path(args.web_root).resolve()
    marker = root / ".onepiece-deployment.json"
    if root == RESOURCE_SOURCE.resolve() or RESOURCE_SOURCE.resolve() in root.parents:
        parser.error("web-root must not be inside the source resource directory")
    if root.exists() and not root.is_dir():
        parser.error("web-root is not a directory")
    if root.exists() and any(root.iterdir()) and not marker.is_file():
        parser.error("web-root is not an empty or previously generated deployment directory")
    channels = sorted((ANDROID / "antbuild").glob("*/build.xml"))
    selected = [p.parent for p in channels if args.channel == "all" or p.parent.name == args.channel]
    if args.platform == "ios":
        selected = []
        args.local_login = True
        if args.host in ("127.0.0.1", "localhost", "10.0.2.2"):
            parser.error("an iPhone needs a reachable LAN/public host, not loopback or an Android emulator alias")
    elif not selected:
        parser.error("unknown channel: " + args.channel)
    if args.server_dir and not (args.server_dir / "xml/config.xml").is_file():
        parser.error("server-dir has no xml/config.xml")
    base = "http://" + args.host + (":" + str(args.web_port) if args.web_port != 80 else "")
    resource_url = base + "/gameserver/dzm/1.8/"
    resource_root = root / "gameserver/dzm/1.8"
    resource_root.mkdir(parents=True, exist_ok=True)
    # Only publish client assets; do not expose the bundled legacy PHP administration tree.
    excluded = {".php", ".html", ".js", ".xls", ".xlsx", ".doc", ".docx", ".sql", ".pem", ".zip"}
    asset_names = set()
    for source in RESOURCE_SOURCE.rglob("*"):
        if not source.is_file():
            continue
        relative = source.relative_to(RESOURCE_SOURCE)
        if "summary_dzm" in relative.parts or source.suffix.lower() in excluded:
            continue
        destination = resource_root / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
        asset_names.add(relative.as_posix())
    version = {"version": 1, "localVerson": args.resource_version,
               "sever": resource_url + "servers_list.php",
               "severBackup": resource_url + "servers_list.php", "branch": "MAIN"}
    for name in ("version_android.cfg", "version_win32.cfg", "version_ios_local.cfg"):
        write_json(resource_root / name, version)
        asset_names.add(name)
    if args.platform == "ios":
        overlay = args.ios_overlay or SOURCE / "client/ios-modern/generated/assets"
        write_json(overlay / "version_ios_local.cfg", version)
    for channel in selected:
        overlay = channel / "deployment"
        write_json(overlay / "version_android.cfg", version)
        (overlay / "game.properties").write_text(
            "# Local device login is opt-in; ordinary channel login remains the default.\n"
            + "localDebugLogin=" + str(args.local_login).lower() + "\n", encoding="utf-8")
    manifest = []
    for name in sorted(asset_names):
        data = (resource_root / name).read_bytes()
        if data:
            manifest.append({"f": "/" + name, "s": len(data), "c": crc16(data)})
    write_json(resource_root / "update.php", {"version": 1, "severVersion": args.resource_version,
                                              "files": manifest,
                                              "totalByteSize": sum(f["s"] for f in manifest)})
    write_json(resource_root / "servers_list.php", {
        "version": 1, "severVerson": args.resource_version,
        "updateAddress": resource_url + "update.php", "rootAddress": resource_url,
        "defaultSeverID": 1, "announcement": "", "updateByteSize": 0,
        "severs": [{"id": 1, "name": "Local", "address": args.host,
                    "port": args.game_port, "state": "new"}]})
    if args.server_dir:
        config = args.server_dir / "xml/config.xml"
        backup = config.with_name("config.xml.before-deployment")
        if not backup.exists():
            shutil.copy2(config, backup)
        tree = ET.parse(config)
        tree.find("connection/port").text = str(args.game_port)
        tree.find("ResourceVersion").text = args.resource_version
        tree.write(config, encoding="UTF-8", xml_declaration=True)
    write_json(marker, {"host": args.host, "webPort": args.web_port, "gamePort": args.game_port,
                        "resourceVersion": args.resource_version,
                        "localLogin": args.local_login,
                        "channels": [p.name for p in selected]})
    print("Generated public web root:", root)
    print("Generated channel overlays:", ", ".join(p.name for p in selected))
    print("Serve this web root and rebuild the selected client before using the new deployment.")
    if args.host in ("127.0.0.1", "localhost"):
        print("WARNING: loopback addresses only work for a client on the server computer, not a phone")


if __name__ == "__main__":
    main()
