#pragma once
#include <fstream>
#include <iostream>

class Logger {
private:
    std::ofstream outFile;
    size_t lineCount = 0;
    size_t maxLines = 20000; 

public:
    Logger(const std::string& filename) {
        outFile.open(filename);
    }

    ~Logger() {
        if (outFile.is_open()) outFile.close();
    }

    template <typename T>
    Logger& operator<<(const T& data) {
        if (lineCount >= maxLines) return *this;
        if (outFile.is_open()) outFile << data;
        return *this;
    }

    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (lineCount >= maxLines) return *this;
        if (outFile.is_open()) {
            if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
                ++lineCount;
            }
            manip(outFile);
            outFile.flush();
        }
        return *this;
    }

    void setMaxLines(size_t max) { maxLines = max; }
    size_t getLineCount() const { return lineCount; }
    bool isLimitReached() const { return lineCount >= maxLines; }
};

extern Logger logger;
