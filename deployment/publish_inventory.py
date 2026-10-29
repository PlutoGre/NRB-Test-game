"""Inspect staged Git file names and sizes before uploading this project. Does not build or push."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parent.parent
LIMIT = 100 * 1024 * 1024
SIGNING_SUFFIXES = {".keystore", ".jks", ".p12", ".pfx", ".p8", ".mobileprovision", ".provisionprofile"}


def git(*args, data=None):
    return subprocess.run(["git", "-C", str(ROOT), *args], input=data, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, check=True).stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", type=Path, help="Optional file-name/size report; no file content is read")
    args = parser.parse_args()
    if not shutil.which("git"):
        parser.error("Install Git for Windows first")
    try:
        top = Path(git("rev-parse", "--show-toplevel").decode("utf-8").strip()).resolve()
    except subprocess.CalledProcessError:
        parser.error("Run git init, then git add . in the project root first")
    if top != ROOT:
        parser.error("The project must be its own Git repository, not part of a parent repository")
    staged = []
    for entry in git("ls-files", "--stage", "-z").split(b"\0"):
        if not entry:
            continue
        metadata, name = entry.split(b"\t", 1)
        mode, object_id, stage = metadata.decode("ascii").split()
        if stage != "0":
            parser.error("Resolve staged merge conflicts before publishing")
        staged.append({"path": name.decode("utf-8"), "mode": mode, "object": object_id})
    if not staged:
        parser.error("No staged/tracked files; run git add . first")
    nested = [item["path"] for item in staged if item["mode"] == "160000"]
    if nested:
        parser.error("Nested repositories/submodules need explicit setup: " + ", ".join(nested))
    output = git("cat-file", "--batch-check=%(objectsize)",
                 data="".join(item["object"] + "\n" for item in staged).encode("ascii"))
    sizes = output.decode("ascii").splitlines()
    if len(sizes) != len(staged):
        raise RuntimeError("Git returned an incomplete object-size list")
    blocked = []
    large = []
    total = 0
    names = {item["path"] for item in staged}
    required = {".github/workflows/ios-build.yml", "deployment/Deploy-Server.ps1", "deployment/server.py",
                "deployment/Start-External.ps1", "deployment/external.cmd", "deployment/external.py", "deployment/websocket_gateway.py",
                "deployment/tunnel-manifest.json", "docs/EXTERNAL_DEPLOYMENT.md",
                "src/client/ios-modern/native/ConnectionSettings.h", "src/client/ios-modern/native/ConnectionSettings.mm",
                "src/client/ios-modern/native/RemoteSocket.h", "src/client/ios-modern/native/RemoteSocket.mm",
                "deployment/runtime-manifest.json", "src/server/Code_Server/server/DZM.jar", "src/server/dzm.sql",
                "src/server/Code_Server/dzm_stat.sql", "src/tools/configure_deployment.py",
                "src/client/ios-modern/CMakeLists.txt", "src/client/ios-modern/prepare.py",
                "src/client/ios-modern/build.py", "src/client/ios-modern/package_app.py",
                "src/client/ios-modern/Sources.cmake", "src/client/ios-modern/vendor/curl-8.23.0.tar.xz",
                "src/client/ios-modern/vendor/libwebp-1.6.0.tar.gz"}
    required.update("src/client/cocos2dx/platform/third_party/android/prebuilt/" + library
                    + "/libs/" + abi + "/" + library + ".a"
                    for library in ("libcurl", "libjpeg", "libpng", "libtiff", "libwebp")
                    for abi in ("armeabi", "armeabi-v7a"))
    required.update({"src/client/Game/proj.android/libs/android-parcel.jar",
                     "src/client/Game/proj.android/libs/android-support-v4.jar",
                     "src/client/Game/proj.android/libs/annotations.jar",
                     "src/client/Game/proj.android/libs/anzhi/AnzhiSDKUCenter1.0.0.jar"})
    for item, size in zip(staged, sizes):
        item["size"] = int(size)
        total += item["size"]
        path = Path(item["path"])
        reason = None
        if item["mode"] == "160000":
            reason = "nested repository/submodule requires explicit dependency setup"
        elif item["size"] >= LIMIT:
            reason = "at/above 100 MiB ordinary Git upload limit"
        elif path.suffix.lower() in SIGNING_SUFFIXES or path.name in {"id_rsa", "id_ed25519"}:
            reason = "signing/private-key file"
        elif item["path"].startswith((".debug/", "deployment/.bootstrap/", "deployment/download-cache/",
                                      "deployment/github-upload/", "runtime/", "OnePieceServer/")):
            reason = "machine-local runtime/cache"
        if reason:
            blocked.append({"path": item["path"], "reason": reason})
        if item["size"] >= 50 * 1024 * 1024:
            large.append({"path": item["path"], "MiB": round(item["size"] / 1024 / 1024, 2)})
    missing = sorted(required - names)
    for prefix in ("src/server/ResourcesAndroid_Encrypt/", "src/client/ios-modern/native/"):
        if not any(name.startswith(prefix) for name in names):
            missing.append(prefix + "*")
    report = {"files": len(staged), "uncompressedMiB": round(total / 1024 / 1024, 2),
              "blocked": blocked, "filesOver50MiB": large, "missingRequiredFiles": missing}
    print(json.dumps(report, ensure_ascii=False, indent=2))
    if args.report:
        args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    if blocked or missing:
        print("Fix the listed staged entries before commit/push. Nothing was uploaded.")
        return 1
    print("Staged file-name/size inventory completed. This is not a build or a full secret scan; nothing was uploaded.")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        print("ERROR:", str(error), file=sys.stderr)
        sys.exit(1)
