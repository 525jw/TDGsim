#pragma once
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

/* ───────────────── Logger 본체 ───────────────── */
class Logger final {
public:
    explicit Logger(const std::string& filename,
                    std::size_t maxLines = 20'000);   // 생성과 동시에 파일 open
    ~Logger();

    Logger(const Logger&)            = delete;        // 복사 금지
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(Logger&&)      = delete;

    /* 일반 데이터 출력 */
    template<typename T>
    Logger& operator<<(const T& data) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (limitReached()) return *this;
        if (outFile_.is_open()) outFile_ << data;
        return *this;
    }

    /* 조작자(endl 등) 처리 */
    Logger& operator<<(std::ostream& (*manip)(std::ostream&));

    /* 부가 기능 */
    void           setMaxLines(std::size_t max);
    std::size_t    lineCount()     const;
    bool           limitReached()  const;

private:
    std::ofstream  outFile_;
    std::size_t    lineCount_{0};
    std::size_t    maxLines_;
    std::mutex     mtx_;
};

/* ────────────── 빌드 옵션에 따른 전역 로거 선언 ────────────── */
/* DISABLE_LOG 를 정의하면 DummyLogger 로 대체 */
#ifndef DISABLE_LOG

extern Logger logger_system;   // 시스템 진단용
extern Logger logger_world;    // 월드/게임플레이 로그

#else

struct DummyLogger {
    template<typename T> DummyLogger& operator<<(const T&) { return *this; }
    DummyLogger& operator<<(std::ostream& (*)(std::ostream&)) { return *this; }
};

extern DummyLogger logger_system;
extern DummyLogger logger_world;

#endif
