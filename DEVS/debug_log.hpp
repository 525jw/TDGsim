#pragma once
#include <iostream>
#include <sstream>

// include while using DEBUG_LOG

#ifdef _DEBUG
    template<typename... Args>  // 가변 템플릿, 인자 개수 신경 안씀
    void DEBUG_LOG(Args&&... args) {
        std::ostringstream oss; // 문자열 스트림 생성
        (oss << ... << args);   // 인자로 들어온 문자열 조립립
        std::cout << "[DEBUG] " << oss.str() << std::endl;
    }
#else
    template<typename... Args>
    void DEBUG_LOG(Args&&...) { }
#endif
