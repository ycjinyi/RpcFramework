#include "rpcchannel.hpp"
#include "rpcheader.pb.h"
#include "mprpcapplication.hpp"
#include "mprpcconfig.hpp"
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller, 
        const google::protobuf::Message* request,
        google::protobuf::Message* response, 
        google::protobuf::Closure* done) {
        //1、首先按照约定的rpcheader构造数据
        const google::protobuf::ServiceDescriptor* sdp = method->service();
        std::string args;
        if(!request->SerializeToString(&args)) {
            controller->SetFailed("Serialize args failed!");
            return;
        }
        //构造rpcheader
        mprpc::RpcHeader header;
        header.set_service_name(sdp->name());
        header.set_method_name(method->name());
        header.set_arg_size(args.size());
        std::string header_str;
        if(!header.SerializeToString(&header_str)) {
            controller->SetFailed("Serialize rpc header failed!");
            return;
        }
        //构造长度
        int header_size = header_str.size();
        std::string query((char*)&header_size, 4);
        //数据
        query += header_str;
        query += args;
        //2、获取rpc服务器配置
        MprpcApplication* app = MprpcApplication::getInstance();
        MprpcConfig* config = app->getConfig();
        std::string ip = config->query("rpcserverip");
        std::string port_str = config->query("rpcserverport");
        uint16_t port = atoi(port_str.c_str());
        //3、TCP编程连接rpc服务器
        int clientfd = socket(AF_INET, SOCK_STREAM, 0);
        if(clientfd == -1) {
            controller->SetFailed("create client fd failed!");
            return;
        }
        sockaddr_in addr;
        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        int ret = connect(clientfd, (sockaddr*) &addr, sizeof(addr));
        if(ret == -1) {
            controller->SetFailed("can not connect rpc server!");
            close(clientfd);
            return;
        }
        //4、将构造的数据发送给服务器
        ret = send(clientfd, query.c_str(), query.size(), 0);
        if(ret == -1) {
            controller->SetFailed("send to rpc server failed!");
            close(clientfd);
            return;
        }
        //5、阻塞等待服务器的响应数据
        char buf[1024] = {0};
        ret = recv(clientfd, buf, sizeof buf, 0);
        if(ret == -1) {
            controller->SetFailed("recv from rpc server failed!");
            close(clientfd);
            return;
        }
        //反序列化消息
        if(!response->ParseFromString(std::string(buf, ret))) {
            controller->SetFailed("response parse from string failed!");
            close(clientfd);
            return;
        }
        close(clientfd);
}