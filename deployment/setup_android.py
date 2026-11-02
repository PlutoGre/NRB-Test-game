"""Provision a project-local legacy Android toolchain. Never execute a compiler."""
import argparse
from contextlib import contextmanager
import hashlib
import http.client
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import stat
import sys
import tempfile
import time
import urllib.error
import urllib.request
import uuid
import zipfile

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "scripts"))
from project_paths import project_path

MANIFEST = json.loads((HERE / "android-toolchain.json").read_text(encoding="utf-8"))
CACHE = HERE / "download-cache"
STAMP = ".nrb-android-tool.json"
INSTALLING = ".nrb-android-installing.json"


@contextmanager
def setup_lock():
    import msvcrt
    identity = hashlib.sha256(str(HERE).casefold().encode("utf-8")).hexdigest()[:24]
    path = Path(tempfile.gettempdir()) / ("nrb-android-setup-" + identity + ".lock")
    with path.open("a+b") as stream:
        if stream.seek(0, os.SEEK_END) == 0:
            stream.write(b"\0")
            stream.flush()
        stream.seek(0)
        try:
            msvcrt.locking(stream.fileno(), msvcrt.LK_NBLCK, 1)
        except OSError:
            raise RuntimeError("Another Android tool setup is running for this checkout")
        try:
            yield
        finally:
            stream.seek(0)
            msvcrt.locking(stream.fileno(), msvcrt.LK_UNLCK, 1)


def verified(path, item):
    if not path.is_file() or path.stat().st_size != item["size"]:
        return False
    digest = hashlib.new(item["algorithm"])
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest() == item["checksum"]


