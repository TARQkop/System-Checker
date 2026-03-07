#include "RamMonitor.h"

#include "ConsoleRenderer.h"

#include <windows.h>

#include <iomanip>
#include <iostream>

void RamMonitor::render(const ConsoleRenderer& ui) const {
    ui.printSection("RAM");

    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(mem);
    if (!GlobalMemoryStatusEx(&mem)) {
        std::cout << "RAM info not available.\n";
        return;
    }

    double totalPhys = toGB(mem.ullTotalPhys);
    double availPhys = toGB(mem.ullAvailPhys);
    double usedPhys = totalPhys - availPhys;

    double totalCommit = toGB(mem.ullTotalPageFile);
    double availCommit = toGB(mem.ullAvailPageFile);
    double usedCommit = totalCommit - availCommit;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(22) << "Physical total" << ": " << totalPhys << " GB\n";
    std::cout << std::left << std::setw(22) << "Physical used" << ": " << usedPhys << " GB\n";
    std::cout << std::left << std::setw(22) << "Physical free" << ": " << availPhys << " GB\n";
    ui.printPercentLine("Usage", static_cast<double>(mem.dwMemoryLoad));
    std::cout << std::left << std::setw(22) << "Commit total" << ": " << totalCommit << " GB\n";
    std::cout << std::left << std::setw(22) << "Commit used" << ": " << usedCommit << " GB\n";
    std::cout << std::left << std::setw(22) << "Commit free" << ": " << availCommit << " GB\n";
}

double RamMonitor::toGB(unsigned long long bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
}
