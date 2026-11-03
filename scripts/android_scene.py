"""Capture a debug Android game's UI tree and derive adb input from its nodes."""
import argparse
import json
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("operation", choices=("capture", "tap", "swipe"))
    parser.add_argument("--serial", default="emulator-5554")
    parser.add_argument("--directory", default="/sdcard/Android/data/com.youai.dreamonepiece.platform.anzhi/files")
    parser.add_argument("--text")
    parser.add_argument("--action")
    parser.add_argument("--within-text", help="Select the nearest row containing this label and the requested control")
    parser.add_argument("--type")
    parser.add_argument("--tag", type=int)
    parser.add_argument("--index", type=int, default=0)
    parser.add_argument("--direction", choices=("up", "down"), default="up")
    parser.add_argument("--output", type=Path, default=Path("outputs/android-qa/current"))
    args = parser.parse_args()
    adb = ["adb", "-s", args.serial]
    raw = subprocess.check_output(adb + ["exec-out", "cat", args.directory.rstrip("/") + "/nrb-scene.json"])
    tree = json.loads(raw)
    entries = []

    def visit(node, path="0", parents=None):
        parents = parents or []
        if not node:
            return
        entry = dict(node)
        entry.pop("children", None)
        entry["path"] = path
        entry["parents"] = parents
        x = tree["viewportX"] + (node["x"] + node["width"] / 2) * tree["scaleX"]
        y = tree["frameHeight"] - tree["viewportY"] - (node["y"] + node["height"] / 2) * tree["scaleY"]
        entry["screen"] = [round(x), round(y)]
        if 0 <= x < tree["frameWidth"] and 0 <= y < tree["frameHeight"]:
            entries.append(entry)
        for index, child in enumerate(node.get("children", [])):
            visit(child, path + "." + str(index), parents + [entry])

    visit(tree["scene"])
    if args.operation == "capture":
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.with_suffix(".json").write_bytes(raw)
        args.output.with_suffix(".png").write_bytes(subprocess.check_output(adb + ["exec-out", "screencap", "-p"]))
        for entry in entries:
            if entry.get("text") or entry.get("action"):
                print(json.dumps({key: entry[key] for key in ("path", "text", "action", "tag", "screen", "enabled") if key in entry}, ensure_ascii=False))
        return
    if not any((args.text, args.action, args.type, args.tag is not None)):
        parser.error("Select a node by --text, --action, --type or --tag")
    selected = [node for node in entries if (args.text is None or node.get("text") == args.text)
                and (args.action is None or node.get("action") == args.action)
                and (args.type is None or args.type in node.get("type", ""))
                and (args.tag is None or node.get("tag") == args.tag)]
    if args.within_text:
        nearby = []
        for label in [node for node in entries if node.get("text") == args.within_text]:
            for parent in reversed(label["parents"]):
                matches = [node for node in selected if node["path"].startswith(parent["path"] + ".")]
                if matches:
                    nearby.extend(matches)
                    break
        selected = nearby
    if not selected:
        raise RuntimeError("No matching visible UI node; capture the current hierarchy first")
    node = selected[args.index]
    if args.operation == "tap":
        if "enabled" not in node:
            node = next((parent for parent in reversed(node["parents"]) if "enabled" in parent), node)
        if node.get("enabled") is False:
            raise RuntimeError("Selected control is disabled")
        coordinates = node["screen"]
        print("Tap UI node", node["path"], node.get("action", node.get("text", "")), coordinates)
        subprocess.run(adb + ["shell", "input", "tap"] + [str(value) for value in coordinates], check=True)
    else:
        x = node["screen"][0]
        distance = min(node["height"] * tree["scaleY"] * 0.6, tree["frameHeight"] * 0.5)
        center = node["screen"][1]
        first, last = round(center + distance / 2), round(center - distance / 2)
        if args.direction == "down": first, last = last, first
        subprocess.run(adb + ["shell", "input", "swipe", str(x), str(first), str(x), str(last), "700"], check=True)


if __name__ == "__main__":
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    main()
