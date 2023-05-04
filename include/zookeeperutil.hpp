#pragma once
#include <zookeeper/zookeeper.h>
#include <string>

class Zookeeper {
public:
    Zookeeper();
    ~Zookeeper();
    bool start();
    bool create(const std::string& path, const std::string& data, int state = 0);
    bool create(const char* path, const char* data, int dataLen, int state = 0);
    std::string getData(const std::string& path);
    std::string getData(const char* path);
private:
    zhandle_t* _zhandle;
};