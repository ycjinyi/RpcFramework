#include "rpcprovider.hpp"
#include "mprpcapplication.hpp"
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>
#include <string>
#include <functional>
#include "google/protobuf/descriptor.h"

 //rpc框架对外提供的发布服务的函数接口
void RpcProvider::notifyService(google::protobuf::Service* service) {
    serviceInfo sInfo;
    //首先通过service获取ServiceDescriptor
    const google::protobuf::ServiceDescriptor* sdp = service->GetDescriptor();
    //通过ServiceDescriptor可以获取service的名称
    std::string serviceName = sdp->name();
    std::cout << "service name: " << serviceName << std::endl;
    //还可以获取对应的方法个数
    int cnt = sdp->method_count();
    //逐一获取方法
    for(int i = 0; i < cnt; ++i) {
        const google::protobuf::MethodDescriptor* mdp = sdp->method(i);
        std::string methodName = mdp->name();
        std::cout << "method name: " << methodName << std::endl;
        //保存对应的方法
        sInfo._methodMap.insert({methodName, mdp});
    }
    sInfo.sdp = sdp;
    _serviceMap.insert({serviceName, sInfo});
}
//启动rpc服务节点，对外提供rpc调用服务
void RpcProvider::run() {
    //首先获取mprpcapplication实例
    MprpcApplication* app = MprpcApplication::getInstance();
    //根据配置设置rpc服务器的ip地址和端口号
    std::string ip = app->getConfig()->query("rpcserverip");
    uint16_t port = atoi(app->getConfig()->query("rpcserverport").c_str());
    //配置地址和端口号
    muduo::net::InetAddress addr(ip, port);
    //创建TcpServer
    muduo::net::TcpServer server(&_evLoop, addr, std::string("RpcServer"));
    //设置TcpServer的连接事件回调函数
    server.setConnectionCallback(
            std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    //读写事件回调函数
    server.setMessageCallback(
            std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, 
                std::placeholders::_2, std::placeholders::_3)
    );
    //设置线程数目
    server.setThreadNum(4);
    //运行
    server.start();
    std::cout << "RpcServer start at ip: " << ip << " port: " << port << std::endl;
    //循环运行
    _evLoop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn) {

}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr&, 
        muduo::net::Buffer*, muduo::Timestamp) {

}

