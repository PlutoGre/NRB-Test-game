cmake_minimum_required(VERSION 3.24)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    message(FATAL_ERROR "Use the Xcode generator with -DCMAKE_SYSTEM_NAME=iOS on a Mac.")
endif()
project(OnePieceIOS LANGUAGES C CXX OBJC OBJCXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 11)
set(CMAKE_XCODE_GENERATE_SCHEME ON)

set(OP_BUNDLE_ID "org.onepiece.local" CACHE STRING "Unique bundle identifier for signing")
set(OP_TEAM_ID "" CACHE STRING "Apple development team, optional for unsigned builds")
set(OP_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated/assets" CACHE PATH "Prepared client assets")
if(NOT EXISTS "${OP_ASSET_DIR}/version_ios_local.cfg")
    message(FATAL_ERROR "Run prepare.py with a phone-reachable --host before configuring Xcode.")
endif()

# Source archives are pinned and bundled; no ARMv7-only vendor .a files are linked.
include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(CURL_USE_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(CURL_USE_CMAKECONFIG OFF CACHE BOOL "" FORCE)
# Only optional legacy engine HTTP helpers use curl. Game resource downloads use
# NSURLSession (native TLS); local engine helpers require plain HTTP in this port.
set(CURL_ENABLE_SSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)
set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
set(CURL_ZSTD OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
set(PICKY_COMPILER OFF CACHE BOOL "" FORCE)
FetchContent_Declare(curl URL "${CMAKE_CURRENT_SOURCE_DIR}/vendor/curl-8.23.0.tar.xz"
    URL_HASH SHA256=8dadd7d2536a314f982ad05a651b713f426bf7d02cc2c76cffb9796f2218e6c5)
foreach(feature ANIM_UTILS CWEBP DWEBP GIF2WEBP IMG2WEBP VWEBP WEBPINFO LIBWEBPMUX WEBPMUX EXTRAS WEBP_JS FUZZTEST)
    set(WEBP_BUILD_${feature} OFF CACHE BOOL "" FORCE)
endforeach()
FetchContent_Declare(webp URL "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libwebp-1.6.0.tar.gz"
    URL_HASH SHA256=e4ab7009bf0629fd11982d4c2aa83964cf244cffba7347ecd39019a9e38c4564)
FetchContent_MakeAvailable(curl webp)

set(OP_CLIENT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/generated/client" CACHE PATH "UTF-8 source staging directory")
get_filename_component(CLIENT "${OP_CLIENT_ROOT}" ABSOLUTE)
if(NOT EXISTS "${CLIENT}/Game/Classes/GamePrecedure.cpp")
    message(FATAL_ERROR "Source staging is missing; run prepare.py first.")
endif()
include(Sources.cmake)
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "/CocosDenshion/")
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "/platform/ios/CCCommon\\.mm$")
file(GLOB GAME_SOURCES CONFIGURE_DEPENDS "${CLIENT}/Game/Classes/*.cpp" "${CLIENT}/Game/Protobuf/*.cc")
list(FILTER GAME_SOURCES EXCLUDE REGEX "/CurlDownload\\.cpp$")
file(GLOB LUA_SOURCES CONFIGURE_DEPENDS "${CLIENT}/scripting/lua/lua/*.c")
# Lua 5.1 interpreter, not the obsolete ARM32 LuaJIT or its CLI executable.
list(FILTER LUA_SOURCES EXCLUDE REGEX "/(lua|print)\\.c$")
file(GLOB LUA_BINDINGS CONFIGURE_DEPENDS "${CLIENT}/scripting/lua/tolua/*.c"
    "${CLIENT}/scripting/lua/cocos2dx_support/*.cpp" "${CLIENT}/scripting/lua/cocos2dx_support/tolua_fix.c")
file(GLOB JSON_SOURCES CONFIGURE_DEPENDS "${CLIENT}/jsoncpp-src-0.5.0/src/lib_json/*.cpp")
file(GLOB TINYXML_SOURCES CONFIGURE_DEPENDS "${CLIENT}/tinyxml/*.cpp")
list(FILTER TINYXML_SOURCES EXCLUDE REGEX "/xmltest\\.cpp$")
file(GLOB NATIVE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/native/*.mm")

add_executable(OnePiece MACOSX_BUNDLE ${ENGINE_SOURCES} ${PROTOBUF_SOURCES}
    ${GAME_SOURCES} ${LUA_SOURCES} ${LUA_BINDINGS} ${JSON_SOURCES} ${TINYXML_SOURCES}
    ${NATIVE_SOURCES} "${CLIENT}/Game/Platform/GamePlatform.mm" "${CLIENT}/91lib/include/libPlatform.cpp")
# This legacy .cpp contains Objective-C imports on Apple platforms.
set_source_files_properties("${CLIENT}/91lib/include/libPlatform.cpp" PROPERTIES LANGUAGE OBJCXX)
target_compile_definitions(OnePiece PRIVATE CC_TARGET_OS_IPHONE=1 PROJECTIOSLOCAL=1
    CC_ENABLE_CHIPMUNK_INTEGRATION=0 CC_ENABLE_BOX2D_INTEGRATION=0 CURL_STATICLIB=1 LUA_USE_POSIX=1)
target_compile_options(OnePiece PRIVATE -Wno-deprecated-declarations)
target_include_directories(OnePiece BEFORE PRIVATE "${webp_SOURCE_DIR}/src" "${webp_SOURCE_DIR}/src/webp"
    "${curl_SOURCE_DIR}/include" "${curl_BINARY_DIR}/include")
set(INCLUDE_DIRS "${CLIENT}" "${CLIENT}/cocos2dx" "${CLIENT}/cocos2dx/include"
    "${CLIENT}/cocos2dx/platform" "${CLIENT}/cocos2dx/platform/ios"
    "${CLIENT}/cocos2dx/kazmath/include" "${CLIENT}/CocosDenshion/include"
    "${CLIENT}/extensions" "${CLIENT}/external" "${CLIENT}/Game/Classes"
    "${CLIENT}/Game/Platform" "${CLIENT}/Game/Protobuf" "${CLIENT}/91lib/include"
    "${CLIENT}/libOS/include" "${CLIENT}/protobuf-2.5.0rc1/src"
    "${CLIENT}/jsoncpp-src-0.5.0/include" "${CLIENT}/tinyxml"
    "${CLIENT}/scripting/lua/lua" "${CLIENT}/scripting/lua/tolua"
    "${CLIENT}/scripting/lua/cocos2dx_support" "${CMAKE_CURRENT_SOURCE_DIR}/native")
foreach(source IN LISTS ENGINE_SOURCES)
    get_filename_component(directory "${source}" DIRECTORY)
    list(APPEND INCLUDE_DIRS "${directory}")
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)
target_include_directories(OnePiece PRIVATE ${INCLUDE_DIRS})
find_package(LibXml2 REQUIRED)
target_link_libraries(OnePiece PRIVATE CURL::libcurl webp LibXml2::LibXml2 z sqlite3 iconv)
foreach(framework UIKit Foundation CoreGraphics QuartzCore OpenGLES AudioToolbox
    AVFoundation CoreMotion SystemConfiguration Security WebKit UserNotifications)
    find_library(OP_${framework} ${framework} REQUIRED)
    target_link_libraries(OnePiece PRIVATE "${OP_${framework}}")
endforeach()

# Copy directories without flattening lua/, sound/ and texture subdirectories.
add_custom_command(TARGET OnePiece POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${OP_ASSET_DIR}" "$<TARGET_BUNDLE_DIR:OnePiece>"
    VERBATIM)
set_target_properties(OnePiece PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${OP_BUNDLE_ID}"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${OP_BUNDLE_ID}"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO
    XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES NO
    XCODE_ATTRIBUTE_ENABLE_BITCODE NO
    XCODE_ATTRIBUTE_ENABLE_USER_SCRIPT_SANDBOXING NO
    XCODE_ATTRIBUTE_SKIP_INSTALL NO
    XCODE_ATTRIBUTE_INSTALL_PATH "/Applications"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE Automatic
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${OP_TEAM_ID}"
    XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/native/Prefix.h"
    XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME ""
    XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos iphonesimulator")
