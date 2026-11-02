#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include <deque>
#include <emscripten/websocket.h>

class ThreadSocket : public Singleton<ThreadSocket> {
public:
    enum SOCKET_STATE { SS_UNINITIALIZED, SS_WAIT, SS_CONNECTING, SS_CONNECT_FAILED,
        SS_SENDING, SS_SEND_FAILED, SS_RECEIVING, SS_RECEIVE_DONE, SS_CONNECTION_LOST };
    struct PacketData { void* buffer; int length; };
    ThreadSocket();
    ~ThreadSocket();
    void connect(const std::string& ip, int port);
    void reconnect();
    void disconnect();
    void sendPacket(PacketData data, int opcode);
    void update();
    void setState(SOCKET_STATE value) { state = value; }
    SOCKET_STATE getState() { return state; }
    // Callbacks only enqueue bytes/state. Game listeners run from update(), so a
    // reconnect inside a listener cannot reenter a WebSocket callback.
    EMSCRIPTEN_WEBSOCKET_T handle;
    SOCKET_STATE state;
    std::string address;
    int port;
    size_t queued;
    double deadline;
    uintptr_t generation;
    std::deque<std::vector<unsigned char> > incoming;
    std::deque<int> failures;
    struct Pending { std::vector<unsigned char> bytes; int opcode; };
    std::deque<Pending> outgoing;
    size_t sending;
};
