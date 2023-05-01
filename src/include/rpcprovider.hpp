#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/EventLoop.h>
#include <unordered_map>

//rpc框架提供的用于发布服务的类
class RpcProvider {
public:
    //rpc框架对外提供的发布服务的函数接口
    void notifyService(google::protobuf::Service*);
    //启动rpc服务节点，对外提供rpc调用服务
    void run();
private:
    //eventLoop
    muduo::net::EventLoop _evLoop;
    //用于保存service name对应的ServiceDescriptor
    //以及所有的method name和MethodDescriptor
    struct serviceInfo {
        google::protobuf::Service* service;
        std::unordered_map<std::string, 
            const google::protobuf::MethodDescriptor*> _methodMap;
    };
    //用于保存service name和对应的serviceInfo之间的映射关系
    std::unordered_map<std::string, serviceInfo> _serviceMap;
private:
    //连接回调函数
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    //消息回调函数
    void onMessage(const muduo::net::TcpConnectionPtr&, 
        muduo::net::Buffer*, muduo::Timestamp);
    //rpc服务执行完毕后的回调函数
    void rpcResponseCall(const muduo::net::TcpConnectionPtr& conn, 
        google::protobuf::Message* msg);
};