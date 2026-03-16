#pragma once
#include <iostream>
#include <string>
#include <ctime>

enum class LogLevel { Info, Warning, Error , Critical};

class Logger {
  public: 
    static void Log(LogLevel level, const std::string& message, const char* file = "", int line = 0) {
        std::string prefix;
        switch (level) {
            case LogLevel::Info: prefix = "[INFO] "; break;
            case LogLevel::Warning: prefix = "[WARNING] "; break;
            case LogLevel::Error: prefix = "[ERROR] "; break;
            case LogLevel::Critical: prefix = "[CRITICAL] "; break;
        }

        std::time_t now = std::time(nullptr);
        char timeBuffer[20];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        std::cout << timeBuffer << " " << prefix << " " << message
                  << " (File: " << file << ", Line: " << line << ")" << std::endl;
    }
};

#define CORE_INFO(msg)     Logger::Log(LogLevel::Info, msg, __FILE__, __LINE__)
#define CORE_WARN(msg)     Logger::Log(LogLevel::Warning, msg, __FILE__, __LINE__)
#define CORE_ERROR(msg)    Logger::Log(LogLevel::Error, msg, __FILE__, __LINE__)