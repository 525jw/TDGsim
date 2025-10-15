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
Logger logger_system{"logs/log_system.txt"};
Logger logger_world {"logs/log_simulation.txt"};

namespace {
std::string FormatFields(std::initializer_list<std::pair<std::string_view, std::string>> fields) {
    std::ostringstream oss;
    bool first = true;
    for (const auto& [key, value] : fields) {
        if (!first) {
            oss << ' ';
        } else {
            first = false;
        }
        oss << key << '=' << value;
    }
    return oss.str();
}
} // namespace

void LogSystem(double simTime,
             std::string_view level,
             std::initializer_list<std::pair<std::string_view, std::string>> fields)
{

    std::ostringstream oss;
    oss << '[' << std::fixed << std::setprecision(3) << simTime
        << "][" << level << ']';

    std::ostringstream payload;

    const std::string extras = FormatFields(fields);
    if (!extras.empty()) {
        payload << ' ' << extras;
    }

    const std::string payloadStr = payload.str();
    if (!payloadStr.empty()) {
        oss << ' ' << payloadStr;
    }

    logger_system << oss.str() << std::endl;
}

void LogSimulation(double simTime,
                   std::string_view actor,
                   std::string_view action,
                   std::initializer_list<std::pair<std::string_view, std::string>> extras)
{
    std::ostringstream oss;
    oss << '[' << std::fixed << std::setprecision(3) << simTime << "] "
        << actor << ' : ' << action;

    const std::string extra = FormatFields(extras);
    if (!extra.empty()) {
        oss << ' ' << extra;
    }

    logger_world << oss.str() << std::endl;
}
#else
DummyLogger logger_system;
DummyLogger logger_simulation;

void LogSystem(double,
             std::string_view,
             std::initializer_list<std::pair<std::string_view, std::string>>) {}

void LogSimulation(double,
                   std::string_view,
                   std::string_view,
                   std::initializer_list<std::pair<std::string_view, std::string>>) {}
#endif
