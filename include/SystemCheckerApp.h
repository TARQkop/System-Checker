#ifndef SYSTEM_CHECKER_APP_H
#define SYSTEM_CHECKER_APP_H

#include "ConsoleRenderer.h"
#include "CpuMonitor.h"
#include "DiskMonitor.h"
#include "NetworkMonitor.h"
#include "RamMonitor.h"

class SystemCheckerApp {
public:
    int run();

private:
    int refreshSeconds_ = 60;
    ConsoleRenderer renderer_;
    CpuMonitor cpu_;
    RamMonitor ram_;
    DiskMonitor disk_;
    NetworkMonitor network_;

    void handleInput(bool& forceRefresh, bool& running);
};

#endif
