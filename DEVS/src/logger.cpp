#include "logger.hpp"
#include <filesystem>
#include <iomanip>   // 필요 시 사용

/* ───────────── Logger 구현 ───────────── */

Logger::Logger(const std::string& filename, std::size_t maxLines)
    : maxLines_(maxLines)
{
    namespace fs = std::filesystem;

    try {
        fs::path p{filename};
        if (p.has_parent_path() && !p.parent_path().empty())
            fs::create_directories(p.parent_path());
    } catch (...) {
        std::cerr << "[Logger] Failed to create directory, continuing anyway.\n";
    }

    outFile_.open(filename, std::ios::out | std::ios::trunc);
    if (!outFile_.is_open()) {
        std::cerr << "[Logger] '" << filename
                  << "' logs will be printed to console only.\n";
    }
}

Logger::~Logger() {
    if (outFile_.is_open()) outFile_.close();
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&)) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (limitReached()) return *this;
    if (outFile_.is_open()) {
        if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl))
            ++lineCount_;
        manip(outFile_);
        outFile_.flush();
    }
    return *this;
}

void Logger::setMaxLines(std::size_t max) {
    std::lock_guard<std::mutex> lock(mtx_);
    maxLines_ = max;
}

std::size_t Logger::lineCount() const { return lineCount_; }

bool Logger::limitReached() const { return lineCount_ >= maxLines_; }

/* ───────────── 전역 로거 정의 ───────────── */
#ifndef DISABLE_LOG
Logger logger_system{"logs/log_system.txt"};
Logger logger_world {"logs/log_world.txt"};
#else
DummyLogger logger_system;
DummyLogger logger_world;
#endif
