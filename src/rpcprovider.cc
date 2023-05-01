#include "rpcprovider.hpp"
#include "mprpcapplication.hpp"
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>
#include <string>
#include <functional>
#include "google/protobuf/descriptor.h"
#include "rpcheader.pb.h"

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
    sInfo.service = service;
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
    //rpc服务调用是短连接，当对端关闭连接后，本段也需要关闭连接
    if(!conn->connected()) {
        conn->shutdown();    
    }
}

//格式约定如下:
//head_size(4字节)rpcHeaderargs
//rpcHeader => service_name method_name arg_size
//由于TCP是基于字节流传输的，因此首先需要通过首部的4字节长度来截取rpcHeader
//随后就可以通过protobuf反序列化rpcHeader获得相关信息
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,
        muduo::net::Buffer* buffer, muduo::Timestamp time) {
    //对端有消息事件，表明对端发送了rpc请求，此时需要解析服务和方法名，通过服务和方法名
    //解析对应的参数，然后根据表来调用对应的函数，并将结果返回。
    std::string buf(buffer->retrieveAllAsString());
    //解析
    //1、首部长度4字节，注意是二进制格式表示的长度，不是字符形式
    uint32_t header_size = 0;
    if(buf.size() < 4) {
        std::cout << "incomplete header_size data!" << std::endl;
        return;
    }
    for(int i = 3; i >= 0; --i) {
        header_size <<= 8;
        header_size += buf[i];
    }
    //2、protobuf反序列化
    if(buf.size() < 4 + header_size) {
        std::cout << "incomplete header data!" << std::endl;
        return;
    }
    mprpc::RpcHeader header;
    if(!header.ParseFromString(buf.substr(4, header_size))) {
        std::cout << "parse header failed!" << std::endl;
        return;
    }
    std::string service_name(header.service_name());
    std::string method_name(header.method_name());
    uint32_t arg_size = header.arg_size();
    //3、获取参数
    if(buf.size() < 4 + header_size + arg_size) {
        std::cout << "incomplete arg data!" << std::endl;
        return;
    }
    std::string args(buf.substr(4 + header_size, arg_size));
    //debug
    std::cout << "================================" << std::endl;
    std::cout << "recv: " << buf << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: "  << method_name << std::endl;
    std::cout << "args: " << args << std::endl;
    std::cout << "================================" << std::endl;
    //获取services
    auto it = _serviceMap.find(service_name);
    if(it == _serviceMap.end()) {
        std::cout << "service: " << service_name << " not exist!" << std::endl;
        return;
    }
    //获取method
    auto mit = it->second._methodMap.find(method_name);
    if(mit == it->second._methodMap.end()) {
        std::cout << service_name << "->method: " << method_name << "not exist!" << std::endl;
        return;
    }
    //找到service和methodDescriptor
    google::protobuf::Service* service = it->second.service;
    const google::protobuf::MethodDescriptor* mdp = mit->second;
    //通过service找到method对应的request和response参数
    google::protobuf::Message* request = service->GetRequestPrototype(mdp).New();
    google::protobuf::Message* response = service->GetResponsePrototype(mdp).New();
    //解析request参数
    if(!request->ParseFromString(args)) {
        std::cout << "parse args failed!" << std::endl;
        return;
    }
    //指定回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider,
        const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(
        this, &RpcProvider::rpcResponseCall, conn, response);
    //执行service的回调函数
    service->CallMethod(mdp, nullptr, request, response, done);
}

void RpcProvider::rpcResponseCall(const muduo::net::TcpConnectionPtr& conn, 
        google::protobuf::Message* msg) {//done->run方法执行后调用
    std::string response;
    if(!msg->SerializeToString(&response)) {
        std::cout << "response serialize to string failed!" << std::endl;
        conn->shutdown();
        return;
    }
    //将rpc方法的调用结果返回给调用方
    conn->send(response);
    //一次rpc调用类似于http短连接，由rpcprovider发送结果后主动关闭连接
    conn->shutdown();
}

