"""Import the two frozen APK activity layouts and isolate their image atlases.

This writes source resources and a deployment manifest; it does not build or run
the game. Ordinary deployment only needs the committed resources, never the APK.
"""
import argparse
import csv
import hashlib
import io
import json
from pathlib import Path
import plistlib
import re
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from import_apk_catalog import decode, merge_table

REPO = Path(__file__).resolve().parents[1]
PREFIX = "ported/activities/v1/"
CLIENT = REPO / "resources/client"
LABELS = {
    "@Treasure": "宝库", "@Recharge": "积分商店", "@HaveIntegral": "积分", "@HaveDiamond": "钻石",
    "@ExploreOne": "探索一次", "@RemainingTime": "永久开放", "@ExploreBest": "探索十次",
    "@ConsumePoints": "积分", "@TotalStarsNum": "本轮收集", "@UlTimesReset": "重置转盘",
    "@PeronaLeftFreeTool": "今日免费次数", "@TimeCountdown": "永久开放", "@PeronaShop": "积分商店",
    "@PeronaDraw": "抽取一次", "@ExploreCoreFreeTimes": "重置消耗", "@PeronaDrawFail": "重复位置保底",
    "@PeronaDrawSuccess": "奖励已发传书",
}


class StringCache:
    """Read only the CCB v5 header/string table; preserve the node graph bytes."""
    def __init__(self, data):
        if data[:4] != b"ibcc":
            raise ValueError("Not a CCB file")
        self.data, self.offset, self.bit = data, 4, 0
        if self.uint() != 5:
            raise ValueError("Only CCB v5 can be imported")
        self.offset += 1  # JS-controlled flag, byte aligned.
        count = self.uint()
        self.start, self.strings = self.offset, []
        for _ in range(count):
            length = struct.unpack_from(">H", data, self.offset)[0]
            self.offset += 2
            self.strings.append(data[self.offset:self.offset + length].decode("utf-8"))
            self.offset += length

    def getbit(self):
        value = (self.data[self.offset] >> self.bit) & 1
        self.bit += 1
        if self.bit == 8:
            self.bit = 0
            self.offset += 1
        return value

    def uint(self):
        size = 0
        while not self.getbit():
            size += 1
        value = 1 << size
        for bit in range(size - 1, -1, -1):
            value |= self.getbit() << bit
        if self.bit:
            self.offset += 1
            self.bit = 0
        return value - 1

    def replace(self, convert):
        strings = []
        for value in self.strings:
            encoded = convert(value).encode("utf-8")
            strings.append(struct.pack(">H", len(encoded)) + encoded)
        return self.data[:self.start] + b"".join(strings) + self.data[self.offset:]


