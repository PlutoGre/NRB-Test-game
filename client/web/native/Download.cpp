#include "CurlDownload.h"
#include "GameMaths.h"
#include "GamePlatform.h"
#include "cocos2d.h"
#include <emscripten/fetch.h>
#include <emscripten.h>
#include <sys/stat.h>
#include <deque>

namespace {
struct Result { std::string url, filename; unsigned short crc; bool check, success; };
std::deque<Result> completed;
emscripten_fetch_t* active = 0;
Result request;

void finish(emscripten_fetch_t* fetch) {
    Result result = request;
    result.success = fetch->status >= 200 && fetch->status < 300 && fetch->numBytes <= 32 * 1024 * 1024;
    if (result.success && result.check)
        result.success = GameMaths::GetCRC16(reinterpret_cast<unsigned char*>(fetch->data), fetch->numBytes) == result.crc;
    if (result.success) {
        std::string temporary = result.filename + ".download";
        saveFileInPath(temporary, "wb", reinterpret_cast<const unsigned char*>(fetch->data), fetch->numBytes);
        struct stat info;
        result.success = stat(temporary.c_str(), &info) == 0 && static_cast<size_t>(info.st_size) == fetch->numBytes;
        if (result.success) result.success = rename(temporary.c_str(), result.filename.c_str()) == 0;
        if (!result.success) remove(temporary.c_str());
    }
    active = 0; emscripten_fetch_close(fetch); completed.push_back(result);
}
}
CurlDownload::~CurlDownload() { if (active) emscripten_fetch_close(active); active = 0; completed.clear(); }
void CurlDownload::downloadFile(const std::string& url, const std::string& filename) { mDownloadQueue.push_back(DownloadFile(url, filename)); }
void CurlDownload::downloadFile(const std::string& url, const std::string& filename, unsigned short crc) { mDownloadQueue.push_back(DownloadFile(url, filename, crc)); }
void CurlDownload::update(float) {
    if (!completed.empty()) {
        Result value = completed.front(); completed.pop_front();
        if (!mDownloadQueue.empty()) mDownloadQueue.pop_front();
        if (!value.success) cocos2d::CCLog("[WebDownload] Resource request/write/CRC failed: %s", value.url.c_str());
        std::set<DownloadListener*> listeners = mListeners;
        for (std::set<DownloadListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
            if (!mListeners.count(*it)) continue;
            if (value.success) (*it)->downloaded(value.url, value.filename);
            else (*it)->downloadFailed(value.url, value.filename);
        }
    }
    if (active || mDownloadQueue.empty()) return;
    const DownloadFile& file = mDownloadQueue.front();
    request.url = file._url; request.filename = file._filename; request.crc = file._checkCRC ? file._crc : 0; request.check = file._checkCRC;
    // Deployment metadata may contain an old LAN host. Fetch resource paths from
    // the browser's authenticated origin so HTTPS never falls back to mixed HTTP.
    const std::string prefix = "/gameserver/dzm/1.8/";
    size_t position = file._url.find(prefix);
    std::string path = position == std::string::npos ? file._url : file._url.substr(position);
    if (path.compare(0, prefix.size(), prefix) != 0) {
        Result error = request; error.success = false; completed.push_back(error); return;
    }
    char* absolute = reinterpret_cast<char*>(EM_ASM_PTR({ return stringToNewUTF8(new URL(UTF8ToString($0), location.origin).href); }, path.c_str()));
    emscripten_fetch_attr_t attributes;
    emscripten_fetch_attr_init(&attributes);
    strcpy(attributes.requestMethod, "GET");
    attributes.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attributes.timeoutMSecs = 30000; attributes.withCredentials = true;
    attributes.onsuccess = finish; attributes.onerror = finish;
    active = emscripten_fetch(&attributes, absolute); free(absolute);
    if (!active) { Result error = request; error.success = false; completed.push_back(error); }
}
