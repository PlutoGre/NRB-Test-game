"""Archive an already-built iPhone app for download and later signing. Never builds or signs."""
import argparse
from pathlib import Path
import plistlib
import tarfile
import zipfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--app", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    app = args.app.resolve()
    if not app.is_dir() or app.suffix != ".app" or not (app / "Info.plist").is_file():
        parser.error("--app must be a compiled .app bundle with Info.plist")
    with (app / "Info.plist").open("rb") as stream:
        info = plistlib.load(stream)
    executable = info.get("CFBundleExecutable", "")
    if not executable or Path(executable).name != executable or not (app / executable).is_file():
        parser.error("app bundle executable is missing")
    output = args.output.resolve()
    if output == app or output.is_relative_to(app):
        parser.error("output must be outside the app bundle")
    output.mkdir(parents=True, exist_ok=True)
    ipa = output / "onepiece-unsigned-for-resigning.ipa"
    with zipfile.ZipFile(ipa, "w", zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(app.rglob("*")):
            name = "Payload/" + path.relative_to(app.parent).as_posix()
            if path.is_symlink():
                # Preserve any bundle-relative link, including its Unix file mode.
                entry = zipfile.ZipInfo(name)
                entry.create_system = 3
                entry.external_attr = path.lstat().st_mode << 16
                archive.writestr(entry, str(path.readlink()).encode("utf-8"))
            elif path.is_file():
                archive.write(path, name)
    with tarfile.open(output / "onepiece-unsigned-iphoneos-app.tar.gz", "w:gz") as archive:
        archive.add(app, arcname=app.name)
    (output / "UNSIGNED-NOT-INSTALLABLE.txt").write_text(
        "This app/IPA is unsigned. Apply valid signing or local re-signing before iPhone installation.\n"
        "A successful build does not verify gameplay on a device.\n", encoding="utf-8")
    print("Unsigned IPA for re-signing:", ipa)


if __name__ == "__main__":
    main()
