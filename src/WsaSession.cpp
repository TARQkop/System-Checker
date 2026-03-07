#include "WsaSession.h"

#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

WsaSession::WsaSession() {
    WSADATA data{};
    active_ = (WSAStartup(MAKEWORD(2, 2), &data) == 0);
}

WsaSession::~WsaSession() {
    if (active_) {
        WSACleanup();
    }
}

bool WsaSession::isActive() const {
    return active_;
}
