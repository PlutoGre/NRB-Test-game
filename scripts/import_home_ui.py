"""Extract isolated reference home art and bake two ship setup poses.

Maintenance utility requiring Pillow and extracted APK assets. Ordinary builds
use the committed PNGs; no APK, Spine runtime or Python image library is needed.
"""
import argparse
import hashlib
import json
import math
import plistlib
import re
from pathlib import Path

from PIL import Image, ImageChops, ImageDraw
from import_apk_catalog import decode

REPO = Path(__file__).resolve().parents[1]
PREFIX = "ported/home/v1/"
NAMES = [
    "u_mainFrameBG01", "u_mainFrameBGsea", "u_mainFrameBGSea02",
    "u_mainFrameCloud01", "u_mainFrameTree01", "u_mainSceneBannerBG1",
    "u_mainSceneExp", "u_mainSceneIcohelper", "u_mainSceneBtnBG01", "u_mainSceneBtnBG02",
    "u_framebutten01", "u_framebutten02", "u_framebutten03", "u_framebutten04",
    "u_framebutten05", "u_framebutten06", "u_framebg01b", "u_mainSceneBtn0_1",
    "u_mbutten01", "u_mbutten04", "u_mbutten05", "u_mbutten06", "u_mbutten07",
    "u_mainSceneBtn1_3", "u_mainSceneBtn1_4", "u_mainSceneBtn1_5",
] + ["u_mainSceneBtn%d_1" % i for i in range(1, 8)] + ["u_mainSceneBtn%d_2" % i for i in range(1, 8)]


def atlas_index(root):
    frames = {}
    for path in sorted((root / "Imagesetfile").glob("*.plist")):
        value = plistlib.loads(decode(path.read_bytes()).encode("utf-8"))
        for name, frame in value.get("frames", {}).items():
            frames.setdefault(Path(name).name, (path.parent / value["metadata"]["textureFileName"], frame))
    return frames


def read_image(root, frames, name):
    direct = root / "mainScene" / (name + ".png")
    if direct.is_file():
        return Image.open(direct).convert("RGBA")
    path, frame = frames[name + ".png"]
    atlas = Image.open(path).convert("RGBA")
    x, y, w, h = map(int, re.findall(r"-?\d+", frame["frame"]))
    if frame.get("rotated"):
        image = atlas.crop((x, y, x + h, y + w)).transpose(Image.Transpose.ROTATE_90)
    else:
        image = atlas.crop((x, y, x + w, y + h))
    sx, sy, _, _ = map(int, re.findall(r"-?\d+", frame["sourceColorRect"]))
    original = tuple(map(int, re.findall(r"\d+", frame["sourceSize"])))
    canvas = Image.new("RGBA", original)
    canvas.paste(image, (sx, sy))
    return canvas


def multiply(a, b):
    return (a[0]*b[0]+a[2]*b[1], a[1]*b[0]+a[3]*b[1],
            a[0]*b[2]+a[2]*b[3], a[1]*b[2]+a[3]*b[3],
            a[0]*b[4]+a[2]*b[5]+a[4], a[1]*b[4]+a[3]*b[5]+a[5])


def point(matrix, x, y):
    a, b, c, d, tx, ty = matrix
    return a*x+c*y+tx, b*x+d*y+ty


