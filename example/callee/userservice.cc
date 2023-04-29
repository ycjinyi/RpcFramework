#include <iostream>
#include <string>
#include "user.pb.h"

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
                        
    }
};

int main() {
    //UserService us;
    //us.Login("ycjinyi", "12345");
    return 0;
}