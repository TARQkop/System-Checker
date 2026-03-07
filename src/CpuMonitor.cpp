#include "CpuMonitor.h"

#include "ConsoleRenderer.h"

#include <iomanip>
#include <iostream>

void CpuMonitor::render(const ConsoleRenderer& ui) {
    ui.printSection("CPU");

    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    std::cout << std::left << std::setw(22) << "Logical processors" << ": " << si.dwNumberOfProcessors << '\n';
    std::cout << std::left << std::setw(22) << "Architecture" << ": " << architectureToString(si.wProcessorArchitecture) << '\n';

    double usage = 0.0;
    if (sampleUsage(usage)) {
        ui.printPercentLine("Usage", usage);
    } else {
        std::cout << std::left << std::setw(22) << "Usage" << ": Calculating...\n";
    }
}

ULONGLONG CpuMonitor::toUll(const FILETIME& ft) {
    return (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

bool CpuMonitor::sampleUsage(double& usagePercent) {
    FILETIME idleFt{}, kernelFt{}, userFt{};
    if (!GetSystemTimes(&idleFt, &kernelFt, &userFt)) return false;

    ULONGLONG idle = toUll(idleFt);
    ULONGLONG kernel = toUll(kernelFt);
    ULONGLONG user = toUll(userFt);

    if (!initialized_) {
        initialized_ = true;
        prevIdle_ = idle;
        prevKernel_ = kernel;
        prevUser_ = user;
        return false;
    }

    ULONGLONG idleDelta = idle - prevIdle_;
    ULONGLONG kernelDelta = kernel - prevKernel_;
    ULONGLONG userDelta = user - prevUser_;
    ULONGLONG total = kernelDelta + userDelta;

    prevIdle_ = idle;
    prevKernel_ = kernel;
    prevUser_ = user;

    if (total == 0) return false;

    usagePercent = (static_cast<double>(total - idleDelta) * 100.0) / static_cast<double>(total);
    if (usagePercent < 0.0) usagePercent = 0.0;
    if (usagePercent > 100.0) usagePercent = 100.0;
    return true;
}

const char* CpuMonitor::architectureToString(WORD arch) {
    switch (arch) {
        case PROCESSOR_ARCHITECTURE_AMD64: return "x64";
        case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
        case PROCESSOR_ARCHITECTURE_ARM64: return "ARM64";
        default: return "Unknown";
    }
}
