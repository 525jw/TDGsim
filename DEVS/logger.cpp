#include "logger.hpp"
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <cmath>

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
        using endl_t = std::ostream& (*)(std::ostream&);
        if (manip == static_cast<endl_t>(std::endl)) ++lineCount_;
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
bool Logger::limitReached() const {
    if (maxLines_ == 0) return false;  // 0이면 라인 제한 없음
    return lineCount_ >= maxLines_;
}

/* ───────────── 유틸 ───────────── */

std::string ToFixedString(float value, int precision) {
    if (!std::isfinite(value) || std::fabs(static_cast<double>(value)) >= 9.9e29) {
        return "INF";
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

/* ───────────── 전역 로거 정의 ───────────── */
#ifndef DISABLE_LOG
Logger logger_system{"logs/log_system.txt",0};
Logger logger_simulation {"logs/log_simulation.txt"};
#else
DummyLogger logger_system;
DummyLogger logger_simulation;
#endif

/* ───────────── 기타 로그 API ───────────── */

void LogTA(double simTime, std::string_view modelName, float taValue) {
#ifndef DISABLE_LOG
    std::ostringstream oss;
    oss << "[t=" << std::fixed << std::setprecision(3) << simTime << "]"
        << "[TA] "
        << "model=" << modelName
        << " value=" << ToFixedString(taValue, 3);
    logger_system << oss.str() << std::endl;
#else
    (void)simTime; (void)modelName; (void)taValue;
#endif
}

void LogState(double simTime, std::string_view modelName, std::string_view state) {
#ifndef DISABLE_LOG
    std::ostringstream oss;
    oss << "[t=" << std::fixed << std::setprecision(3) << simTime << "]"
        << "[STATE] "
        << "model=" << modelName
        << " value=" << state;
    logger_system << oss.str() << std::endl;
#else
    (void)simTime; (void)modelName; (void)state;
#endif
}
