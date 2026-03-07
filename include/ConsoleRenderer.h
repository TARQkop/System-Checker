#ifndef CONSOLE_RENDERER_H
#define CONSOLE_RENDERER_H

#include <winsock2.h>
#include <windows.h>

#include <string>

class ConsoleRenderer {
public:
    ConsoleRenderer();
    void initUtf8() const;
    void clearScreen() const;
    void moveCursorHome() const;
    void printHeader(int refreshSeconds) const;
    void printSection(const std::string& title) const;
    void printPercentLine(const std::string& label, double percent) const;
    void printPercentInline(double percent) const;
    void resetColor() const;

private:
    HANDLE consoleHandle_;

    void setColor(WORD color) const;
    static std::string percentBar(double percent, int width = 24);
    static WORD colorForPercent(double percent);
};

#endif
