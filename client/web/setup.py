"""Install the pinned Emscripten SDK inside this checkout (no global activation)."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tarfile
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parents[2]
VERSION = "3.1.74"
COMMIT = "3d6d8ee910466516a53e665b86458faa81dae9ba"


def command(args, **kwargs):
    subprocess.run([str(x) for x in args], check=True, **kwargs)


def setup():
    sdk = ROOT / "tools/emsdk"
    marker = sdk / ".nrb-emsdk.json"
    if sdk.exists() and not marker.exists():
        raise RuntimeError("tools/emsdk exists without an NRB ownership marker; use --emsdk for an existing SDK")
    if not sdk.exists():
        sdk.parent.mkdir(exist_ok=True)
        url = "https://codeload.github.com/emscripten-core/emsdk/tar.gz/" + COMMIT
        print("Downloading pinned Emscripten SDK " + VERSION, flush=True)
        with tempfile.TemporaryDirectory(dir=sdk.parent) as temporary:
            archive = Path(temporary) / "sdk.tar.gz"
            urllib.request.urlretrieve(url, archive)
            with tarfile.open(archive) as package:
                base = Path(temporary).resolve()
                for member in package.getmembers():
                    if (not (base / member.name).resolve().is_relative_to(base)
                            or member.issym() or member.islnk()):
                        raise RuntimeError("Unsafe SDK archive")
                package.extractall(base)
            (base / ("emsdk-" + COMMIT)).rename(sdk)
        marker.write_text(json.dumps({"version": VERSION, "commit": COMMIT}), encoding="utf-8")
    elif json.loads(marker.read_text(encoding="utf-8"))["commit"] != COMMIT:
        raise RuntimeError("SDK marker version differs; keep a separate checkout for a different SDK")
    if os.name == "nt":
        command([sys.executable, sdk / "emsdk.py", "install", "python-3.9.2-nuget-64bit"], cwd=sdk)
    command([sys.executable, sdk / "emsdk.py", "install", VERSION], cwd=sdk)
    command([sys.executable, sdk / "emsdk.py", "activate", VERSION], cwd=sdk)
    return sdk


if __name__ == "__main__":
    try:
        print(setup())
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        sys.exit("Web SDK setup failed: " + str(error))
