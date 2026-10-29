"""Create a portable Mac handoff ZIP from prepared UTF-8 sources and client assets."""
import argparse
import hashlib
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "dist/onepiece-ios-source.zip")
    args = parser.parse_args()
    generated = ROOT / "generated"
    if not (generated / "client/Game/Classes/GamePrecedure.cpp").is_file():
        parser.error("run prepare.py before packaging")
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    count = 0
    with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
        # The handoff retains the original relative source layout so prepare.py
        # works after extraction, without the Android SDK or server executables.
        for source, destination in ((generated / "client", "src/client"),
                                    (generated / "assets", "src/server/ResourcesAndroid_Encrypt")):
            for path in sorted(source.rglob("*")):
                if path.is_file():
                    archive.write(path, destination + "/" + path.relative_to(source).as_posix())
                    count += 1
        for path in sorted(ROOT.rglob("*")):
            relative = path.relative_to(ROOT)
            if not path.is_file() or path.resolve() == output or relative.parts[0] in {"generated", "dist", "__pycache__"}:
                continue
            archive.write(path, "src/client/ios-modern/" + relative.as_posix())
            count += 1
        script = ROOT.parents[1] / "tools/configure_deployment.py"
        archive.write(script, "src/tools/configure_deployment.py")
        count += 1
    checksum = hashlib.sha256(output.read_bytes()).hexdigest()
    output.with_suffix(output.suffix + ".sha256").write_text(checksum + "  " + output.name + "\n", encoding="ascii")
    print("Portable source ZIP:", output)
    print("Files:", count, "Bytes:", output.stat().st_size)
    print("SHA256:", checksum)
    print("This is source and build preparation, not a compiled or installable IPA.")


if __name__ == "__main__":
    main()
