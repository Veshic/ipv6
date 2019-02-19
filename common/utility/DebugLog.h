#pragma once
#include <string>

class Debug
{
public:
    static void Log(std::string log, ...)
    {
        va_list va;
        va_start(va, log);
        log += "\n";
        vprintf(log.c_str(), va);
        va_end(va);
    }
};