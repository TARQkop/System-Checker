#ifndef DISK_MONITOR_H
#define DISK_MONITOR_H

class ConsoleRenderer;

class DiskMonitor {
public:
    void render(const ConsoleRenderer& ui) const;

private:
    static double toGB(unsigned long long bytes);
    static const char* driveTypeToString(unsigned int type);
    static const char* detectMediaType(const char* root);
};

#endif
