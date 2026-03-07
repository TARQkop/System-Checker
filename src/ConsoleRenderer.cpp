#include "ConsoleRenderer.h"

#include <iomanip>
#include <iostream>

ConsoleRenderer::ConsoleRenderer() : consoleHandle_(GetStdHandle(STD_OUTPUT_HANDLE)) {}

void ConsoleRenderer::initUtf8() const {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void ConsoleRenderer::clearScreen() const {
    if (consoleHandle_ == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi{};
    if (!GetConsoleScreenBufferInfo(consoleHandle_, &csbi)) return;

    DWORD count = static_cast<DWORD>(csbi.dwSize.X) * static_cast<DWORD>(csbi.dwSize.Y);
    DWORD written = 0;
    COORD home{0, 0};
    FillConsoleOutputCharacterA(consoleHandle_, ' ', count, home, &written);
    FillConsoleOutputAttribute(consoleHandle_, csbi.wAttributes, count, home, &written);
    SetConsoleCursorPosition(consoleHandle_, home);
}

void ConsoleRenderer::moveCursorHome() const {
    if (consoleHandle_ == INVALID_HANDLE_VALUE) return;
    COORD home{0, 0};
    SetConsoleCursorPosition(consoleHandle_, home);
}

void ConsoleRenderer::printHeader(int refreshSeconds) const {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    std::cout << "System Checker Monitor\n";
    std::cout << "Updated: " << std::setfill('0')
              << std::setw(4) << st.wYear << '-'
              << std::setw(2) << st.wMonth << '-'
              << std::setw(2) << st.wDay << ' '
              << std::setw(2) << st.wHour << ':'
              << std::setw(2) << st.wMinute << ':'
              << std::setw(2) << st.wSecond
              << std::setfill(' ') << '\n';
    std::cout << "Auto refresh: " << refreshSeconds
              << "s | Keys: [r] refresh now  [q] quit  [+/-] change interval\n";
}

void ConsoleRenderer::printSection(const std::string& title) const {
    std::cout << "\n+--------------------------------------------------+\n";
    std::cout << "| " << std::left << std::setw(48) << title << "|\n";
    std::cout << "+--------------------------------------------------+\n";
}

void ConsoleRenderer::printPercentLine(const std::string& label, double percent) const {
    std::cout << std::left << std::setw(22) << label << ": ";
    setColor(colorForPercent(percent));
    std::cout << percentBar(percent) << " " << std::fixed << std::setprecision(1) << percent << "%\n";
    resetColor();
}

void ConsoleRenderer::printPercentInline(double percent) const {
    setColor(colorForPercent(percent));
    std::cout << percentBar(percent) << " " << std::fixed << std::setprecision(1) << percent << "%\n";
    resetColor();
}

void ConsoleRenderer::resetColor() const {
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ConsoleRenderer::setColor(WORD color) const {
    if (consoleHandle_ != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(consoleHandle_, color);
    }
}

std::string ConsoleRenderer::percentBar(double percent, int width) {
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;

    int filled = static_cast<int>((percent / 100.0) * width + 0.5);
    std::string bar = "[";
    for (int i = 0; i < width; ++i) {
        bar += (i < filled) ? '#' : '-';
    }
    bar += "]";
    return bar;
}

WORD ConsoleRenderer::colorForPercent(double percent) {
    if (percent >= 80.0) return FOREGROUND_RED | FOREGROUND_INTENSITY;
    if (percent >= 75.0) return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
}
