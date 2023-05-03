#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.hpp"
#include "rpcchannel.hpp"
#include "mprpccontroller.hpp"
#include <google/protobuf/service.h>


int main(int argc, char** argv) {
    //读取配置文件，包括rpc服务器的ip和端口号
    MprpcApplication::init(argc, argv);
    //使用stub调用rcp方法，由MpRpcChannel实现数据的序列化和发送以及响应的接收
    fixbug::UserServiceRpc_Stub stub(new MpRpcChannel());
    //首先指定调用参数
    fixbug::LoginRequest request;
    fixbug::LoginResponse response;
    request.set_name("ycjinyi");
    request.set_pwd("123456");
    MpRpcController controller;
    //同步的调用过程，返回时表明已经收到了来自rpc服务器的数据
    stub.Login(&controller, &request, &response, nullptr);//实际上是调用->MpRpcChannel::CallMethod
    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
        return -1;
    }
    //获取响应数据
    if(response.result().errcode() == 1) {
        std::cout << "response error: " << response.result().errmsg() << std::endl;
        exit(0);
    }
    std::cout << "login: " << response.success() << std::endl;
    return 0;
}