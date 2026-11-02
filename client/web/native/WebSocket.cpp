#include "ThreadSocket.h"
#include "PacketManager.h"
#include "cocos2d.h"
#include <emscripten.h>
#include <memory>

static const size_t LIMIT = 2 * 1024 * 1024;
static ThreadSocket* current = 0;
static uintptr_t nextGeneration = 0;
static ThreadSocket* callbackTarget(void* user) {
    return current && current->generation == reinterpret_cast<uintptr_t>(user) ? current : 0;
}
static EM_BOOL opened(int, const EmscriptenWebSocketOpenEvent* event, void* user) {
    ThreadSocket* self = callbackTarget(user);
    if (self && event->socket == self->handle) self->state = ThreadSocket::SS_WAIT;
    return true;
}
static EM_BOOL received(int, const EmscriptenWebSocketMessageEvent* event, void* user) {
    ThreadSocket* self = callbackTarget(user);
    if (!self || event->socket != self->handle) return true;
    if (event->isText || self->queued + event->numBytes > LIMIT) {
        self->state = ThreadSocket::SS_CONNECTION_LOST;
        return true;
    }
    const unsigned char* bytes = static_cast<const unsigned char*>(event->data);
    self->incoming.push_back(std::vector<unsigned char>(bytes, bytes + event->numBytes));
    self->queued += event->numBytes;
    return true;
}
static EM_BOOL closed(int, const EmscriptenWebSocketCloseEvent* event, void* user) {
    ThreadSocket* self = callbackTarget(user);
    if (self && event->socket == self->handle)
        self->state = self->state == ThreadSocket::SS_CONNECTING ? ThreadSocket::SS_CONNECT_FAILED : ThreadSocket::SS_CONNECTION_LOST;
    return true;
}
static EM_BOOL failed(int, const EmscriptenWebSocketErrorEvent* event, void* user) {
    ThreadSocket* self = callbackTarget(user);
    if (self && event->socket == self->handle)
        self->state = self->state == ThreadSocket::SS_CONNECTING ? ThreadSocket::SS_CONNECT_FAILED : ThreadSocket::SS_CONNECTION_LOST;
    return true;
}
ThreadSocket::ThreadSocket() : handle(0), state(SS_UNINITIALIZED), port(0), queued(0), deadline(0), generation(0), sending(0) { current = this; }
ThreadSocket::~ThreadSocket() { current = 0; disconnect(); }
void ThreadSocket::connect(const std::string& ip, int number) {
    disconnect(); address = ip; port = number; state = SS_CONNECTING;
    // The server-list address selects a realm. Browser traffic always stays on
    // the trusted same-origin gateway; never connect to an arbitrary list URL.
    char* url = reinterpret_cast<char*>(EM_ASM_PTR({
        return stringToNewUTF8((location.protocol === 'https:' ? 'wss://' : 'ws://') + location.host + '/web/game');
    }));
    EmscriptenWebSocketCreateAttributes attributes = {};
    attributes.url = url; attributes.protocols = "nrb-tcp-v1"; attributes.createOnMainThread = true;
    handle = emscripten_websocket_new(&attributes);
    free(url);
    if (handle <= 0) { handle = 0; state = SS_CONNECT_FAILED; return; }
    deadline = emscripten_get_now() + 15000;
    void* token = reinterpret_cast<void*>(generation);
    emscripten_websocket_set_onopen_callback(handle, token, opened);
    emscripten_websocket_set_onmessage_callback(handle, token, received);
    emscripten_websocket_set_onclose_callback(handle, token, closed);
    emscripten_websocket_set_onerror_callback(handle, token, failed);
    cocos2d::CCLog("[WebSocket] Connecting through browser gateway");
}
void ThreadSocket::reconnect() { std::string ip = address; int number = port; connect(ip, number); }
void ThreadSocket::disconnect() {
    generation = ++nextGeneration; // Late events cannot affect a reused SDK handle or singleton.
    if (handle > 0) {
        EMSCRIPTEN_WEBSOCKET_T old = handle; handle = 0;
        emscripten_websocket_close(old, 1000, "client disconnect");
        emscripten_websocket_delete(old);
    }
    incoming.clear(); outgoing.clear(); failures.clear(); queued = 0; sending = 0; state = SS_UNINITIALIZED;
    PacketManager::Get()->resetReceiveBuffer();
}
void ThreadSocket::sendPacket(PacketData data, int opcode) {
    std::unique_ptr<char[]> owned(static_cast<char*>(data.buffer));
    if (!owned || data.length <= 0 || static_cast<size_t>(data.length) + sending > LIMIT
            || (state != SS_WAIT && state != SS_CONNECTING)) {
        failures.push_back(opcode); return;
    }
    Pending packet; packet.opcode = opcode;
    packet.bytes.assign(owned.get(), owned.get() + data.length);
    sending += packet.bytes.size(); outgoing.push_back(packet);
}
void ThreadSocket::update() {
    if (state == SS_CONNECTING && emscripten_get_now() > deadline) state = SS_CONNECT_FAILED;
    if (state == SS_CONNECTION_LOST || state == SS_CONNECT_FAILED) {
        bool initial = state == SS_CONNECT_FAILED;
        std::string ip = address; int number = port;
        disconnect();
        cocos2d::CCLog("[WebSocket] Connection ended; waiting for explicit reconnect");
        if (initial) PacketManager::Get()->_boardcastConnectionFailed(ip, number);
        else PacketManager::Get()->_boardcastSendFailed(0);
        return;
    }
    if (!failures.empty()) {
        int opcode = failures.front(); failures.pop_front();
        PacketManager::Get()->_boardcastSendFailed(opcode); return;
    }
    unsigned long buffered = 0;
    while (state == SS_WAIT && !outgoing.empty()) {
        emscripten_websocket_get_buffered_amount(handle, &buffered);
        if (buffered > LIMIT / 2) break;
        Pending packet = outgoing.front(); outgoing.pop_front(); sending -= packet.bytes.size();
        if (emscripten_websocket_send_binary(handle, packet.bytes.data(), packet.bytes.size()) != EMSCRIPTEN_RESULT_SUCCESS) {
            failures.push_back(packet.opcode); state = SS_CONNECTION_LOST; break;
        }
    }
    // Do not assume that a WebSocket message is one Protobuf packet.
    for (int budget = 0; budget < 32 && state == SS_WAIT && !incoming.empty(); ++budget) {
        std::vector<unsigned char> bytes; bytes.swap(incoming.front()); incoming.pop_front(); queued -= bytes.size();
        if (!bytes.empty()) PacketManager::Get()->_onReceivedPacket(bytes.data(), bytes.size());
    }
}