def download(item, offline):
    CACHE.mkdir(parents=True, exist_ok=True)
    path = CACHE / item["file"]
    if path.exists():
        if not verified(path, item):
            raise RuntimeError("Cached package checksum mismatch; replace " + str(path))
        return path
    if offline:
        raise RuntimeError("Offline package missing: " + str(path))
    partial = path.with_name(path.name + ".partial")
    for attempt in range(3):
        offset = partial.stat().st_size if partial.exists() else 0
        if offset == item["size"]:
            if not verified(partial, item):
                raise RuntimeError("Partial package checksum mismatch; replace " + str(partial))
            os.replace(partial, path)
            return path
        if offset > item["size"]:
            raise RuntimeError("Partial package is too large; replace " + str(partial))
        headers = {"User-Agent": "NRB-Android-Tool-Setup/1"}
        if offset:
            headers["Range"] = "bytes=" + str(offset) + "-"
        print("Downloading", item["file"], "(attempt", str(attempt + 1) + "/3)", flush=True)
        try:
            with urllib.request.urlopen(urllib.request.Request(item["url"], headers=headers), timeout=60) as response:
                if not response.geturl().startswith("https://"):
                    raise RuntimeError("Tool downloads require HTTPS")
                append = response.status == 206 and offset > 0
                if append and not response.headers.get("Content-Range", "").startswith("bytes " + str(offset) + "-"):
                    raise RuntimeError("Download server returned an unexpected byte range")
                if not append:
                    offset = 0
                last_report = time.monotonic()
                with partial.open("ab" if append else "wb") as output:
                    while block := response.read(1024 * 1024):
                        output.write(block)
                        offset += len(block)
                        if offset > item["size"]:
                            raise RuntimeError("Downloaded package exceeds its pinned size")
                        if time.monotonic() - last_report >= 5:
                            print(" ", offset // (1024 * 1024), "/", item["size"] // (1024 * 1024), "MiB", flush=True)
                            last_report = time.monotonic()
            if not verified(partial, item):
                if partial.stat().st_size == item["size"]:
                    raise RuntimeError("Downloaded package checksum mismatch; replace " + str(partial))
                raise OSError("Download ended before the package was complete")
            os.replace(partial, path)
            return path
        except urllib.error.HTTPError as error:
            if error.code == 416 and partial.exists() and verified(partial, item):
                os.replace(partial, path)
                return path
            if error.code not in (408, 429, 500, 502, 503, 504) or attempt == 2:
                raise RuntimeError("Official download failed (HTTP " + str(error.code) + "): " + item["url"])
        except (OSError, urllib.error.URLError, http.client.HTTPException) as error:
            if attempt == 2:
                raise RuntimeError("Download failed; check access to Google/Apache or prefill download-cache: " + str(error))
        time.sleep(2)


def complete(directory, item):
    return all((directory / name).is_file() for name in item["required"])


def extended_path(path):
    # Long Eclipse/NDK archive entries can exceed MAX_PATH in a staging folder.
    value = str(path.resolve())
    if os.name == "nt" and not value.startswith("\\\\?\\"):
        value = "\\\\?\\UNC\\" + value[2:] if value.startswith("\\\\") else "\\\\?\\" + value
    return value


def retry_windows_io(operation, action):
    # 解压后的文件可能被短暂占用；只重试 Windows 的拒绝访问和共享冲突。
    for attempt in range(6):
        try:
            return action()
        except OSError as error:
            if getattr(error, "winerror", None) not in (5, 32, 33) or attempt == 5:
                raise
            print(operation + ": Windows denied access; retry " + str(attempt + 1) + "/5", flush=True)
            time.sleep(0.5 * (attempt + 1))


def copy_tool_file(source, destination):
    # copytree 会汇总文件异常，因此在这里保留单个文件的 Windows 错误码并重试。
    return retry_windows_io("Copy tool file " + str(destination), lambda: shutil.copyfile(source, destination))


def publish_tool(source, directory, expected, recovering):
    if not recovering:
        if directory.exists():
            retry_windows_io("Remove empty tool directory", directory.rmdir)
        try:
            # Windows 的 rename 拒绝覆盖已有目标，避免替换其他程序创建的目录。
            retry_windows_io("Move extracted tool", lambda: os.rename(extended_path(source), extended_path(directory)))
            return
        except OSError as error:
            if getattr(error, "winerror", None) not in (5, 32, 33):
                raise
            print("Directory move is still blocked; copying the verified tool files instead:", directory, flush=True)
        # 只有本次成功创建的目录才允许复制，已有的外部工具目录不会被覆盖。
        retry_windows_io("Create tool directory", directory.mkdir)
        with (directory / INSTALLING).open("x", encoding="utf-8") as stream:
            stream.write(json.dumps(expected, indent=2) + "\n")
    else:
        print("Resuming an interrupted tool installation:", directory, flush=True)
    try:
        # 保留进行中标记直到所有文件复制完毕，避免下次误用半套工具。
        retry_windows_io("Copy extracted tool", lambda: shutil.copytree(
            extended_path(source), extended_path(directory), dirs_exist_ok=True,
            copy_function=copy_tool_file, ignore=shutil.ignore_patterns(STAMP, INSTALLING)))
        retry_windows_io("Record installed tool", lambda: (directory / STAMP).write_text(
            json.dumps(expected, indent=2) + "\n", encoding="utf-8"))
        retry_windows_io("Finish tool installation", (directory / INSTALLING).unlink)
    except OSError as error:
        raise RuntimeError("Cannot finish tool installation at " + str(directory)
                           + ". Close programs using this folder and check its write permissions, then rerun. "
                           + "The download cache and installation marker were kept for retry. " + str(error)) from error


def install(name, directory, item, offline):
    stamp = directory / STAMP
    expected = {key: item[key] for key in ("version", "algorithm", "checksum")}
    pending = directory / INSTALLING
    recovering = pending.is_file()
    if recovering and json.loads(pending.read_text(encoding="utf-8")) != expected:
        raise RuntimeError("Interrupted tool installation has a different version; choose a fresh directory: " + str(directory))
    if not recovering and complete(directory, item):
        if stamp.exists() and json.loads(stamp.read_text(encoding="utf-8")) != expected:
            raise RuntimeError("Managed tool version changed; use a fresh tool directory: " + str(directory))
        if name == "ndk" and not (directory / "RELEASE.TXT").read_text(encoding="utf-8").strip().startswith("r8e"):
            raise RuntimeError("This project requires NDK r8e; choose a separate NDK directory")
        print("Using existing", name + ":", directory, flush=True)
        return
    if not recovering and directory.exists() and any(directory.iterdir()):
        hint = " Modern SDK Tools do not contain Ant support; use a separate legacy SDK directory." if name == "sdk-tools" else ""
        raise RuntimeError("Incomplete/unrecognized tool directory; no existing files were replaced: " + str(directory) + hint)
    archive = download(item, offline)
    parent = directory.parent.resolve()
    parent.mkdir(parents=True, exist_ok=True)
    stage = parent / (".nrb-tool-" + uuid.uuid4().hex)
    stage.mkdir()
    print("Extracting", name, item["version"], flush=True)
    try:
        with zipfile.ZipFile(archive) as package:
            members = package.infolist()
            for member in members:
                relative = PurePosixPath(member.filename.replace("\\", "/"))
                destination = (stage / str(relative)).resolve()
                if (relative.is_absolute() or ".." in relative.parts or any(":" in part for part in relative.parts)
                        or not destination.is_relative_to(stage.resolve()) or stat.S_ISLNK(member.external_attr >> 16)):
                    raise RuntimeError("Unsafe archive entry in " + item["file"])
            last_report = time.monotonic()
            extraction_path = extended_path(stage)
            for index, member in enumerate(members, 1):
                package.extract(member, extraction_path)
                if time.monotonic() - last_report >= 5:
                    print(" ", index, "/", len(members), "archive entries", flush=True)
                    last_report = time.monotonic()
        source = stage / item["root"]
        if not source.resolve().is_relative_to(stage.resolve()) or not directory.resolve().is_relative_to(parent):
            raise RuntimeError("Tool installation path escaped its selected directory")
        if not complete(source, item):
            raise RuntimeError("Official package does not contain the expected " + name + " files")
        (source / STAMP).write_text(json.dumps(expected, indent=2) + "\n", encoding="utf-8")
        publish_tool(source, directory, expected, recovering)
    finally:
        # This is the only recursive deletion: a unique staging directory we own.
        if stage.exists():
            if not stage.name.startswith(".nrb-tool-") or not stage.resolve().is_relative_to(parent):
                raise RuntimeError("Refusing to clean a staging directory outside the tool parent")
            try:
                retry_windows_io("Clean tool staging directory", lambda: shutil.rmtree(extended_path(stage)))
            except OSError as error:
                # 清理失败不应覆盖真正的安装错误，也不能把已成功安装报告成失败。
                print("WARNING: Temporary tool files could not be removed:", stage, str(error), file=sys.stderr)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", type=Path, default=Path("tools/android-sdk"))
    parser.add_argument("--ndk", type=Path, default=Path("tools/android-ndk-r8e"))
    parser.add_argument("--ant", type=Path, default=Path("tools/apache-ant"))
    parser.add_argument("--offline", action="store_true")
    args = parser.parse_args()
    if os.name != "nt":
        parser.error("This toolchain setup is for Windows x64")
    sdk, ndk, ant = (project_path(path).resolve() for path in (args.sdk, args.ndk, args.ant))
    for name, item in MANIFEST.items():
        length = hashlib.new(item["algorithm"]).digest_size * 2
        if not re.fullmatch(r"[0-9a-f]{" + str(length) + "}", item["checksum"]):
            raise RuntimeError("Tool manifest checksum is not valid: " + name)
    with setup_lock():
        for name, item in MANIFEST.items():
            if item["destination"].startswith("sdk/"):
                destination = sdk / item["destination"][4:]
            else:
                destination = ndk if item["destination"] == "ndk" else ant
            install(name, destination, item, args.offline)
    print("Android SDK, API 21, Build Tools, Platform Tools, NDK r8e and Ant are ready.", flush=True)
    print("No compilation, packaging, signing or device test was executed by this setup tool.", flush=True)


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, zipfile.BadZipFile) as error:
        print("ERROR:", str(error), file=sys.stderr)
        sys.exit(1)
