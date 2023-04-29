#include <iostream>
#include <string>
#include "test.pb.h"
using namespace fixbug;

int main() {
    LoginRequest req;
    req.set_name("ycjinyi");
    req.set_pwd("123456");

    //序列化
    std::string opt;
    if(req.SerializePartialToString(&opt)) {
        std:: cout << opt << std:: endl;
    }
    //反序列化
    LoginRequest rec;
    if(rec.ParseFromString(opt)) {
        std::cout << rec.name() << std::endl;
        std::cout << rec.pwd() << std::endl;
    }
    
    return 0;
}