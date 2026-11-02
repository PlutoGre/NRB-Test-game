"""Compile the original Cocos/Lua game to WebAssembly on Windows/Linux/macOS.

Invoked by deployment/build-web.cmd. No Android/iOS compiler or binary is used.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tarfile
import time

# The server's embedded Python intentionally omits the script directory.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from prepare import ROOT, REPO, GENERATED, prepare
from setup import VERSION, setup


def run(command, log, env):
    log.parent.mkdir(parents=True, exist_ok=True)
    with log.open("wb") as stream:
        result = subprocess.run([str(x) for x in command], stdout=stream, stderr=subprocess.STDOUT, env=env, cwd=ROOT)
    if result.returncode:
        print(log.read_text(encoding="utf-8", errors="replace")[-12000:], file=sys.stderr)
        raise RuntimeError("Compiler/linker failed; full log: " + str(log))


def unpack_webp():
    archive = ROOT.parent / "ios/vendor/libwebp-1.6.0.tar.gz"
    digest = "e4ab7009bf0629fd11982d4c2aa83964cf244cffba7347ecd39019a9e38c4564"
    if hashlib.sha256(archive.read_bytes()).hexdigest() != digest:
        raise RuntimeError("Bundled WebP archive checksum mismatch")
    destination = GENERATED / "vendor"
    destination.mkdir(exist_ok=True)
    with tarfile.open(archive) as package:
        for item in package.getmembers():
            target = (destination / item.name).resolve()
            if not target.is_relative_to(destination.resolve()) or item.issym() or item.islnk():
                raise RuntimeError("Unsafe WebP archive member")
        package.extractall(destination)
    return destination / "libwebp-1.6.0"


def source_list(client, webp):
    manifest = (ROOT.parent / "ios/Sources.cmake").read_text(encoding="utf-8")
    excluded = {"CCSAXParser.cpp", "CCImageCommonWebp.cpp", "HttpClient.cpp", "AssetsManager.cpp", "LocalStorage.cpp", "platform.cpp"}
    sources = [client / name for name in re.findall(r'"\$\{CLIENT\}/([^"\n]+)"', manifest)
               if Path(name).suffix in {".cpp", ".cc", ".c"} and Path(name).name not in excluded
               and "/ios/" not in name and not name.startswith("CocosDenshion/")]
    skip = {"CurlDownload.cpp", "ThreadSocket.cpp", "AsyncSocket.cpp", "IoSocket.cpp", "SocketBase.cpp", "Concurrency.cpp"}
    sources += [p for p in (client / "Game/Classes").glob("*.cpp") if p.name not in skip]
    sources += list((client / "Game/Protobuf").glob("*.cc"))
    for directory, patterns in {
        "scripting/lua/lua": ("*.c",), "scripting/lua/tolua": ("*.c",),
        "scripting/lua/cocos2dx_support": ("*.cpp", "tolua_fix.c"),
        "jsoncpp-src-0.5.0/src/lib_json": ("*.cpp",), "tinyxml": ("*.cpp",),
    }.items():
        for pattern in patterns:
            sources += [p for p in (client / directory).glob(pattern) if p.name not in {"lua.c", "luac.c", "print.c", "xmltest.cpp"}]
    sources += [client / "Game/Platform/GamePlatform.cpp", client / "91lib/include/libPlatform.cpp"]
    sources += list((ROOT / "native").glob("*.cpp"))
    # The portable decoder includes architecture dispatch but no SIMD/threads or
    # tools. Emscripten supplies PNG/JPEG; WebP is built from the pinned source.
    for directory in ("dec", "dsp", "utils"):
        sources += list((webp / "src" / directory).glob("*.c"))
    return sorted(set(sources))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--emsdk", type=Path, help="Existing activated Emscripten 3.1.74 SDK; skips download")
    parser.add_argument("--output", type=Path, default=REPO / "outputs/web")
    parser.add_argument("--jobs", type=int, default=2)
    args = parser.parse_args()
    if not 1 <= args.jobs <= 16:
        parser.error("--jobs must be between 1 and 16")
    existing_sdk = args.emsdk.expanduser() if args.emsdk else None
    sdk = ((existing_sdk if existing_sdk.is_absolute() else REPO / existing_sdk).resolve()
           if existing_sdk else setup())
    compiler_python = Path(sys.executable)
    if os.name == "nt":
        # Run emcc and its child tools with the SDK's complete Python, rather
        # than the server bootstrap interpreter's isolated ._pth configuration.
        candidates = sorted((sdk / "python").glob("*/python.exe"))
        if not candidates:
            candidates = sorted((sdk / "python").glob("*/tools/python.exe"))
        if candidates:
            compiler_python = candidates[-1]
        elif sys.flags.isolated:
            raise RuntimeError("SDK Python is missing. Install python-3.9.2-nuget-64bit with emsdk before building")
    compiler = sdk / "upstream/emscripten/emcc.py"
    if not compiler.is_file():
        parser.error("Missing Emscripten SDK: " + str(compiler))
    actual = (compiler.parent / "emscripten-version.txt").read_text(encoding="utf-8").strip().strip('"')
    if actual != VERSION:
        parser.error("Use the pinned Emscripten " + VERSION + "; found " + actual)
    client, assets = prepare()
    webp = unpack_webp()
    sources = source_list(client, webp)
    missing = [str(p) for p in sources if not p.is_file()]
    if missing:
        raise RuntimeError("Missing source files: " + ", ".join(missing))
    includes = [ROOT / "native", client, client / "cocos2dx", client / "cocos2dx/include", client / "cocos2dx/platform",
        client / "cocos2dx/kazmath/include", client / "CocosDenshion/include", client / "extensions", client / "external",
        client / "Game/Classes", client / "Game/Platform", client / "Game/Protobuf", client / "91lib/include",
        client / "libOS/include", client / "protobuf-2.5.0rc1/src", client / "jsoncpp-src-0.5.0/include",
        client / "tinyxml", client / "scripting/lua/lua", client / "scripting/lua/tolua",
        client / "scripting/lua/cocos2dx_support", webp, webp / "src"]
    # Some recovered engine files use local basename includes. Include only
    # directories in this target, never a platform/ios or Android vendor folder.
    includes += sorted({p.parent for p in sources if str(p).startswith(str(client / "cocos2dx"))
                        or str(p).startswith(str(client / "extensions"))})
    flags = ["-O2", "-gsource-map", "-fexceptions", "-Wno-deprecated-declarations", "-Wno-register",
        "-DPROJECTWEB=1", "-DCC_ENABLE_CHIPMUNK_INTEGRATION=0", "-DCC_ENABLE_BOX2D_INTEGRATION=0",
        # Protobuf 2.5 uses two spellings for its single-thread switches. Browser
        # callbacks and packet decoding all run on the UI thread in this target.
        "-DGOOGLE_PROTOBUF_NO_THREAD_SAFETY=1", "-DGOOGLE_PROTOBUF_NO_THREADSAFETY=1",
        "-DCC_TEXTURE_ATLAS_USE_VAO=0", "-DWEBP_DISABLE_SIMD=1", "-DUSE_FILE32API=1",
        "-sUSE_SDL=2", "-sUSE_SDL_IMAGE=2", '-sSDL2_IMAGE_FORMATS=["png","jpg"]', "-sUSE_ZLIB=1",
        "-include", str(ROOT / "native/Prefix.h")]
    for path in dict.fromkeys(includes):
        flags += ["-I", str(path)]
    env = os.environ.copy()
    env["EM_CONFIG"] = str(sdk / ".emscripten")
    env["EM_CACHE"] = str(sdk / "upstream/emscripten/cache")
    env["PYTHONUTF8"] = "1"
    env["EMSDK_PYTHON"] = str(compiler_python)
    # Header changes invalidate cached objects even when a .cpp did not change.
    signature = hashlib.sha256(json.dumps(flags).encode())
    for directory in (client, ROOT / "native"):
        for path in sorted(directory.rglob("*.h")):
            signature.update(path.read_bytes())
    common = signature.digest()
    obj = GENERATED / "objects"
    obj.mkdir(exist_ok=True)
    logs = GENERATED / "logs"

    def compile_one(path):
        key = hashlib.sha256(common + str(path).encode() + path.read_bytes()).hexdigest()
        target = obj / (key + ".o")
        if target.is_file(): return target
        temporary = obj / (key + ".part.o")
        language = ["-x", "c", "-std=gnu99"] if path.suffix == ".c" else ["-x", "c++", "-std=c++11"]
        arguments = flags + language + ["-c", str(path), "-o", str(temporary)]
        response = obj / (key + ".rsp")
        response.write_text("\n".join(json.dumps(str(value), ensure_ascii=False) for value in arguments), encoding="utf-8")
        run([compiler_python, compiler, "@" + str(response)], logs / (path.name + ".log"), env)
        temporary.replace(target)
        return target

    print("Compiling", len(sources), "original game/engine/protocol sources; first build may take a long time.", flush=True)
    # Port downloads/sysroot initialization must complete before parallel workers.
    objects = [compile_one(sources[0])]
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(compile_one, path) for path in sources[1:]]
        for index, future in enumerate(as_completed(futures), 2):
            objects.append(future.result())
            if index % 25 == 0: print("Compiled", index, "/", len(sources), flush=True)
    package = GENERATED / "package"
    package.mkdir(exist_ok=True)
    exports = ["_main", "_malloc", "_free", "_nrb_message", "_nrb_input", "_nrb_edit", "_nrb_board", "_nrb_ime", "_nrb_backspace", "_nrb_visibility"]
    link = [str(p) for p in sorted(objects)] + ["-O2", "-fexceptions", "-sDISABLE_EXCEPTION_CATCHING=0", "-sALLOW_MEMORY_GROWTH=1",
        "-sINITIAL_MEMORY=134217728", "-sMAXIMUM_MEMORY=1073741824", "-sSTACK_SIZE=5242880",
        "-sMODULARIZE=1", "-sEXPORT_NAME=createNRBGame", "-sENVIRONMENT=web", "-sEXIT_RUNTIME=0",
        "-sASSERTIONS=1", "-sFETCH=1", "-sFORCE_FILESYSTEM=1", "-sFULL_ES2=1", "-sMAX_WEBGL_VERSION=1",
        # Legacy Cocos/Lua callbacks contain native function-pointer casts.
        "-sEMULATE_FUNCTION_POINTER_CASTS=1",
        "-sUSE_SDL=2", "-sUSE_SDL_IMAGE=2", '-sSDL2_IMAGE_FORMATS=["png","jpg"]', "-sUSE_ZLIB=1",
        "-sEXPORTED_FUNCTIONS=" + json.dumps(exports, separators=(",", ":")),
        '-sEXPORTED_RUNTIME_METHODS=["ccall","FS","IDBFS","addRunDependency","removeRunDependency"]',
        "-lidbfs.js", "-lwebsocket.js", "--preload-file", str(assets) + "@/assets", "-o", str(package / "game.js")]
    response = GENERATED / "link.rsp"
    response.write_text("\n".join(json.dumps(value, ensure_ascii=False) for value in link), encoding="utf-8")
    print("Linking WebAssembly and shared game resources...", flush=True)
    run([compiler_python, compiler.with_name("em++.py"), "@" + str(response)], logs / "link.log", env)
    for name in ("game.js", "game.wasm", "game.data"):
        if not (package / name).is_file() or not (package / name).stat().st_size:
            raise RuntimeError("Linker did not produce a complete Web distribution: " + name)
    for name in ("index.html", "web.js", "style.css"):
        shutil.copy2(ROOT / "shell" / name, package / name)
    try:
        revision = subprocess.run(["git", "rev-parse", "HEAD"], cwd=REPO, capture_output=True, text=True).stdout.strip() or "unknown"
    except OSError:
        revision = "unknown"  # A downloaded source ZIP may not have Git installed.
    manifest = {"platform": "web", "buildComplete": True, "sourceRevision": revision, "emscripten": VERSION,
                "resourceVersion": "1.78.6", "builtAt": int(time.time()), "browserVerified": False,
                "files": {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in package.iterdir() if p.is_file() and p.name != "web-client.json"}}
    output = args.output if args.output.is_absolute() else REPO / args.output
    if output.resolve() in {REPO.resolve(), ROOT.resolve(), GENERATED.resolve()}:
        raise RuntimeError("Output must be a dedicated generated directory")
    if output.exists() and any(output.iterdir()) and not (output / "web-client.json").is_file():
        raise RuntimeError("Output directory contains files not owned by this Web builder")
    output.mkdir(parents=True, exist_ok=True)
    (output / "web-client.json").unlink(missing_ok=True)
    for path in package.iterdir():
        if path.is_file() and path.name != "web-client.json": shutil.copy2(path, output / path.name)
    (output / "web-client.json").write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print("Web build complete:", output)
    print("Browser play verification has not been performed by this build script.")


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        sys.exit("Web build failed: " + str(error))
