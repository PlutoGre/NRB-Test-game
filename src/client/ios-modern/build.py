"""Generate Xcode, build an app, or archive/export a signed development IPA on macOS."""
import argparse
import json
from pathlib import Path
import platform
import plistlib
import re
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parent


def run(command, log):
    print("+", shlex.join(str(value) for value in command), flush=True)
    with log.open("a", encoding="utf-8") as stream:
        stream.write("\n+ " + shlex.join(str(value) for value in command) + "\n")
        with subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              text=True, encoding="utf-8", errors="replace") as process:
            for line in process.stdout:
                print(line, end="", flush=True)
                stream.write(line)
            code = process.wait()
    if code:
        raise SystemExit("Command failed with exit code " + str(code) + "; see " + str(log))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--action", choices=("configure", "build", "archive", "export"), default="configure")
    parser.add_argument("--sdk", choices=("iphoneos", "iphonesimulator"), default="iphoneos")
    parser.add_argument("--bundle-id", default="org.onepiece.local")
    parser.add_argument("--team-id", default="")
    parser.add_argument("--sign", action="store_true", help="Use the team's automatic development signing")
    parser.add_argument("--configuration", choices=("Debug", "Release"), default="Debug")
    parser.add_argument("--generated", type=Path, default=ROOT / "generated")
    parser.add_argument("--build-dir", type=Path)
    parser.add_argument("--export-method", choices=("debugging", "release-testing"), default="debugging")
    args = parser.parse_args()
    if platform.system() != "Darwin":
        parser.error("macOS with full Xcode is required. Windows can run prepare.py only.")
    for executable in ("cmake", "xcodebuild", "xcrun"):
        if not shutil.which(executable):
            parser.error("required executable not found: " + executable)
    if not re.fullmatch(r"[A-Za-z0-9-]+(?:\.[A-Za-z0-9-]+)+", args.bundle_id):
        parser.error("invalid bundle identifier")
    if args.team_id and not re.fullmatch(r"[A-Z0-9]{10}", args.team_id):
        parser.error("team-id must be the 10-character Apple Team ID")
    if args.sign and not args.team_id:
        parser.error("--sign requires --team-id; configure it in Xcode for a Personal Team")
    if args.action in ("archive", "export") and (args.sdk != "iphoneos" or not args.sign):
        parser.error("archive/export requires --sdk iphoneos --sign --team-id TEAMID")
    generated = args.generated.resolve()
    preparation = generated / "preparation.json"
    if not preparation.is_file() or json.loads(preparation.read_text(encoding="utf-8"))["stageOnly"]:
        parser.error("run prepare.py --host PHONE_REACHABLE_HOST first")
    build = (args.build_dir or generated / ("build-" + args.sdk)).resolve()
    build.mkdir(parents=True, exist_ok=True)
    log = build / "build.log"
    arch = "arm64" if args.sdk == "iphoneos" or platform.machine() == "arm64" else "x86_64"
    run(["cmake", "-S", str(ROOT), "-B", str(build), "-G", "Xcode",
         "-DCMAKE_SYSTEM_NAME=iOS", "-DCMAKE_OSX_SYSROOT=" + args.sdk,
         "-DCMAKE_OSX_ARCHITECTURES=" + arch, "-DCMAKE_OSX_DEPLOYMENT_TARGET=15.0",
         "-DOP_CLIENT_ROOT=" + str(generated / "client"), "-DOP_ASSET_DIR=" + str(generated / "assets"),
         "-DOP_BUNDLE_ID=" + args.bundle_id, "-DOP_TEAM_ID=" + args.team_id,
         "-DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=" + ("YES" if args.sign else "NO")], log)
    if args.action == "configure":
        print("Xcode project:", build / "OnePieceIOS.xcodeproj")
        return
    command = ["xcodebuild", "-project", str(build / "OnePieceIOS.xcodeproj"), "-scheme", "OnePiece",
               "-configuration", args.configuration, "-sdk", args.sdk,
               "-destination", "generic/platform=" + ("iOS" if args.sdk == "iphoneos" else "iOS Simulator"),
               "-derivedDataPath", str(build / "DerivedData")]
    if args.sign:
        command += ["-allowProvisioningUpdates", "DEVELOPMENT_TEAM=" + args.team_id, "CODE_SIGN_STYLE=Automatic"]
    else:
        command += ["CODE_SIGNING_ALLOWED=NO"]
    archive = build / "OnePiece.xcarchive"
    if args.action == "build":
        run(command + ["build"], log)
        print("App output:", build / "DerivedData/Build/Products" / (args.configuration + "-" + args.sdk))
        print("A physical iPhone requires signing; an unsigned .app is not installable on it.")
    else:
        run(command + ["-archivePath", str(archive), "archive"], log)
        if args.action == "export":
            options = build / "ExportOptions.plist"
            with options.open("wb") as stream:
                plistlib.dump({"method": args.export_method, "teamID": args.team_id,
                               "signingStyle": "automatic", "destination": "export"}, stream)
            run(["xcodebuild", "-exportArchive", "-archivePath", str(archive),
                 "-exportPath", str(build / "ipa"), "-exportOptionsPlist", str(options),
                 "-allowProvisioningUpdates"], log)
            print("Signed IPA directory:", build / "ipa")
        else:
            print("Archive:", archive)
    (build / "result.json").write_text(json.dumps({"action": args.action, "sdk": args.sdk,
        "architecture": arch, "signed": args.sign, "bundleId": args.bundle_id,
        "buildSucceeded": True, "deviceVerified": False}, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
