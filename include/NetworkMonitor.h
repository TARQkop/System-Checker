#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <winsock2.h>
#include <iphlpapi.h>

#include <cstddef>
#include <chrono>
#include <string>
#include <unordered_map>

class ConsoleRenderer;

class NetworkMonitor {
public:
    void render(const ConsoleRenderer& ui);

private:
    struct ThroughputSnapshot {
        unsigned long long inOctets = 0;
        unsigned long long outOctets = 0;
        std::chrono::steady_clock::time_point time;
    };

    std::unordered_map<unsigned long, ThroughputSnapshot> history_;

    static std::string wideToUtf8(const wchar_t* wide);
    static const char* statusToString(IF_OPER_STATUS status);
    bool sampleThroughput(unsigned long index, double& downBps, double& upBps);
    static std::string formatRate(double bytesPerSec);
    static void printIpList(IP_ADAPTER_ADDRESSES* adapter, int family, const char* label, size_t ipBufferSize);
};

#endif
