#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class LockQueue {
public:
    void push(const T& elem);
    T pop();
    inline int size() const { return _que.size();}
private:
    std::queue<T> _que;
    std::mutex _mutex;
    std::condition_variable _condVar;
};


template<typename T>
void LockQueue<T>::push(const T& elem) {
    {
        std::lock_guard<std::mutex> lg(_mutex);
        _que.push(elem);
    }
    _condVar.notify_one();
}

template<typename T> 
T LockQueue<T>::pop() {
    //unique_lock初始化的时候会加锁，析构时解锁
    std::unique_lock<std::mutex> lk(_mutex);
    while(_que.empty()) {
        _condVar.wait(lk);
    }
    T front = _que.front();
    _que.pop();
    return front;
}