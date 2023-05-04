#include "zookeeperutil.hpp"
#include "mprpcapplication.hpp"
#include <iostream>
#include <semaphore.h>

static void watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx) {//当连接完成后，会执行该回调函数
    if(type == ZOO_SESSION_EVENT) { //事件类型
        if(state == ZOO_CONNECTED_STATE) { //状态类型 
            sem_t* _sem = (sem_t*)zoo_get_context(zh);
            //唤醒阻塞在_sem上的start()函数
            sem_post(_sem);
        }
    }
}

Zookeeper::Zookeeper(): _zhandle(nullptr) {}

Zookeeper::~Zookeeper() {
    if(_zhandle) zookeeper_close(_zhandle);
}

//zookeeper的API客户端程序创建了3个线程
//1、API调用线程
//2、网络IO线程 pthread_create
//3、watcher回调线程 pthread_create
bool Zookeeper::start() {//连接zookeeper server
    //首先获取zookeeper server的ip和端口号
    std::string zkip = MprpcApplication::getInstance()->getConfig()->query("zookeeperip");
    std::string zkport = MprpcApplication::getInstance()->getConfig()->query("zookeeperport");
    //拼接为host
    std::string host = zkip + ":" + zkport;
    //异步初始化连接
    _zhandle = zookeeper_init(host.c_str(), watcher, 30000, nullptr, nullptr, 0);
    if(_zhandle == nullptr) {
        std::cout << "zookeeper init failed!" << std::endl;
        return false;
    }
    //初始化信号量
    sem_t _sem;
    sem_init(&_sem, 0, 0);
    zoo_set_context(_zhandle, &_sem);
    sem_wait(&_sem);
    std::cout << "zookeeper init succeed!" << std::endl;
    return true;
}
bool Zookeeper::create(const std::string& path, const std::string& data, int state) {
    return create(path.c_str(), data.c_str(), data.size(), state);
}

bool Zookeeper::create(const char* path, const char* data, int dataLen, int state) {
    if(ZNONODE != zoo_exists(_zhandle, path, 0, nullptr)) { //路径节点已存在
        std::cout << path << " is already exists!" << std::endl;
        return true;
    }
    char buf[128] = {0};
    if(ZOK == zoo_create(_zhandle, path, data, dataLen, &ZOO_OPEN_ACL_UNSAFE, 
        state, buf, sizeof buf)) { //创建节点
        std::cout << "create " << path << " succeed!" << std::endl;
        return true;
    } 
    std::cout << "create " << path << " failed!" << std::endl;
    return false;
}

std::string Zookeeper::getData(const std::string& path) {
    return getData(path.c_str());
}

std::string Zookeeper::getData(const char* path) {
    char buf[512] = {0};
    int buf_len = sizeof buf;
    if(ZOK == zoo_get(_zhandle, path, 0, buf, &buf_len, nullptr)) {
        return buf;
    }
    std::cout << "get " << path << " data failed!" << std::endl;
    return "";
}