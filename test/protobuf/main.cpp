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

    fixbug::test ts;
    //类成员变量的访问需要先接收指针，再通过指针去修改
    fixbug::LoginResponse* ptr = ts.mutable_rsp();
    ptr->set_errcode(0);
    ptr->set_errmsg("200 ok");
    ptr->set_success(true);

    //User数组
    //添加操作，返回指向新添加的成员的指针，通过指针设置值
    fixbug::User* user = ts.add_friendlist();
    user->set_age(24);
    user->set_name("ycjinyi");
    user->set_sex(fixbug::User::Man);
    //计算数组长度
    std::cout << ts.friendlist_size() << std::endl;
    //获取数组元素
    fixbug::User* temp = ts.mutable_friendlist(0);
    std::cout << temp->age() << std::endl;
    return 0;
}