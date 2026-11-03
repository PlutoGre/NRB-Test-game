"""Import the first basic-content batch from the frozen 3.3.0 APK assets.

This is a data migration, not a compiler. Existing template IDs are never replaced.
Run only when intentionally importing content; normal deployment uses catalog.json.
"""
import argparse
import csv
import hashlib
import json
from pathlib import Path
import re
import shutil
from xml.sax.saxutils import quoteattr
import xml.etree.ElementTree as ET
import zlib

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "resources/catalog/v330.json"
KEY = b"youaiABC"
SKILLS = {78, 79, 83, 84, 85, 86, 87, 88, 89}
PENDING = {208, 209, 225, 226, 229, 343}
SOURCE_HASHES = {
    "Disciples.txt": "9165c15d413a8646a4b71fd9005dd8c9ed0a9ea29a832edd101504070a7cf627",
    "Equips.txt": "0a1c965200589faf16affa8ef4fa544b109c79a67291dd10e0e866e7a374b671",
    "Skills.txt": "f7d3af24de0370916b4fb3d62db9f19a57cd9c4ab144856377940aecc88e8c80",
    "Yuanfens.txt": "31300939ede9336f4d6c3de772a9276d58e1daa72b36ecdb06155585be307efe",
    "CrewEvolution.txt": "faf639a1347a95e37a1f506d2788745625f79c1c8ac8409949a18667b7e5ce54",
}
FONT_BASE_HASHES = {
    "Lang/heiOutline24.fnt": "258b0f63b18a1b25fe09d17e676bb2e3cd16f9809e8e29ae7a5a897bf25928b9",
    "Lang/heiOutline24_0.png": "c9ab7121371f2ec3c560d560d6e981a6d7ac1edf73bcd2d1dc12ddbcd1651615",
}


def decode(data):
    if data.startswith(b"\xef\xfe"):
        raw = bytes(v ^ KEY[i % len(KEY)] for i, v in enumerate(data[2:]))
        data = zlib.decompress(raw[1:])
    try:
        return data.decode("utf-8-sig")
    except UnicodeDecodeError:
        return data.decode("gb18030")


def rows(path):
    return {int(row[0]): row for row in csv.reader(decode(path.read_bytes()).splitlines()[1:], delimiter="\t")
            if row and row[0].isdigit()}


def merge_table(path, additions, previous_rows=None):
    """Preserve every original row; reject a conflicting existing ID."""
    original = path.read_bytes()
    text = decode(original)
    existing = {int(line.split()[0]): line.split() for line in text.splitlines()[1:]
                if line.split() and line.split()[0].isdigit()}
    added, replaced = [], False
    for row in additions:
        item_id = int(row[0])
        if item_id in existing:
            if existing[item_id] != row:
                if existing[item_id] != (previous_rows or {}).get(str(item_id)):
                    raise ValueError("Conflicting client template: %s #%s" % (path, item_id))
                lines = text.splitlines(keepends=True)
                for index, line in enumerate(lines):
                    fields = line.split()
                    if fields and fields[0] == str(item_id):
                        ending = "\r\n" if line.endswith("\r\n") else "\n"
                        lines[index] = "\t".join(row) + ending
                text, replaced = "".join(lines), True
        else:
            added.append("\t".join(row))
    if not added and not replaced:
        return
    ending = "\r\n" if "\r\n" in text else "\n"
    plain = (text.rstrip("\r\n") + ending + (ending.join(added) + ending if added else "")).encode("utf-8")
    if original.startswith(b"\xef\xfe"):
        blocks = len(plain) // 8192 + 1
        if blocks > 255:
            raise ValueError("Legacy compressed table exceeds its block header: " + str(path))
        # The native decoder allocates from the unencrypted first byte, then XORs it.
        packed = bytes([blocks]) + zlib.compress(plain)
        plain = b"\xef\xfe" + packed[:1] + bytes(v ^ KEY[(i + 1) % 8] for i, v in enumerate(packed[1:]))
    elif original.startswith(b"\xef\xbb\xbf"):
        plain = b"\xef\xbb\xbf" + plain
    path.write_bytes(plain)


