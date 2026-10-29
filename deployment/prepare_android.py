"""Prepare the Anzhi Ant channel on a new machine; never compile, build or sign it."""
import argparse
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "src/tools"))
from project_paths import project_path, project_root

REPO = project_root()
PROJECT = REPO / "src/client/Game/proj.android"


def backup_write(path, text):
    if path.is_file() and not path.with_suffix(path.suffix + ".before-portable").exists():
        shutil.copy2(path, path.with_suffix(path.suffix + ".before-portable"))
    path.write_text(text, encoding="utf-8")


def property_path(path, base):
    try:
        value = Path(os.path.relpath(path, base)).as_posix()
    except ValueError:
        # An explicitly supplied SDK on another drive cannot be relative.
        value = path.as_posix()
    # Java .properties is ISO-8859-1 with UTF-16 escapes, not UTF-8.
    units = value.encode("utf-16-be")
    return "".join(chr(code) if 33 <= code <= 126 and code != 92 else "\\u%04x" % code
                   for code in (int.from_bytes(units[i:i + 2], "big") for i in range(0, len(units), 2)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", type=Path, default=Path("tools/android-sdk"), help="Project-relative SDK path or absolute path")
    parser.add_argument("--skip-native-sync", action="store_true", help="Stage before compiling NDK libraries")
    args = parser.parse_args()
    sdk = project_path(args.sdk)
    if not (sdk / "tools/ant/build.xml").is_file() or not (sdk / "platforms/android-17/android.jar").is_file():
        parser.error("SDK requires legacy Ant support and platform android-17; modern command-line tools alone are insufficient")
    channel = PROJECT / "antbuild/anzhi"
    for name in ("version_android.cfg", "game.properties"):
        if not (channel / "deployment" / name).is_file():
            parser.error("Generate matching server/client deployment settings first; missing deployment/" + name)
    package = ET.parse(channel / "AndroidManifest.xml").getroot().get("package")
    for path in sorted((channel / "src").rglob("*.java")):
        relative = path.relative_to(channel / "src")
        source = PROJECT / "src" / relative
        # Channel factory comes from its own template, not the root's other channel.
        if source.is_file() and path.name != "PlatformSDKState.java":
            text = source.read_text(encoding="utf-8-sig")
            text = re.sub(r"import com\.youai\.dreamonepiece\.platform\.[\w.]+\.R;", "import " + package + ".R;", text)
            if path.name == "NotificationService.java":
                text = re.sub(r"import com\.youai\.dreamonepiece\.platform\.[^;]+;",
                              "import com.youai.dreamonepiece.platform.anzhi.AnZhiActivity;", text)
                text = re.sub(r",\s*\w+Activity\.class\)", ",AnZhiActivity.class)", text)
            path.write_text(text, encoding="utf-8")
    shutil.copytree(PROJECT / "res_anzhi", channel / "res", dirs_exist_ok=True)
    assets = channel / "assets"
    assets.mkdir(exist_ok=True)
    resource_root = REPO / "src/server/ResourcesAndroid_Encrypt"
    excluded = {".php", ".html", ".js", ".sql", ".pem", ".xls", ".xlsx", ".doc", ".docx", ".zip"}
    for path in resource_root.rglob("*"):
        if not path.is_file() or path.suffix.lower() in excluded or "summary_dzm" in path.parts:
            continue
        target = assets / path.relative_to(resource_root)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, target)
    for path in (REPO / "src/client/Game/anzhiRes").rglob("*"):
        if path.is_file():
            relative = path.relative_to(REPO / "src/client/Game/anzhiRes")
            target = assets / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(path, target)
    (channel / "libs").mkdir(exist_ok=True)
    for relative in ("android-parcel.jar", "android-support-v4.jar", "annotations.jar", "anzhi/AnzhiSDKUCenter1.0.0.jar"):
        source = PROJECT / "libs" / relative
        if not source.is_file():
            parser.error("Missing bundled dependency: " + str(source))
        shutil.copy2(source, channel / "libs" / source.name)
    references = (REPO / "src/client/cocos2dx/platform/android/java", REPO / "src/client/91lib/Android/AnZhiSdkPro")
    properties = "target=android-17\n"
    for index, library in enumerate(references, 1):
        if not library.is_dir():
            parser.error("Missing Android library project: " + str(library))
        properties += "android.library.reference." + str(index) + "=" + property_path(library, channel) + "\n"
        backup_write(library / "local.properties", "sdk.dir=" + property_path(sdk, library) + "\n")
    backup_write(channel / "project.properties", properties)
    backup_write(channel / "SDK.properties", "sdk.dir=" + property_path(sdk, channel) + "\nprj.dir=./\n")
    if args.skip_native_sync:
        print("Sources/assets staged. After the NDK build, run again without --skip-native-sync.")
        return
    subprocess.run([sys.executable, str(PROJECT / "antbuild/prepare_channel.py"), "--project-dir", str(channel)], check=True)
    print("Prepared Anzhi Ant channel:", channel)
    print("No APK compilation or signing was performed.")


if __name__ == "__main__":
    main()
