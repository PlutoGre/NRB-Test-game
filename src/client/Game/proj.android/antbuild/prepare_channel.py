"""Prepare shared Ant sources and complete native ABI directories before packaging.

Python 3 only. This script does not compile or sign the game.
"""
import argparse
from pathlib import Path
import re
import shutil
import struct
import xml.etree.ElementTree as ET


ANDROID_PROJECT = Path(__file__).resolve().parent.parent
COMMON_SOURCES = (
    "com/youai/LogUtils.java",
    "com/youai/PlatformAndGameInfo.java",
    "com/youai/dreamonepiece/GameConfig.java",
    "com/youai/dreamonepiece/GameActivity.java",
    "com/youai/dreamonepiece/GameAnnounceDialog.java",
    "com/youai/dreamonepiece/platform/defaultplatform/PlatformDefaultLoginAndPay.java",
)
MACHINES = {"armeabi": 40, "armeabi-v7a": 40, "arm64-v8a": 183,
            "x86": 3, "x86_64": 62}


def copy_source(source, destination, package):
    if source.resolve() == destination.resolve():
        return
    text = source.read_text(encoding="utf-8-sig")
    text = re.sub(r"import com\.youai\.dreamonepiece\.platform\.[\w.]+\.R;",
                  "import " + package + ".R;", text)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(text, encoding="utf-8")


def prepare_sources(project):
    package = ET.parse(project / "AndroidManifest.xml").getroot().get("package")
    if not package:
        raise ValueError("AndroidManifest.xml has no package")
    for name in COMMON_SOURCES:
        copy_source(ANDROID_PROJECT / "src" / name, project / "src" / name, package)
    if project != ANDROID_PROJECT:
        templates = list(project.glob("*PlatformSDKState.java"))
        if len(templates) != 1:
            raise ValueError("Expected one channel PlatformSDKState template: " + str(project))
        copy_source(templates[0], project / "src/com/youai/dreamonepiece/PlatformSDKState.java", package)
    for name in ("game.properties", "version_android.cfg"):
        overlay = project / "deployment" / name
        if overlay.is_file():
            assets = project / "assets"
            assets.mkdir(exist_ok=True)
            shutil.copy2(overlay, assets / name)
            print("Deployment overlay:", overlay)


def prepare_native_libraries(project):
    libraries = project / "libs"
    compiled = ANDROID_PROJECT / "libs"
    # Include freshly compiled game libraries even if a channel only had SDK libs.
    for abi in MACHINES:
        source = compiled / abi / "libDreamOnePiece.so"
        destination = libraries / abi / source.name
        if source.is_file() and source.resolve() != destination.resolve():
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, destination)
    packaged = []
    for abi, machine in MACHINES.items():
        folder = libraries / abi
        if not folder.is_dir() or not list(folder.glob("*.so")):
            continue
        game = folder / "libDreamOnePiece.so"
        if not game.is_file() and abi == "armeabi-v7a":
            # Generic ARM instructions also run on ARMv7; never copy across architectures.
            generic = libraries / "armeabi/libDreamOnePiece.so"
            if generic.is_file():
                shutil.copy2(generic, game)
                print("WARNING: ARMv7 uses the generic ARM game library; rebuild ARMv7 when available")
        if not game.is_file():
            raise ValueError("Incomplete ABI " + abi + ": SDK libraries exist but libDreamOnePiece.so is missing")
        for library in folder.glob("*.so"):
            with library.open("rb") as stream:
                header = stream.read(20)
            if len(header) < 20 or header[:4] != b"\x7fELF" or header[5] != 1:
                raise ValueError("Invalid native library: " + str(library))
            if struct.unpack_from("<H", header, 18)[0] != machine:
                raise ValueError("Native library architecture does not match " + abi + ": " + str(library))
        packaged.append(abi)
    if not packaged:
        raise ValueError("No game native libraries found; compile the NDK game library first")
    print("Complete native ABIs:", ", ".join(packaged))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project-dir", required=True, type=Path)
    project = parser.parse_args().project_dir.resolve()
    if project != ANDROID_PROJECT and project.parent != ANDROID_PROJECT / "antbuild":
        parser.error("project-dir must be this Android project or one of its Ant channel directories")
    prepare_sources(project)
    prepare_native_libraries(project)


if __name__ == "__main__":
    main()
