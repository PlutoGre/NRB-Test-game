"""Extract the selected reference UI art into isolated, ordinary PNG resources.

Maintenance only: requires Pillow and the already extracted APK assets.
Deployment and client packaging use committed PNGs and never need the APK.
No client build, server startup or gameplay is performed by this script.
"""
import argparse
import hashlib
import json
import plistlib
import re
from pathlib import Path

from PIL import Image
from import_apk_catalog import decode

REPO = Path(__file__).resolve().parents[1]
PREFIX = "ported/growth/v1/"
NAMES = (
    "u_backgroundB", "u_buttenB01", "u_buttenB03", "u_buttenA01", "u_buttenA03",
    "u_buttenC01", "u_flbg01", "u_frameA", "u_icobg01", "u_texbg01",
    "u_banner01", "u_duanzaoHelp", "u_pgong", "u_pfang", "u_pti", "u_pyi",
    "u_yunbaobg1", "u_yunbao_xiang2", "u_yunbao_xiang3", "u_yunbao_xiang4",
    "u_EquipQuenchBG", "u_EquipFrameBG1", "u_SkillEvolutionBG01", "u_SkillEvolutionPic",
    "u_SuperEvolutionBG02", "u_MemTrainingbg01", "u_MemTrainingIco",
    "u_icoqyyunbao", "u_icoqyMemTraining",
)


def extract(assets):
    frames = {}
    for path in sorted((assets / "Imagesetfile").glob("*.plist")):
        data = plistlib.loads(decode(path.read_bytes()).encode("utf-8"))
        for name, frame in data.get("frames", {}).items():
            frames.setdefault(Path(name).name, (path, data["metadata"], frame))
    output = REPO / "resources/client" / PREFIX
    output.mkdir(parents=True, exist_ok=True)
    hashes = {}
    for name in NAMES:
        filename = name + ".png"
        direct = assets / "mainScene" / filename
        if direct.is_file():
            image = Image.open(direct).convert("RGBA")
        else:
            path, metadata, frame = frames[filename]
            texture = metadata.get("textureFileName") or metadata["realTextureFileName"]
            atlas = Image.open(path.parent / texture).convert("RGBA")
            x, y, width, height = map(int, re.findall(r"-?\d+", frame["frame"]))
            if frame.get("rotated"):
                image = atlas.crop((x, y, x + height, y + width)).transpose(Image.Transpose.ROTATE_90)
            else:
                image = atlas.crop((x, y, x + width, y + height))
            # Restore transparent trim margins, so all clients use the source dimensions.
            sx, sy, _, _ = map(int, re.findall(r"-?\d+", frame["sourceColorRect"]))
            original = tuple(map(int, re.findall(r"\d+", frame["sourceSize"])))
            canvas = Image.new("RGBA", original)
            canvas.paste(image, (sx, sy))
            image = canvas
        target = output / ("nrb_growth_" + filename)
        image.save(target, optimize=True)
        hashes[PREFIX + target.name] = hashlib.sha256(target.read_bytes()).hexdigest()
    manifest = REPO / "resources/catalog/expansion-systems-v1.json"
    release = json.loads(manifest.read_text(encoding="utf-8"))
    release["resourceVersion"] = "1.81.0"
    release["assets"].update(hashes)
    manifest.write_text(json.dumps(release, ensure_ascii=False, indent=2) + "\n", encoding="utf-8", newline="\n")
    print("Extracted %d reference art files; no build or runtime started." % len(hashes))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--assets", type=Path, required=True)
    extract(parser.parse_args().assets.resolve())