def merge_xml(path, spec):
    original = path.read_bytes()
    data = original.decode("utf-8-sig")
    ending = "\r\n" if "\r\n" in data else "\n"
    tree = ET.fromstring(data)
    existing = {node.get("id"): node for node in tree.findall(spec["container"] + "/" + spec["tag"])}
    added, replaced = [], False
    for attrs in spec["rows"]:
        node = existing.get(attrs["id"])
        if node is not None:
            if node.attrib != attrs:
                if node.attrib != spec.get("previousRows", {}).get(attrs["id"]):
                    raise ValueError("Conflicting server template: %s #%s" % (path, attrs["id"]))
                replacement = "<%s %s/>" % (spec["tag"], " ".join(k + "=" + quoteattr(v) for k, v in attrs.items()))
                pattern = r"<" + re.escape(spec["tag"]) + r"\b[^>]*\bid=[\"']" + re.escape(attrs["id"]) + r"[\"'][^>]*/>"
                data, count = re.subn(pattern, lambda match: replacement, data)
                if count != 1:
                    raise ValueError("Cannot replace the known malformed template: " + str(path))
                replaced = True
        else:
            added.append("\t\t<%s %s/>" % (spec["tag"], " ".join(k + "=" + quoteattr(v) for k, v in attrs.items())))
    if added:
        end = "\t</" + spec["container"] + ">"
        if data.count(end) != 1:
            raise ValueError("Unexpected XML container: " + str(path))
        data = data.replace(end, ending.join(added) + ending + end)
    if added or replaced:
        path.write_bytes((b"\xef\xbb\xbf" if original.startswith(b"\xef\xbb\xbf") else b"") + data.encode("utf-8"))


