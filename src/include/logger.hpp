#pragma once
#include "lockqueue.hpp"
#include <string>
#include <cstdio>
#include <thread>

enum LogLevel {
    INFO,
    ERROR
};

#define LOG_INFO(logformat, ...) \
        do { \
            Logger* lg = Logger::getInstance();\
            lg->setLogLevel(LogLevel::INFO);\
            char buf[128] = {0};\
            snprintf(buf, sizeof buf, logformat, ##__VA_ARGS__);\
            lg->log(buf);\
        } while(0);

#define LOG_ERROR(logformat, ...) \
        do { \
            Logger* lg = Logger::getInstance();\
            lg->setLogLevel(LogLevel::ERROR);\
            char buf[128] = {0};\
            snprintf(buf, sizeof buf, logformat, ##__VA_ARGS__);\
            lg->log(buf);\
        } while(0);

class Logger {
public:
    static Logger* getInstance();
    void setLogLevel(LogLevel level);
    void log(std::string msg);
private:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    void writeLog();
private:
    LockQueue<std::string> _lkque;
    LogLevel _logLevel;
};