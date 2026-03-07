#ifndef RAM_MONITOR_H
#define RAM_MONITOR_H

class ConsoleRenderer;

class RamMonitor {
public:
    void render(const ConsoleRenderer& ui) const;

private:
    static double toGB(unsigned long long bytes);
};

#endif
