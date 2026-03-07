#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include <winsock2.h>
#include <windows.h>

class ConsoleRenderer;

class CpuMonitor {
public:
    void render(const ConsoleRenderer& ui);

private:
    bool initialized_ = false;
    ULONGLONG prevIdle_ = 0;
    ULONGLONG prevKernel_ = 0;
    ULONGLONG prevUser_ = 0;

    static ULONGLONG toUll(const FILETIME& ft);
    bool sampleUsage(double& usagePercent);
    static const char* architectureToString(WORD arch);
};

#endif
