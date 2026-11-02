#include "cocos2d.h"
#include <SDL.h>
#include <SDL_image.h>
#include <webp/decode.h>
#include <emscripten.h>
#include <limits.h>

// Canvas text is used only for dynamic TTF strings. All CCBI sprites and bitmap
// fonts still come from the shared mobile assets and the original scene graph.
EM_JS(unsigned char*, textPixels, (const char* value, int width, int height, int align, int size, int* dimensions), {
    const canvas = document.createElement('canvas'), ctx = canvas.getContext('2d');
    size = Math.max(1, size || 16);
    const font = size + 'px "Microsoft YaHei", "PingFang SC", sans-serif';
    ctx.font = font;
    const lines = [];
    for (const paragraph of UTF8ToString(value).split('\n')) {
        let line = '';
        for (const character of paragraph) {
            if (width > 0 && line && ctx.measureText(line + character).width > width) { lines.push(line); line = ''; }
            line += character;
        }
        lines.push(line);
    }
    const lineHeight = Math.ceil(size * 1.3);
    const w = Math.max(1, width || Math.ceil(Math.max(...lines.map(line => ctx.measureText(line).width)) + 2));
    const h = Math.max(1, height || lines.length * lineHeight);
    if (w > 8192 || h > 8192) return 0;
    canvas.width = w; canvas.height = h; ctx.font = font; ctx.fillStyle = 'white'; ctx.textBaseline = 'top';
    const horizontal = align & 15, vertical = align >> 4;
    ctx.textAlign = horizontal === 1 ? 'left' : horizontal === 2 ? 'right' : 'center';
    const x = horizontal === 1 ? 0 : horizontal === 2 ? w : w / 2;
    const top = vertical === 1 ? 0 : vertical === 2 ? h - lines.length * lineHeight : (h - lines.length * lineHeight) / 2;
    lines.forEach((line, i) => ctx.fillText(line, x, top + i * lineHeight));
    const pixels = ctx.getImageData(0, 0, w, h).data, address = _malloc(pixels.length);
    if (!address) return 0;
    HEAPU8.set(pixels, address); HEAP32[dimensions >> 2] = w; HEAP32[(dimensions + 4) >> 2] = h;
    return address;
});

NS_CC_BEGIN
CCImage::CCImage() : m_nWidth(0), m_nHeight(0), m_nBitsPerComponent(8), m_pData(0), m_bHasAlpha(true), m_bPreMulti(false) {}
CCImage::~CCImage() { delete[] m_pData; }
bool CCImage::_initWithRawData(void* bytes, int length, int width, int height, int bits) {
    if (!bytes || width <= 0 || height <= 0 || width > 8192 || height > 8192 || bits != 8 || length < width * height * 4) return false;
    delete[] m_pData; m_pData = new unsigned char[width * height * 4];
    memcpy(m_pData, bytes, width * height * 4);
    m_nWidth = width; m_nHeight = height; m_nBitsPerComponent = 8; m_bHasAlpha = true; m_bPreMulti = true;
    for (int i = 0; i < width * height * 4; i += 4) {
        unsigned int alpha = m_pData[i + 3];
        for (int channel = 0; channel < 3; ++channel) m_pData[i + channel] = (m_pData[i + channel] * alpha + 127) / 255;
    }
    return true;
}
bool CCImage::initWithImageData(void* bytes, int length, EImageFormat format, int width, int height, int bits) {
    if (format == kFmtRawData) return _initWithRawData(bytes, length, width, height, bits);
    if (!bytes || length <= 0) return false;
    if (length >= 12 && !memcmp(bytes, "RIFF", 4) && !memcmp(static_cast<char*>(bytes) + 8, "WEBP", 4))
        return _initWithWebpData(bytes, length);
    SDL_Surface* decoded = IMG_Load_RW(SDL_RWFromConstMem(bytes, length), 1);
    if (!decoded) { CCLog("[WebImage] Decode failed: %s", IMG_GetError()); return false; }
    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(decoded, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(decoded);
    if (!rgba) return false;
    std::vector<unsigned char> pixels(rgba->w * rgba->h * 4);
    for (int row = 0; row < rgba->h; ++row)
        memcpy(pixels.data() + row * rgba->w * 4, static_cast<char*>(rgba->pixels) + row * rgba->pitch, rgba->w * 4);
    bool result = _initWithRawData(pixels.data(), pixels.size(), rgba->w, rgba->h, 8);
    SDL_FreeSurface(rgba); return result;
}
bool CCImage::_initWithWebpData(void* bytes, int length) {
    int width = 0, height = 0;
    if (!WebPGetInfo(static_cast<unsigned char*>(bytes), length, &width, &height) || width > 8192 || height > 8192) return false;
    unsigned char* pixels = WebPDecodeRGBA(static_cast<unsigned char*>(bytes), length, &width, &height);
    bool result = pixels && _initWithRawData(pixels, width * height * 4, width, height, 8);
    WebPFree(pixels); return result;
}
bool CCImage::initWithImageFile(const char* path, EImageFormat format) {
    unsigned long length = 0;
    unsigned char* data = CCFileUtils::sharedFileUtils()->getFileData(path, "rb", &length);
    bool result = data && initWithImageData(data, length, format);
    delete[] data; return result;
}
bool CCImage::initWithImageFileThreadSafe(const char* path, EImageFormat format) { return initWithImageFile(path, format); }
bool CCImage::_initWithJpgData(void* bytes, int length) { return initWithImageData(bytes, length, kFmtJpg); }
bool CCImage::_initWithPngData(void* bytes, int length) { return initWithImageData(bytes, length, kFmtPng); }
bool CCImage::_initWithTiffData(void*, int) { CCLog("[WebImage] TIFF assets are unsupported"); return false; }
bool CCImage::initWithString(const char* text, int width, int height, ETextAlign align, const char*, int size) {
    if (!text) return false;
    int dimensions[2] = {0, 0};
    unsigned char* pixels = textPixels(text, width, height, align, size, dimensions);
    bool result = pixels && _initWithRawData(pixels, dimensions[0] * dimensions[1] * 4, dimensions[0], dimensions[1], 8);
    free(pixels); return result;
}
bool CCImage::_saveImageToPNG(const char* path, bool) {
    if (!m_pData) return false;
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(m_pData, m_nWidth, m_nHeight, 32, m_nWidth * 4, SDL_PIXELFORMAT_RGBA32);
    if (!surface) return false;
    bool result = IMG_SavePNG(surface, path) == 0;
    SDL_FreeSurface(surface); return result;
}
bool CCImage::_saveImageToJPG(const char*) { CCLog("[WebImage] JPEG export is unsupported; use PNG"); return false; }
bool CCImage::saveToFile(const char* path, bool rgb) { return _saveImageToPNG(path, rgb); }
NS_CC_END
