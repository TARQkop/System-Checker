#include "DiskMonitor.h"

#include "ConsoleRenderer.h"

#include <windows.h>
#include <winioctl.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

void DiskMonitor::render(const ConsoleRenderer& ui) const {
    ui.printSection("DISK");

    DWORD len = GetLogicalDriveStringsA(0, nullptr);
    if (len == 0) {
        std::cout << "Disk info unavailable (GetLogicalDriveStringsA failed).\n";
        return;
    }

    std::vector<char> drives(len + 1, '\0');
    if (GetLogicalDriveStringsA(len, drives.data()) == 0) {
        std::cout << "Disk info unavailable (GetLogicalDriveStringsA returned 0).\n";
        return;
    }

    bool foundAny = false;
    const char* drive = drives.data();
    while (*drive != '\0') {
        foundAny = true;
        UINT type = GetDriveTypeA(drive);
        const char* media = (type == DRIVE_FIXED) ? detectMediaType(drive) : "-";

        std::cout << "Drive " << std::left << std::setw(8) << drive
                  << " Type: " << std::setw(10) << driveTypeToString(type)
                  << " Media: " << media << '\n';

        ULARGE_INTEGER freeBytes{}, totalBytes{}, totalFreeBytes{};
        if (GetDiskFreeSpaceExA(drive, &freeBytes, &totalBytes, &totalFreeBytes)) {
            double totalGB = toGB(totalBytes.QuadPart);
            double freeGB = toGB(totalFreeBytes.QuadPart);
            double usedGB = totalGB - freeGB;
            double usage = (totalGB > 0.0) ? (usedGB * 100.0 / totalGB) : 0.0;

            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  " << std::left << std::setw(19) << "Total" << ": " << totalGB << " GB\n";
            std::cout << "  " << std::left << std::setw(19) << "Used" << ": " << usedGB << " GB\n";
            std::cout << "  " << std::left << std::setw(19) << "Free" << ": " << freeGB << " GB\n";
            std::cout << "  " << std::left << std::setw(19) << "Usage" << ": ";
            ui.printPercentInline(usage);
        } else {
            std::cout << "  Capacity data unavailable.\n";
        }

        std::cout << '\n';
        drive += std::strlen(drive) + 1;
    }

    if (!foundAny) {
        std::cout << "No drives found.\n";
    }
}

double DiskMonitor::toGB(unsigned long long bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
}

const char* DiskMonitor::driveTypeToString(unsigned int type) {
    switch (type) {
        case DRIVE_FIXED: return "Fixed";
        case DRIVE_REMOVABLE: return "Removable";
        case DRIVE_REMOTE: return "Network";
        case DRIVE_CDROM: return "CD-ROM";
        case DRIVE_RAMDISK: return "RAM Disk";
        case DRIVE_NO_ROOT_DIR: return "No Root";
        default: return "Unknown";
    }
}

const char* DiskMonitor::detectMediaType(const char* root) {
    if (!root || std::strlen(root) < 2) return "Unknown";

    char path[] = "\\\\.\\X:";
    path[4] = root[0];
    HANDLE h = CreateFileA(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return "Unknown";

    STORAGE_PROPERTY_QUERY query{};
    query.PropertyId = StorageDeviceSeekPenaltyProperty;
    query.QueryType = PropertyStandardQuery;

    DEVICE_SEEK_PENALTY_DESCRIPTOR desc{};
    DWORD bytes = 0;
    BOOL ok = DeviceIoControl(
        h, IOCTL_STORAGE_QUERY_PROPERTY,
        &query, sizeof(query),
        &desc, sizeof(desc),
        &bytes, nullptr
    );

    CloseHandle(h);
    if (!ok) return "Unknown";
    return desc.IncursSeekPenalty ? "HDD" : "SSD";
}
