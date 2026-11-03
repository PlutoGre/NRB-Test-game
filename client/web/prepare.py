"""Create a UTF-8 Web-only staging tree without changing mobile sources."""
import importlib.util
import json
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parent
REPO = ROOT.parents[1]
GENERATED = ROOT / "generated"


def replace(client, name, before, after):
    path = client / name
    source = path.read_text(encoding="utf-8")
    if before not in source:
        raise RuntimeError("Web staging patch no longer applies: " + name)
    path.write_text(source.replace(before, after), encoding="utf-8", newline="\n")


def prepare():
    if not GENERATED.resolve().is_relative_to(ROOT.resolve()) or GENERATED.is_symlink():
        raise RuntimeError("Web staging must remain inside client/web")
    if GENERATED.exists() and any(GENERATED.iterdir()) and not (GENERATED / ".nrb-web-generated").is_file():
        raise RuntimeError("Web staging directory must be empty or owned by this builder")
    spec = importlib.util.spec_from_file_location("ios_staging", ROOT.parent / "ios/prepare.py")
    staging = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(staging)
    GENERATED.mkdir(parents=True, exist_ok=True)
    (GENERATED / ".nrb-web-generated").touch()
    client = GENERATED / "client"
    staging.stage_sources(client)
    assets = GENERATED / "assets"
    # Remove only this tool's own generated asset tree, never runtime/player files.
    if assets.exists():
        if (assets.is_symlink() or not assets.resolve().is_relative_to(GENERATED.resolve())
                or not (GENERATED / ".nrb-web-generated").is_file()):
            raise RuntimeError("Unrecognized Web staging directory")
        shutil.rmtree(assets)
    (GENERATED / ".nrb-web-generated").touch()
    assets.mkdir()
    for path in staging.ASSETS.rglob("*"):
        if path.is_file() and path.suffix.lower() not in staging.EXCLUDED:
            target = assets / path.relative_to(staging.ASSETS)
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(path, target)
    # Relative URLs are resolved against the current browser origin at runtime.
    # The version must describe the bundled release, otherwise every fresh Web
    # build asks for an obsolete full-client update after a catalog upgrade.
    releases = [json.loads(path.read_text(encoding="utf-8")).get("resourceVersion")
                for path in (REPO / "resources/catalog").glob("*.json")]
    versions = [value for value in releases if value]
    if not versions:
        raise RuntimeError("No client resource release version found")
    bundled_version = max(versions, key=lambda value: tuple(int(part) for part in value.split(".")))
    (assets / "version_web.cfg").write_text(json.dumps({
        "version": 1, "localVerson": bundled_version,
        "sever": "/gameserver/dzm/1.8/servers_list.php",
        "severBackup": "/gameserver/dzm/1.8/servers_list.php",
        "inStoreUpdate": "/", "branch": "MAIN",
    }), encoding="utf-8")
    replace(client, "cocos2dx/platform/CCPlatformConfig.h",
            "// post configure", "// post configure\n#ifdef __EMSCRIPTEN__\n#define CC_PLATFORM_WEB 10\n#undef CC_TARGET_PLATFORM\n#define CC_TARGET_PLATFORM CC_PLATFORM_WEB\n#endif")
    replace(client, "Game/Classes/LoadingFrame.cpp", "#ifdef PROJECTIOSLOCAL",
            '#ifdef PROJECTWEB\nconst std::string _platfrom="version_web.cfg";\n#endif\n\n#ifdef PROJECTIOSLOCAL')
    replace(client, "91lib/include/libPlatform.cpp", "!defined(WIN32) && !defined(ANDROID)",
            "!defined(WIN32) && !defined(ANDROID) && !defined(__EMSCRIPTEN__)")
    replace(client, "Game/Classes/AppDelegate.cpp", "|| defined(PROJECTIOSLOCAL)",
            "|| defined(PROJECTIOSLOCAL) || defined(PROJECTWEB)")
    # All preload files live in MEMFS; avoid an unsupported pthread image loader.
    path = client / "cocos2dx/textures/CCTextureCache.cpp"
    source = path.read_text(encoding="utf-8")
    start = source.index("void CCTextureCache::addImageAsync(")
    brace = source.index("{", start)
    depth, end = 1, brace + 1
    while depth:
        if source[end] == "{": depth += 1
        if source[end] == "}": depth -= 1
        end += 1
    source = source[:brace] + "{\n CCTexture2D* texture = addImage(path);\n if (target && selector) (target->*selector)(texture);\n}" + source[end:]
    path.write_text(source, encoding="utf-8", newline="\n")
    # PacketManager retains an unused historical socket member. Remove it rather
    # than linking BSD socket emulation into the real WebSocket transport.
    replace(client, "Game/Classes/PacketManager.h", "AsyncSocket mySocket;", "// Web transport is owned by ThreadSocket.")
    replace(client, "Game/Classes/SeverConsts.h", "ThreadService mCheckFileThread;", "// File checks are scheduled on the browser event loop.")
    replace(client, "Game/Classes/SeverConsts.cpp", "mCheckFileThread.shutdown();", "// Browser callback already completed.")
    replace(client, "Game/Classes/SeverConsts.cpp", "mCheckFileThread.execute(new SeverCheckingFileTask);",
            "emscripten_async_call([](void*) { SeverCheckingFileTask task; task.run(); }, 0, 0);")
    replace(client, "Game/Classes/SeverConsts.cpp", "delete pBuffer;", "delete[] pBuffer;")
    replace(client, "cocos2dx/platform/CCApplicationProtocol.h", "kTargetNaCl,", "kTargetNaCl,\n    kTargetWeb,")
    replace(client, "cocos2dx/include/cocos2d.h", '#include "script_support/CCScriptSupport.h"',
            '#if defined(__EMSCRIPTEN__)\n#include "CCApplication.h"\n#include "CCEGLView.h"\n#include "CCAccelerometer.h"\n#endif\n#include "script_support/CCScriptSupport.h"')
    replace(client, "Game/Platform/GamePlatform.cpp", "outBuffer = new unsigned char[outSize];",
            "outBuffer = new unsigned char[outSize + 1];\n        outBuffer[outSize] = 0;")
    replace(client, "scripting/lua/lua/loslib.c", "#if defined(LUA_USE_IOS)",
            "#if defined(LUA_USE_IOS) || defined(PROJECTWEB)")
    replace(client, "protobuf-2.5.0rc1/src/google/protobuf/stubs/platform_macros.h", "#elif defined(__pnacl__)",
            "#elif defined(__wasm32__)\n#define GOOGLE_PROTOBUF_ARCH_WASM32 1\n#define GOOGLE_PROTOBUF_ARCH_32_BIT 1\n#elif defined(__pnacl__)")
    shutil.copy2(ROOT / "native/ThreadSocket.h", client / "Game/Classes/ThreadSocket.h")
    # WebGL requires bound element buffers; legacy desktop code used client arrays.
    replace(client, "cocos2dx/platform/CCPlatformMacros.h", "#define CC_REBIND_INDICES_BUFFER  0",
            "#define CC_REBIND_INDICES_BUFFER  1")
    return client, assets


if __name__ == "__main__":
    prepare()
    print("Web sources and assets staged; no compilation performed.")
