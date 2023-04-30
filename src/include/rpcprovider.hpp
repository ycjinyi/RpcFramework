#pragma once
#include "google/protobuf/service.h"

//rpc框架提供的用于发布服务的类
class RpcProvider {
public:
    //rpc框架对外提供的发布服务的函数接口
    void notifyService(google::protobuf::Service*);
    //启动rpc服务节点，对外提供rpc调用服务
    void run();
private:
};