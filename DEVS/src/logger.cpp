#include "logger.hpp"

Logger::Logger(const std::string& filename) {
    outFile.open(filename);
}

Logger::~Logger() {
    if (outFile.is_open()) outFile.close();
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&)) {
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

Logger logger("log.txt");
