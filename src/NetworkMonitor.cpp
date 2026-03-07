#include "NetworkMonitor.h"

#include "ConsoleRenderer.h"

#include <ws2tcpip.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

void NetworkMonitor::render(const ConsoleRenderer& ui) {
    ui.printSection("NETWORK");

    ULONG bufferLen = 15 * 1024;
    std::vector<unsigned char> buffer(bufferLen);
    ULONG result = ERROR_BUFFER_OVERFLOW;

    for (int i = 0; i < 3 && result == ERROR_BUFFER_OVERFLOW; ++i) {
        auto* ptr = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
        result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, ptr, &bufferLen);
        if (result == ERROR_BUFFER_OVERFLOW) {
            buffer.resize(bufferLen);
        }
    }

    if (result != NO_ERROR) {
        std::cout << "Network info unavailable (GetAdaptersAddresses code: " << result << ").\n";
        return;
    }

    auto* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
    bool any = false;

    for (IP_ADAPTER_ADDRESSES* ad = adapters; ad != nullptr; ad = ad->Next) {
        any = true;
        std::string name = wideToUtf8(ad->FriendlyName);
        if (name.empty()) name = "(Unnamed)";

        ULONG idx = ad->IfIndex != 0 ? ad->IfIndex : ad->Ipv6IfIndex;

        std::cout << "Adapter: " << name << '\n';
        std::cout << "  Status            : " << statusToString(ad->OperStatus) << '\n';

        double downBps = 0.0;
        double upBps = 0.0;
        if (sampleThroughput(idx, downBps, upBps)) {
            std::cout << "  Download          : " << formatRate(downBps) << '\n';
            std::cout << "  Upload            : " << formatRate(upBps) << '\n';
        } else {
            std::cout << "  Download          : Calculating...\n";
            std::cout << "  Upload            : Calculating...\n";
        }

        printIpList(ad, AF_INET, "  IPv4              : ", INET_ADDRSTRLEN);
        printIpList(ad, AF_INET6, "  IPv6              : ", INET6_ADDRSTRLEN);
        std::cout << '\n';
    }

    if (!any) {
        std::cout << "No adapters found.\n";
    }
}

std::string NetworkMonitor::wideToUtf8(const wchar_t* wide) {
    if (!wide) return "";
    int needed = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    if (needed <= 1) return "";
    std::string out(static_cast<size_t>(needed - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, out.data(), needed, nullptr, nullptr);
    return out;
}

const char* NetworkMonitor::statusToString(IF_OPER_STATUS status) {
    switch (status) {
        case IfOperStatusUp: return "Up";
        case IfOperStatusDown: return "Down";
        case IfOperStatusTesting: return "Testing";
        case IfOperStatusDormant: return "Dormant";
        case IfOperStatusNotPresent: return "Not Present";
        case IfOperStatusLowerLayerDown: return "Lower Layer Down";
        default: return "Unknown";
    }
}

bool NetworkMonitor::sampleThroughput(unsigned long index, double& downBps, double& upBps) {
    if (index == 0) return false;

    MIB_IFROW row{};
    row.dwIndex = index;
    if (GetIfEntry(&row) != NO_ERROR) return false;

    auto now = std::chrono::steady_clock::now();
    auto it = history_.find(index);
    if (it == history_.end()) {
        history_[index] = ThroughputSnapshot{
            static_cast<unsigned long long>(row.dwInOctets),
            static_cast<unsigned long long>(row.dwOutOctets),
            now
        };
        return false;
    }

    double sec = std::chrono::duration<double>(now - it->second.time).count();
    if (sec <= 0.0) return false;

    unsigned long long currIn = static_cast<unsigned long long>(row.dwInOctets);
    unsigned long long currOut = static_cast<unsigned long long>(row.dwOutOctets);

    unsigned long long inDelta = (currIn >= it->second.inOctets) ? (currIn - it->second.inOctets) : 0;
    unsigned long long outDelta = (currOut >= it->second.outOctets) ? (currOut - it->second.outOctets) : 0;

    it->second = ThroughputSnapshot{currIn, currOut, now};
    downBps = static_cast<double>(inDelta) / sec;
    upBps = static_cast<double>(outDelta) / sec;
    return true;
}

std::string NetworkMonitor::formatRate(double bytesPerSec) {
    const char* units[] = {"B/s", "KB/s", "MB/s", "GB/s"};
    int unit = 0;
    while (bytesPerSec >= 1024.0 && unit < 3) {
        bytesPerSec /= 1024.0;
        ++unit;
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(unit == 0 ? 0 : 2) << bytesPerSec << ' ' << units[unit];
    return oss.str();
}

void NetworkMonitor::printIpList(IP_ADAPTER_ADDRESSES* adapter, int family, const char* label, size_t ipBufferSize) {
    std::cout << label;
    bool first = true;

    for (IP_ADAPTER_UNICAST_ADDRESS* ua = adapter->FirstUnicastAddress; ua != nullptr; ua = ua->Next) {
        SOCKADDR* sa = ua->Address.lpSockaddr;
        if (!sa || sa->sa_family != family) continue;

        std::vector<char> ip(ipBufferSize, '\0');
        DWORD ipLen = static_cast<DWORD>(ip.size());
        if (WSAAddressToStringA(sa, static_cast<DWORD>(ua->Address.iSockaddrLength), nullptr, ip.data(), &ipLen) == 0) {
            if (!first) std::cout << ", ";
            std::cout << ip.data();
            first = false;
        }
    }

    if (first) std::cout << "N/A";
    std::cout << '\n';
}