def compact(row):
    # The legacy C++ reader tokenizes whitespace rather than parsing TSV quoting.
    return [re.sub(r"\s+", "", str(cell)) or "none" for cell in row]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets", required=True, type=Path, help="Extracted assets of the audited APK, not hot-update assets")
    args = parser.parse_args()
    assets = args.assets.resolve()
    for name, expected in SOURCE_HASHES.items():
        if hashlib.sha256((assets / "txt" / name).read_bytes()).hexdigest() != expected:
            raise ValueError("Use the audited APK assets, not online or modified tables: " + name)
    files = {p.relative_to(assets).as_posix().lower(): p for p in assets.rglob("*") if p.is_file()}
    source = {name: rows(assets / "txt" / name) for name in
              ("Disciples.txt", "Equips.txt", "Skills.txt", "Yuanfens.txt", "CrewEvolution.txt")}
    # Candidate IDs are fixed so a later execution cannot accidentally import advanced forms.
    card_ids = sorted((set(range(201, 251)) - {243}) | {343})
    equip_ids = [92, 93, 94, 95, 96, 98, 101, 102, 103, 106, 107, 108, 109, 110, 112,
                 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126]
    ready = [i for i in card_ids if i not in PENDING]
    fate_ids = sorted({int(v) for i in ready for v in source["Disciples.txt"][i][11:17] if int(v)})
    fate_map = {i: 689 + index for index, i in enumerate(fate_ids)}
    tables = {name: [] for name in ("Disciples.txt", "Equips.txt", "Skills.txt", "Yuanfens.txt")}
    server = {"disciple.xml": {"container": "Disciples", "tag": "Disciple", "rows": []},
              "equip.xml": {"container": "Equips", "tag": "Equip", "rows": []},
              "skill.xml": {"container": "Skills", "tag": "Skill", "rows": []},
              "fate.xml": {"container": "Fates", "tag": "Fate", "rows": []}}
    copied, adjustments = {}, []

    def art(value, fallback=None):
        corrected = value.replace(".mp3.mp3", ".mp3")
        path = files.get(corrected.lower())
        if path is None and corrected.lower().endswith(".mp3") and "/" not in corrected:
            path = files.get("audio/" + corrected.lower())
        if path is None:
            if fallback is not None:
                adjustments.append({"source": value, "replacement": fallback, "reason": "legacy name sprite is hidden; text label displays the name"})
                return fallback
            raise ValueError("Missing APK asset: " + value)
        relative = path.relative_to(assets).as_posix()
        target = "ported/v330/" + relative
        copied[target] = path
        if corrected != value or relative != corrected:
            adjustments.append({"source": value, "replacement": target, "reason": "correct filename or case"})
        return target

    for item_id in ready:
        r = source["Disciples.txt"][item_id]
        if any(int(x) for x in r[17:21]):
            raise ValueError("Card has more than six effective fates: " + str(item_id))
        growth = source["CrewEvolution.txt"][item_id]
        fate_values = [str(fate_map[int(v)]) if int(v) else "0" for v in r[11:17]]
        body = art(r[24])
        tables["Disciples.txt"].append(compact(r[:11] + fate_values + [r[21], art(r[22]), body,
                                                    art(r[25], body), r[26], art(r[27]), art(r[28])]))
        attrs = dict(zip(("id", "quality", "group", "defaultSkill", "defaultHealth", "defaultAttack",
                          "defaultDefence", "defaultRp", "defaultExp"), [r[0], r[2], r[3]] + r[5:11]))
        attrs.update(zip(("defaultPotentiality", "growAttack", "growDefence", "growHealth", "growRp"), growth[9:14]))
        attrs.update({"fate" + str(n + 1): value for n, value in enumerate(fate_values)})
        server["disciple.xml"]["rows"].append(attrs)
    for item_id in equip_ids:
        r = list(source["Equips.txt"][item_id])
        if item_id == 125:
            # Both font versions lack 鹫; use its full Chinese definition instead.
            original = r[4]
            r[4] = r[4].replace("狮鹫", "狮身鹰首兽")
            adjustments.append({"kind": "text", "item": "equip:125", "source": original,
                                "replacement": r[4], "reason": "same meaning using available font glyphs"})
        body = art(r[8])
        tables["Equips.txt"].append(compact(r[:7] + [art(r[7]), body, art(r[9], body)]))
        server["equip.xml"]["rows"].append(dict(zip(("id", "type", "quality", "quantity", "growth"), r[:1] + r[2:4] + r[5:7])))
    for item_id in sorted(SKILLS):
        r = source["Skills.txt"][item_id]
        if int(r[3]) not in (4, 5, 6) or r[11] != "s_buff" or r[13] != "0":
            raise ValueError("Unsupported new skill mechanism: " + str(item_id))
        body = art(r[8])
        # Reuse the existing buff CCBI; attack/defence/will are calculated server-side.
        tables["Skills.txt"].append(compact(r[:7] + [art(r[7]), body, art(r[9], body)] + r[10:14] + [art(r[14])]))
        server["skill.xml"]["rows"].append(dict(zip(("id", "quality", "type", "quantity", "growth", "rate"), r[:1] + r[2:4] + r[5:7] + ["1"])))
    previous_tables = {}
    for item_id in fate_ids:
        r = list(source["Yuanfens.txt"][item_id])
        if item_id == 1026 and len(r) == 8 and r[4] == "":
            previous_tables["Yuanfens.txt"] = {str(fate_map[item_id]): compact([str(fate_map[item_id])] + r[1:7])}
            server["fate.xml"]["previousRows"] = {str(fate_map[item_id]): {
                "id": str(fate_map[item_id]), "type": "1", "require": r[3], "upgradeType": "", "quantity": "0.03"}}
            r.pop(4)
            adjustments.append({"kind": "table", "sourceFateId": 1026,
                                "reason": "remove the extra empty column before upgrade type"})
        if int(r[2]) not in (1, 2, 3) or int(r[4]) not in (1, 2, 3, 4):
            raise ValueError("Unsupported fate columns: " + str(item_id))
        row = compact([str(fate_map[item_id])] + r[1:7])
        tables["Yuanfens.txt"].append(row)
        quantity = format(int(r[5]) / 100, ".4f").rstrip("0").rstrip(".")
        server["fate.xml"]["rows"].append(dict(zip(("id", "type", "require", "upgradeType", "quantity"), [row[0], r[2], r[3], r[4], quantity])))
    catalog = {"id": "apk-v330-basic-2", "resourceVersion": "1.78.8",
               "sourceApkSha256": "f62a45209d80a5e8df0b38e81b14b11e5e4a2a9fc91beef35d16dec1760cb1f6",
               "sourceTableSha256": {name: hashlib.sha256((assets / "txt" / name).read_bytes()).hexdigest() for name in source},
               "clientTables": tables, "clientPreviousRows": previous_tables, "serverTemplates": server,
               "fateIdMap": {str(k): v for k, v in fate_map.items()}, "assetAdjustments": adjustments,
               "pendingCards": [{"id": i, "name": source["Disciples.txt"][i][1],
                                 "reason": "missing growth" + (" and source fates 1058-1063" if i == 343 else ""),
                                 "sourceRow": source["Disciples.txt"][i]} for i in sorted(PENDING)]}
    # Prepare the complete delta before changing any shared table.
    for target, path in copied.items():
        destination = ROOT / "resources/client" / target
        destination.parent.mkdir(parents=True, exist_ok=True)
        if destination.exists() and destination.read_bytes() != path.read_bytes():
            raise ValueError("Conflicting imported asset: " + target)
        shutil.copy2(path, destination)
    catalog["assets"] = {target: hashlib.sha256(path.read_bytes()).hexdigest() for target, path in sorted(copied.items())}
    catalog["sharedAssets"] = {}
    for name, baseline in FONT_BASE_HASHES.items():
        destination = ROOT / "resources/client" / name
        data = (assets / name).read_bytes()
        if name.endswith(".fnt"):
            text = decode(data)
            data = re.sub(r'face="[^"]+"', 'face="weiruanyahei"', text, count=1).encode("utf-8")
            old = decode(destination.read_bytes())
            def glyphs(value):
                return {int(item["id"]): item for line in value.splitlines() if line.startswith("char id=")
                        for item in [dict(re.findall(r"(\w+)=(-?\d+)", line))]}
            before, after = glyphs(old), glyphs(data.decode("utf-8"))
            metrics = ("width", "height", "xoffset", "yoffset", "xadvance")
            if any(c not in after or any(v.get(k) != after[c].get(k) for k in metrics) for c, v in before.items()):
                raise ValueError("APK font does not preserve existing glyphs and metrics")
        result_hash = hashlib.sha256(data).hexdigest()
        current_hash = hashlib.sha256(destination.read_bytes()).hexdigest()
        if current_hash not in (baseline, result_hash):
            raise ValueError("Custom source font detected: " + name)
        destination.write_bytes(data)
        catalog["sharedAssets"][name] = {"beforeSha256": baseline, "afterSha256": result_hash}
    for name, additions in tables.items():
        for directory in ("archive/client-resources", "resources/client"):
            merge_table(ROOT / directory / name, additions, previous_tables.get(name))
    for name, spec in server.items():
        merge_xml(ROOT / "server/config/xml" / name, spec)
    CATALOG.parent.mkdir(parents=True, exist_ok=True)
    CATALOG.write_text(json.dumps(catalog, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print("Imported basic content: %s cards, %s equipment, %s skills; %s cards pending original data." %
          (len(ready), len(equip_ids), len(SKILLS), len(PENDING)))
    print("Catalog:", CATALOG)


if __name__ == "__main__":
    main()
