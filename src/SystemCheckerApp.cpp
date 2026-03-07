#include "SystemCheckerApp.h"

#include "WsaSession.h"

#include <conio.h>

#include <chrono>
#include <iostream>
#include <thread>

int SystemCheckerApp::run() {
    renderer_.initUtf8();
    WsaSession wsa;
    if (!wsa.isActive()) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    renderer_.clearScreen();
    auto nextRefresh = std::chrono::steady_clock::now();
    bool forceRefresh = true;
    bool running = true;

    while (running) {
        auto now = std::chrono::steady_clock::now();
        if (forceRefresh || now >= nextRefresh) {
            renderer_.moveCursorHome();
            renderer_.printHeader(refreshSeconds_);
            cpu_.render(renderer_);
            ram_.render(renderer_);
            disk_.render(renderer_);
            network_.render(renderer_);
            std::cout << std::flush;

            forceRefresh = false;
            nextRefresh = std::chrono::steady_clock::now() + std::chrono::seconds(refreshSeconds_);
        }

        handleInput(forceRefresh, running);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    renderer_.resetColor();
    return 0;
}

void SystemCheckerApp::handleInput(bool& forceRefresh, bool& running) {
    if (!_kbhit()) return;

    int key = _getch();
    if (key == 'q' || key == 'Q') {
        running = false;
    } else if (key == 'r' || key == 'R') {
        forceRefresh = true;
    } else if (key == '+') {
        if (refreshSeconds_ < 300) ++refreshSeconds_;
        forceRefresh = true;
    } else if (key == '-') {
        if (refreshSeconds_ > 1) --refreshSeconds_;
        forceRefresh = true;
    }
}
