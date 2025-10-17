#pragma once
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <sstream>
#include <iomanip>

/* ───────────── Logger 본체 ───────────── */
class Logger final {
public:
    explicit Logger(const std::string& filename,
                    std::size_t maxLines = 20'000);
    ~Logger();

    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(Logger&&)      = delete;

    /* 일반 출력 */
    template <typename T>
    Logger& operator<<(const T& data) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (limitReached()) return *this;
        if (outFile_.is_open()) outFile_ << data;
        return *this;
    }

    /* 조작자(endl 등) */
    Logger& operator<<(std::ostream& (*manip)(std::ostream&));

    /* 부가 기능 */
    void        setMaxLines(std::size_t max);
    std::size_t lineCount()    const;
    bool        limitReached() const;

private:
    std::ofstream outFile_;
    std::size_t   lineCount_ = 0;
    std::size_t   maxLines_  = 0;
    std::mutex    mtx_;
};

/* ───────────── 포맷 유틸 ───────────── */
std::string ToFixedString(float value, int precision = 3);

#ifndef DISABLE_LOG
extern Logger logger_system;
extern Logger logger_simulation;
#else
struct DummyLogger {
    template <typename T> DummyLogger& operator<<(const T&) { return *this; }
    DummyLogger& operator<<(std::ostream& (*)(std::ostream&)) { return *this; }
};
extern DummyLogger logger_system;
extern DummyLogger logger_simulation;
#endif

/* ───────────── 로그 API ───────────── */
template <typename... Args>
void LogTrace(double simTime, std::string_view component, Args&&... args) {
#ifndef DISABLE_LOG
    std::ostringstream msg;
    (msg << ... << std::forward<Args>(args));

    std::ostringstream oss;
    oss << "[t=" << std::fixed << std::setprecision(3) << simTime << "]"
        << "[TRACE]"
        << "[" << component << "] ";
    if (!msg.str().empty()) oss << msg.str();

    logger_system << oss.str() << std::endl;
#else
    (void)simTime; (void)component;
#endif
}

template <typename... Args>
void LogError(double simTime, std::string_view component, Args&&... args) {
#ifndef DISABLE_LOG
    std::ostringstream msg;
    (msg << ... << std::forward<Args>(args));

    std::ostringstream oss;
    oss << "[t=" << std::fixed << std::setprecision(3) << simTime << "]"
        << "[ERROR]"
        << "[" << component << "] ";
    if (!msg.str().empty()) oss << msg.str();

    logger_system << oss.str() << std::endl;
#else
    (void)simTime; (void)component;
#endif
}

void LogTA   (double simTime, std::string_view modelName, float taValue);
void LogState(double simTime, std::string_view modelName, std::string_view state);

/* ───────────── 시뮬레이션 로그 ───────────── */
template <typename... Args>
void LogSimulation(double simTime,
                   std::string_view actor,
                   std::string_view action,
                   Args&&... args) {
#ifndef DISABLE_LOG
    std::ostringstream msg;
    (msg << ... << std::forward<Args>(args));

    std::ostringstream oss;
    oss << "[" << std::fixed << std::setprecision(3) << simTime << "] "
        << actor << " : " << action;
    if (!msg.str().empty()) oss << " " << msg.str();

    logger_simulation << oss.str() << std::endl;
#else
    (void)simTime; (void)actor; (void)action;
#endif
}

