#include "mprpcapplication.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

MprpcApplication::MprpcApplication() { }

MprpcApplication* MprpcApplication::getInstance() {
    static MprpcApplication app;
    return &app;
}

void showCommandHelp () {
    std::cout << "format: ./a.out -i configFile" << std::endl;
}

void MprpcApplication::init(int argc, char** argv) {
    if(argc < 2) {
        showCommandHelp();
        exit(0);
    }
    
    char arg = 0;
    std::string configFile;
    //"i:"的意思是寻找-i选项，且必须携带参数
    while((arg = getopt(argc, argv, "i:")) != -1) {
        switch(arg) {
            case 'i': //找到选项，按照要求携带了参数，optarg指向该参数字符串
                configFile = std::string(optarg);
                break;
            case '?': //无法识别的选项
                std::cout << "invalid command!" << std::endl;
                showCommandHelp();
                exit(0);
            case ':': //选项缺少参数
                std::cout << "missing configFile!" << std::endl;
                showCommandHelp();
                exit(0);
            default:
                showCommandHelp();
                exit(0);
        }
    }

    //根据configFile配置文件加载配置
    //rpcServer: ip/port; zookeeper: ip/port;

}