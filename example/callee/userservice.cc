#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.hpp"
#include "rpcprovider.hpp"

//UserService原来是一个本地服务，提供进程内的本地方法
class UserService: public fixbug::UserServiceRpc {//使用在rpc服务提供端
public:
    //原本地方法
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }
    //重写UserServiceRpc的Login虚函数, 由rpc框架调用
    virtual void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done) override {
        //业务代码：
        //1、获取函数调用的参数
        std::string name = request->name();
        std::string pwd = request->pwd();
        //2、调用本地业务
        bool login_result = Login(name, pwd);
        //3、填写返回值
        fixbug::ResultCode* rc = response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("");
        response->set_success(login_result);
        //4、调用框架提供的done方法，将结果发送回去
        done->Run();
    }
};

int main(int arc, char** argv) {
    //UserService us;
    //us.Login("ycjinyi", "12345");
    //调用框架的初始化操作
    MprpcApplication::init(arc, argv);
    //provider 是一个rpc网络服务对象，用于发布服务
    RpcProvider provider;
    provider.notifyService(new UserService());
    //启动一个rpc服务发布节点, 启动后进入阻塞状态，
    //等待远程调用rpc方法
    provider.run();
    return 0;
}