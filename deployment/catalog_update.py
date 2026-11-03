"""Apply additive catalog releases to an owned, stopped deployment without SQL."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import sys
import tempfile
import xml.etree.ElementTree as ET

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "scripts"))
from configure_deployment import crc16
from import_apk_catalog import merge_table, merge_xml

CATALOG = REPO / "resources/catalog/v330.json"


def release_digest(catalog_path=None):
    catalog_path = catalog_path or CATALOG
    return hashlib.sha256(catalog_path.read_bytes()).hexdigest()


def pending(root, catalog_path=None, marker_name="catalog-v330.json"):
    marker = root / "private" / marker_name
    return not marker.is_file() or json.loads(marker.read_text(encoding="utf-8")).get("sha256") != release_digest(catalog_path)


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def apply(root, state, catalog_path=None, marker_name="catalog-v330.json"):
    """Stage and conflict-check all changes before replacing runtime files.

    Caller must hold the deployment lock and ensure the game is stopped. An
    interrupted update is reapplied before launch; no player rows are touched.
    """
    catalog_path = catalog_path or CATALOG
    if not pending(root, catalog_path, marker_name):
        return False
    catalog = json.loads(catalog_path.read_text(encoding="utf-8"))
    release = release_digest(catalog_path)
    private = root / "private"
    private.mkdir(exist_ok=True)
    public = Path("web/gameserver/dzm/1.8")
    version = max((state["resourceVersion"], catalog["resourceVersion"]),
                  key=lambda text: tuple(int(value) for value in text.split(".")))
    with tempfile.TemporaryDirectory(prefix="catalog-", dir=private) as work:
        stage = Path(work)
        destinations = []

        def copy_for_update(relative):
            destination = stage / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(root / relative, destination)
            destinations.append(relative)
            return destination

        for name, spec in catalog["serverTemplates"].items():
            merge_xml(copy_for_update(Path("server/xml") / name), spec)
        for name, spec in catalog.get("serverFiles", {}).items():
            if Path(name).name != name:
                raise RuntimeError("Invalid server release filename: " + name)
            source = REPO / "server/config/xml" / name
            if not spec.get("allowLocalOverride") and hashlib.sha256(source.read_bytes()).hexdigest() != spec["sha256"]:
                raise RuntimeError("Server activity configuration differs from release: " + name)
            relative = Path("server/xml") / name
            # Probability and reward rules may be edited for this deployment.
            # An upgrade only supplies the default when no local config exists.
            if spec.get("preserveExisting") and (root / relative).is_file():
                continue
            destination = stage / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, destination)
            destinations.append(relative)
        for name, additions in catalog["clientTables"].items():
            merge_table(copy_for_update(public / name), additions, catalog.get("clientPreviousRows", {}).get(name))
        for name, expected in catalog["assets"].items():
            source = REPO / "resources/client" / name
            if hashlib.sha256(source.read_bytes()).hexdigest() != expected:
                raise RuntimeError("Imported asset checksum differs from catalog: " + name)
            relative = public / name
            destination = stage / relative
            if (root / relative).is_file() and (root / relative).read_bytes() != source.read_bytes():
                raise RuntimeError("Custom runtime asset conflicts with imported content: " + name)
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, destination)
            destinations.append(relative)

        for name, hashes in catalog.get("sharedAssets", {}).items():
            source = REPO / "resources/client" / name
            if hashlib.sha256(source.read_bytes()).hexdigest() != hashes["afterSha256"]:
                raise RuntimeError("Expanded font checksum differs from catalog: " + name)
            relative = public / name
            existing = hashlib.sha256((root / relative).read_bytes()).hexdigest()
            if existing not in (hashes["beforeSha256"], hashes["afterSha256"]):
                raise RuntimeError("Custom runtime font requires a manual merge: " + name)
            destination = stage / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, destination)
            destinations.append(relative)

        config = copy_for_update(Path("server/xml/config.xml"))
        tree = ET.parse(config)
        tree.find("ResourceVersion").text = version
        tree.write(config, encoding="utf-8", xml_declaration=True)
        for name in ("version_android.cfg", "version_win32.cfg", "version_ios_local.cfg", "servers_list.php"):
            path = copy_for_update(public / name)
            value = json.loads(path.read_text(encoding="utf-8-sig"))
            value["severVerson" if name == "servers_list.php" else "localVerson"] = version
            write_json(path, value)
        for name in ("version_android.cfg", "version_ios_local.cfg"):
            relative = Path("client-config") / name
            if (root / relative).is_file():
                path = copy_for_update(relative)
                value = json.loads(path.read_text(encoding="utf-8-sig"))
                value["localVerson"] = version
                write_json(path, value)
        relative = Path("client-config/deployment.json")
        if (root / relative).is_file():
            path = copy_for_update(relative)
            value = json.loads(path.read_text(encoding="utf-8"))
            value["resourceVersion"] = version
            write_json(path, value)
        relative = Path("web/.onepiece-deployment.json")
        if (root / relative).is_file():
            path = copy_for_update(relative)
            value = json.loads(path.read_text(encoding="utf-8"))
            value["resourceVersion"] = version
            write_json(path, value)

        # Retain the deployment's published file set, including custom content.
        update = copy_for_update(public / "update.php")
        manifest = json.loads(update.read_text(encoding="utf-8-sig"))
        existing_entries = {item["f"].lstrip("/"): item for item in manifest["files"]}
        names = set(existing_entries)
        names.update(catalog["clientTables"])
        names.update(catalog["assets"])
        names.update(catalog.get("sharedAssets", {}))
        entries = []
        for name in sorted(names):
            relative = Path(name)
            if relative.is_absolute() or ".." in relative.parts or "\\" in name or ":" in name:
                raise RuntimeError("Invalid resource manifest path: " + name)
            path = stage / public / relative
            if not path.is_file():
                path = root / public / relative
                if name in existing_entries and path.is_file():
                    entries.append(existing_entries[name])
                    continue
            data = path.read_bytes()
            if data:
                entries.append({"f": "/" + name, "s": len(data), "c": crc16(data)})
        manifest.update(severVersion=version, files=entries, totalByteSize=sum(item["s"] for item in entries))
        write_json(update, manifest)

        changes = [relative for relative in destinations if not (root / relative).is_file()
                   or (root / relative).read_bytes() != (stage / relative).read_bytes()]
        backup = private / "catalog-backups" / release[:16]
        for relative in changes:
            original = root / relative
            saved = backup / relative
            if original.is_file() and not saved.exists():
                saved.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(original, saved)
        for relative in changes:
            destination = root / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            os.replace(stage / relative, destination)
    state["resourceVersion"] = version
    # Persist state here as well: an interruption before the caller's save must
    # not leave a marker claiming completion alongside the previous version.
    state_path = root / ".onepiece-server.json"
    temporary = state_path.with_name(state_path.name + ".catalog-writing")
    write_json(temporary, state)
    os.replace(temporary, state_path)
    marker = private / marker_name
    temporary = marker.with_name(marker.name + ".writing")
    write_json(temporary, {"sha256": release, "resourceVersion": version, "release": catalog["id"]})
    os.replace(temporary, marker)
    if "Disciples.txt" in catalog["clientTables"]:
        print("Applied basic content catalog: %s cards, %s equipment, %s skills; resource version %s" % (
            len(catalog["clientTables"]["Disciples.txt"]), len(catalog["clientTables"]["Equips.txt"]),
            len(catalog["clientTables"]["Skills.txt"]), version), flush=True)
    elif catalog["id"] == "expansion-systems-v1":
        print("Applied treasure transport and growth systems; resource version " + version, flush=True)
    else:
        print("Applied permanent activities: All Blue and Fortune Wheel; resource version " + version, flush=True)
    return True
