#pragma once
#include "mprpcconfig.hpp"

//用于初始化rpc服务类
class MprpcApplication {
public:
    static MprpcApplication* getInstance();
    static void init(int argc, char** argv);
    MprpcConfig* getConfig();
private:
    MprpcApplication();
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
private:
    static MprpcConfig _config;
};