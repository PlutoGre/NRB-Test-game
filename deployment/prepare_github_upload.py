"""Refresh the portable iOS snapshot and prepare files for GitHub's browser uploader."""
import argparse
import hashlib
from pathlib import Path
import re
import shutil
import subprocess
import sys

HERE = Path(__file__).resolve().parent
REPO = HERE.parent
sys.path.insert(0, str(REPO / "src/tools"))
from project_paths import project_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", required=True)
    parser.add_argument("--web-port", type=int, default=18081)
    parser.add_argument("--game-port", type=int, default=25524)
    parser.add_argument("--resource-version", default="1.78.6")
    parser.add_argument("--output", type=Path, default=HERE / "github-upload")
    args = parser.parse_args()
    output = project_path(args.output).resolve()
    marker = output / ".onepiece-github-upload"
    if output.exists() and any(output.iterdir()) and not marker.exists():
        parser.error("output must be empty or previously created by this script")
    ios = REPO / "src/client/ios-modern"
    subprocess.run([sys.executable, str(ios / "prepare.py"), "--host", args.host,
                    "--web-port", str(args.web_port), "--game-port", str(args.game_port),
                    "--resource-version", args.resource_version], check=True)
    subprocess.run([sys.executable, str(ios / "package_source.py")], check=True)
    output.mkdir(parents=True, exist_ok=True)
    marker.touch()
    source = output / "source"
    source.mkdir(exist_ok=True)
    archive = ios / "dist/onepiece-ios-source.zip"
    names = set()
    with archive.open("rb") as stream:
        index = 1
        while True:
            data = stream.read(20 * 1024 * 1024)
            if not data:
                break
            name = archive.name + "." + str(index).zfill(3)
            (source / name).write_bytes(data)
            names.add(name)
            index += 1
    # Delete only obsolete part files in this marker-owned output, never source directories.
    for path in source.glob("onepiece-ios-source.zip.[0-9][0-9][0-9]"):
        if path.name not in names and path.resolve().is_relative_to(output):
            path.unlink()
    (source / (archive.name + ".sha256")).write_text(
        hashlib.sha256(archive.read_bytes()).hexdigest() + "  " + archive.name + "\n", encoding="ascii")
    workflow = output / ".github/workflows/ios-build.yml"
    workflow.parent.mkdir(parents=True, exist_ok=True)
    text = (HERE / "github/ios-build.yml").read_text(encoding="utf-8")
    defaults = {"server_host": args.host, "web_port": str(args.web_port),
                "game_port": str(args.game_port), "resource_version": args.resource_version}
    for name, value in defaults.items():
        pattern = r"(^      " + name + r":.*?^        default: )'[^']*'"
        text, count = re.subn(pattern, lambda match: match.group(1) + "'" + value + "'", text,
                             flags=re.MULTILINE | re.DOTALL)
        if count != 1:
            raise RuntimeError("Workflow template input missing: " + name)
    workflow.write_text(text, encoding="utf-8")
    shutil.copy2(REPO / "docs/CLIENT_BUILD.md", output / "CLIENT_BUILD.md")
    print("Prepared GitHub upload folder:", output)
    print("Upload source/ and create .github/workflows/ios-build.yml in the repository default branch.")
    print("No remote upload, build, signing or workflow run was performed.")


if __name__ == "__main__":
    main()