def import_assets(source):
    files = {p.relative_to(source).as_posix().lower(): p for p in source.rglob("*") if p.is_file()}
    frames, plists = {}, {}
    for key, path in files.items():
        if not key.endswith(".plist"):
            continue
        try:
            value = plistlib.loads(decode(path.read_bytes()).encode("utf-8"))
        except Exception as error:
            # Non-atlas plists are not dependencies of the two imported layouts.
            if key.startswith("imagesetfile/"):
                raise ValueError("Cannot read APK image atlas: " + str(path)) from error
            continue
        if isinstance(value, dict) and isinstance(value.get("frames"), dict):
            plists[key] = value
            for frame in value["frames"]:
                frames.setdefault(frame.lower(), (key, frame))
                frames.setdefault(Path(frame).name.lower(), (key, frame))
    written, atlases = {}, set()
    manifest_path = REPO / "resources/catalog/permanent-activities-v1.json"
    previous = json.loads(manifest_path.read_text(encoding="utf-8"))["assets"] if manifest_path.is_file() else {}

    def frame_name(atlas, name):
        return "nrb_activity_" + hashlib.sha256(atlas.encode("utf-8")).hexdigest()[:8] + "_" + Path(name).name

    def emit(relative, data):
        relative = PREFIX + relative
        path = CLIENT / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.is_file() and path.read_bytes() != data:
            if hashlib.sha256(path.read_bytes()).hexdigest() != previous.get(relative):
                raise ValueError("Imported activity resource conflicts: " + relative)
        path.write_bytes(data)
        written[relative] = hashlib.sha256(data).hexdigest()
        return relative

    def copy_file(key):
        key = key.replace("\\", "/")
        if ".." in Path(key).parts or key.startswith("/"):
            raise ValueError("Invalid APK resource path")
        path = files.get(key.lower())
        if not path:
            raise ValueError("Missing APK resource: " + key)
        relative = path.relative_to(source).as_posix()
        target = PREFIX + relative
        if target in written:
            return target
        # Reserve before recursing: nested CCB files and atlas aliases may repeat.
        written[target] = "pending"
        if relative.lower().endswith(".ccbi"):
            data = StringCache(path.read_bytes()).replace(convert)
        elif relative.lower().endswith(".plist") and relative.lower() in plists:
            value = dict(plists[relative.lower()])
            # This engine strips directories from frame lookups. Namespace the
            # basename as well, so imported atlases never replace old UI frames.
            value["frames"] = {frame_name(relative.lower(), name): frame for name, frame in value["frames"].items()}
            for frame in value["frames"].values():
                if "aliases" in frame:
                    frame["aliases"] = [frame_name(relative.lower(), alias) for alias in frame["aliases"]]
            metadata = value.get("metadata", {})
            texture = metadata.get("textureFileName") or metadata.get("realTextureFileName")
            if not texture:
                raise ValueError("Atlas texture is unspecified: " + relative)
            texture_path = (Path(relative).parent / texture).as_posix()
            if texture_path.lower() not in files:
                texture_path = texture
            copy_file(texture_path)
            data = plistlib.dumps(value, fmt=plistlib.FMT_XML, sort_keys=False)
            atlases.add(target)
        elif relative.lower().endswith(".fnt"):
            text = decode(path.read_bytes())
            for page in re.findall(r'page\s+id=\d+\s+file="([^"]+)"', text):
                copy_file((Path(relative).parent / page).as_posix())
            data = text.encode("utf-8")
        else:
            data = path.read_bytes()
        return emit(relative, data)

    def convert(value):
        # The old client's language bundle lacks some newer activity keys.
        if value in LABELS:
            return LABELS[value]
        suffix = Path(value).suffix.lower()
        if suffix in (".ccbi", ".ccb"):
            name = str(Path(value).with_suffix(".ccbi")).replace("\\", "/")
            key = name if name.lower() in files else "ccbi/" + name
            target = copy_file(key)
            return str(Path(target).with_suffix(suffix)).replace("\\", "/")
        if suffix == ".fnt":
            # Keep the already upgraded shared fonts when available.
            return value if (CLIENT / value).is_file() else copy_file(value)
        frame = frames.get(value.lower()) or frames.get(Path(value).name.lower())
        if frame:
            copy_file(frame[0])
            return frame_name(frame[0], frame[1])
        if suffix in (".png", ".jpg", ".jpeg", ".plist", ".pvr", ".ccz"):
            return copy_file(value)
        return value

    for name in ("AllBluePage.ccbi", "PeronaTurntable.ccbi"):
        copy_file("ccbi/" + name)
    emit("atlases.txt", ("\n".join(sorted(atlases)) + "\n").encode("utf-8"))
    return written


def normalize_reward_icons():
    path = REPO / "server/config/xml/permanentActivities.json"
    config = json.loads(path.read_text(encoding="utf-8"))
    tables = {}
    for kind, filename, column in (("property", "UserProperty.txt", 3), ("tool", "Tools.txt", 8),
                                   ("skill", "Skills.txt", 7), ("equip", "Equips.txt", 7),
                                   ("card", "Disciples.txt", 18)):
        rows = csv.reader(io.StringIO(decode((CLIENT / filename).read_bytes())), delimiter="\t")
        tables[kind] = {int(row[0]): row[column].strip() for row in rows if row and row[0].isdigit()}
    for reward in config["rewards"] + [config["fortuneWheel"]["duplicateReward"]]:
        kind, template = reward["key"].split(":")
        reward["icon"] = tables[kind][int(template)]
    path.write_text(json.dumps(config, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets", type=Path, required=True, help="Frozen APK extracted assets")
    args = parser.parse_args()
    assets = import_assets(args.assets.resolve())
    normalize_reward_icons()
    additions = [
        ["9001", "All_Blue", "90", "mainScene/u_icoqyqixi.png", "none", "1"],
        ["9002", "幸运转盘", "91", "mainScene/u_icoqy03.png", "none", "1"],
    ]
    for directory in (CLIENT, REPO / "archive/client-resources"):
        merge_table(directory / "AdventureConfig.txt", additions)
    config = REPO / "server/config/xml/permanentActivities.json"
    manifest = {"id": "permanent-activities-v1", "resourceVersion": "1.79.0", "serverTemplates": {},
                "serverFiles": {"permanentActivities.json": {"sha256": hashlib.sha256(config.read_bytes()).hexdigest(), "preserveExisting": True, "allowLocalOverride": True}},
                "clientTables": {"AdventureConfig.txt": additions}, "assets": assets}
    output = REPO / "resources/catalog/permanent-activities-v1.json"
    output.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print("Imported activity source resources:", len(assets), "files; no compilation or gameplay executed")


if __name__ == "__main__":
    main()
