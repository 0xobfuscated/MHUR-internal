#pragma once
#include <Windows.h>
#include <cstdio>

enum LogType {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_TRACE
};

namespace Console {
    void Allocate();
    void Free();
    void Log(LogType type, const char* fmt, ...);
}

#ifdef _DEBUG
#define LOG(type, fmt, ...) Console::Log(type, fmt, __VA_ARGS__)
#else
#define LOG(type, fmt, ...)
#endif
