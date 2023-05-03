#include "logger.hpp"
#include <iostream>
#include <functional>
#include <ctime>
#include <fstream>

Logger::Logger(): _logLevel(LogLevel::INFO) {
    //启动一个专门写日志的守护线程
    std::thread th(std::bind(&Logger::writeLog, this));
    th.detach();
}

Logger* Logger::getInstance() {
    static Logger lg;
    return &lg;
}

void Logger::setLogLevel(LogLevel level) {
    _logLevel = level;
}

void Logger::log(std::string msg) {
    time_t nowTime = time(nullptr);
    tm* tptr = localtime(&nowTime);
    const char* lp = _logLevel == LogLevel::INFO ? "INFO: " : "ERROR: ";
    char timeStamp[128] = {0};
    //hou-min-sec:msg
    sprintf(timeStamp, "%s%d-%d-%d: ", lp, tptr->tm_hour, tptr->tm_min, tptr->tm_sec);
    std::string tlog(std::string(timeStamp) + msg);
    _lkque.push(tlog);
}

void Logger::writeLog() {
    while(true) {
        time_t nowTime = time(nullptr);
        tm* tptr = localtime(&nowTime);
        char fileName[128] = {0};
        //year-month-day.txt
        sprintf(fileName, "%d-%d-%d.txt", tptr->tm_year + 1900, 
            tptr->tm_mon + 1, tptr->tm_mday);
        #if 0
            //以追加的方式打开文件
            FILE* fd = fopen(fileName, "a+");
            if(fd == nullptr) {
                std::cout << "open: " << fileName << " failed!" << std::endl;
                exit(EXIT_FAILURE);
            }
            do {
                std::string msg = _lkque.pop();
                msg.push_back('\n');
                fwrite(msg.c_str(), msg.size(), 1, fd);
            } while(_lkque.size());
            fclose(fd);
        #else
            std::ofstream of;
            of.open(fileName, std::ios_base::openmode::_S_app);
            do {
                std::string msg = _lkque.pop();
                msg.push_back('\n');
                of << msg;
            } while(_lkque.size());
            of.close();
        #endif
    }
}