def ship_pose(root, name):
    """Rasterize reference Spine 2.1 mesh triangles without a game runtime."""
    directory = root / "spine/Ship" / name
    skeleton = json.loads(decode((directory / (name + ".json")).read_bytes()))
    texture = Image.open(directory / (name + ".png")).convert("RGBA")
    regions, current = {}, None
    for line in (directory / (name + ".atlas")).read_text().splitlines():
        if line and not line.startswith(" ") and ":" not in line and line != name + ".png":
            current = {}; regions[line] = current
        elif current is not None and line.startswith("  ") and ":" in line:
            key, value = line.strip().split(":", 1);current[key] = value.strip()
    bones, by_name = [], {}
    for bone in skeleton["bones"]:
        radians = math.radians(bone.get("rotation", 0));c, s = math.cos(radians), math.sin(radians)
        sx, sy = bone.get("scaleX", 1), bone.get("scaleY", 1)
        matrix = (c*sx, s*sx, -s*sy, c*sy, bone.get("x", 0), bone.get("y", 0))
        if "parent" in bone:matrix = multiply(by_name[bone["parent"]], matrix)
        by_name[bone["name"]] = matrix;bones.append(matrix)
    attachments = []
    for slot in skeleton["slots"]:
        attachment = skeleton["skins"]["default"][slot["name"]][slot["attachment"]]
        region = regions[attachment.get("path", slot["attachment"])];x, y = map(int, region["xy"].split(","));w, h = map(int, region["size"].split(","))
        if region["rotate"] == "true":
            image = texture.crop((x, y, x+h, y+w)).transpose(Image.Transpose.ROTATE_90)
        else:image = texture.crop((x, y, x+w, y+h))
        if attachment.get("type") not in ("mesh", "skinnedmesh"):
            raise ValueError("Unexpected reference ship attachment type")
        vertices, points = attachment["vertices"], []
        if attachment["type"] == "mesh":
            points = [point(by_name[slot["bone"]], vertices[i], vertices[i+1]) for i in range(0, len(vertices), 2)]
        else:
            cursor = 0
            while cursor < len(vertices):
                count = vertices[cursor];cursor += 1;px = py = 0
                for _ in range(count):
                    index, vx, vy, weight = vertices[cursor:cursor+4];cursor += 4
                    bx, by = point(bones[index], vx, vy);px += bx*weight;py += by*weight
                points.append((px, py))
        attachments.append((image, attachment, points))
    all_points = [p for _, _, points in attachments for p in points]
    left, right = min(p[0] for p in all_points), max(p[0] for p in all_points)
    bottom, top = min(p[1] for p in all_points), max(p[1] for p in all_points)
    scale = 2;canvas = Image.new("RGBA", (math.ceil((right-left)*scale)+4, math.ceil((top-bottom)*scale)+4))
    for image, attachment, points in attachments:
        points = [((x-left)*scale+2, (top-y)*scale+2) for x, y in points]
        uv = list(zip(attachment["uvs"][::2], attachment["uvs"][1::2]))
        triangles = attachment["triangles"]
        for offset in range(0, len(triangles), 3):
            ids = triangles[offset:offset+3];dst = [points[i] for i in ids];src = [(uv[i][0]*image.width, uv[i][1]*image.height) for i in ids]
            x0, y0 = math.floor(min(p[0] for p in dst)), math.floor(min(p[1] for p in dst))
            x1, y1 = math.ceil(max(p[0] for p in dst)), math.ceil(max(p[1] for p in dst))
            if x1 <= x0 or y1 <= y0:continue
            (ax, ay), (bx, by), (cx, cy) = dst;det = (bx-ax)*(cy-ay)-(cx-ax)*(by-ay)
            if abs(det) < 0.00001:continue
            coeffs = []
            for dim in range(2):
                av, bv, cv = [p[dim] for p in src]
                a = ((bv-av)*(cy-ay)-(cv-av)*(by-ay))/det
                b = ((bx-ax)*(cv-av)-(cx-ax)*(bv-av))/det
                coeffs.extend((a, b, av-a*ax-b*ay+a*x0+b*y0))
            tile = image.transform((x1-x0, y1-y0), Image.Transform.AFFINE, coeffs, Image.Resampling.BICUBIC)
            mask = Image.new("L", tile.size);ImageDraw.Draw(mask).polygon([(x-x0, y-y0) for x, y in dst], fill=255)
            tile.putalpha(ImageChops.multiply(tile.getchannel("A"), mask));canvas.alpha_composite(tile, (x0, y0))
    return canvas.resize((canvas.width//2, canvas.height//2), Image.Resampling.LANCZOS)


def extract(root):
    frames = atlas_index(root);output = REPO / "resources/client" / PREFIX
    output.mkdir(parents=True, exist_ok=True);assets = {}
    def emit(name, image):
        path = output / ("nrb_home_" + name + ".png");image.save(path, optimize=True)
        assets[PREFIX + path.name] = hashlib.sha256(path.read_bytes()).hexdigest()
    for name in NAMES:emit(name, read_image(root, frames, name))
    for name in ("Ship_A", "Ship_B"):emit(name, ship_pose(root, name))
    manifest = {"id":"home-ui-v1", "resourceVersion":"1.82.0", "serverTemplates":{}, "clientTables":{}, "assets":assets}
    (REPO / "resources/catalog/home-ui-v1.json").write_text(json.dumps(manifest, ensure_ascii=False, indent=2)+"\n", encoding="utf-8", newline="\n")
    print("Extracted %d home art files; no client build or server startup performed." % len(assets))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__);parser.add_argument("--assets", type=Path, required=True)
    extract(parser.parse_args().assets.resolve())
