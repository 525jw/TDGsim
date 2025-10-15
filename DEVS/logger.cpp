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
        // endl 식별 시 줄수 증가
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
bool Logger::limitReached() const { return lineCount_ >= maxLines_; }

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
Logger logger_system{"logs/log_system.txt"};
Logger logger_simulation {"logs/log_simulation.txt"};
#else
DummyLogger logger_system;
DummyLogger logger_simulation;
#endif


/* ───────────── 최종 포맷 API ───────────── */

template <typename... Args>
void LogTrace(double simTime, std::string_view component, Args&&... args) {
#ifndef DISABLE_LOG
    std::ostringstream oss_msg;
    (oss_msg << ... << std::forward<Args>(args));

    std::ostringstream oss;
    oss << "[t=" << std::fixed << std::setprecision(3) << simTime << "]"
        << "[TRACE]"
        << "[" << component << "] "
        << oss_msg.str();
    logger_system << oss.str() << std::endl;
#else
    (void)simTime; (void)component;
#endif
}

template <typename... Args>
void LogError(double simTime, std::string_view component, Args&&... args) {
#ifndef DISABLE_LOG
    std::ostringstream oss_msg;
    (oss_msg << ... << std::forward<Args>(args));

    std::ostringstream oss;
    oss << "[t=" << std::fixed << std::setprecision(3) << simTime << "]"
        << "[ERROR]"
        << "[" << component << "] "
        << oss_msg.str();
    logger_system << oss.str() << std::endl;
#else
    (void)simTime; (void)component;
#endif
}

// TA: model/value만
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

// STATE: model/value만
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

#ifndef DISABLE_LOG
namespace {
std::string FormatFields(std::initializer_list<std::pair<std::string_view, std::string>> fields) {
    std::ostringstream oss;
    bool first = true;
    for (const auto& [key, value] : fields) {
        if (!first) oss << ' ';
        else first = false;
        oss << key << '=' << value;
    }
    return oss.str();
}
} // namespace

void LogSimulation(double simTime,
                   std::string_view actor,
                   std::string_view action,
                   std::initializer_list<std::pair<std::string_view, std::string>> extras)
{
    std::ostringstream oss;
    oss << '[' << std::fixed << std::setprecision(3) << simTime << "] "
        << actor << " : " << action;

    const std::string extra = FormatFields(extras);
    if (!extra.empty()) oss << ' ' << extra;

    logger_simulation << oss.str() << std::endl;
}
#else
void LogSimulation(double, std::string_view, std::string_view,
                   std::initializer_list<std::pair<std::string_view, std::string>>) {}
#endif
