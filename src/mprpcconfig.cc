#include "mprpcconfig.hpp"
#include <iostream>
#include <fstream>
#include <string>

void MprpcConfig::loadConfig(const std::string& path) {
    std::ifstream file;
    std::string line;
    file.open(path, std::ios::in);
    if(!file.is_open()) {
        std::cout << "can't open " << path << std::endl;
    }
    while(getline(file, line)) {
        std::string show = line;
        trim(line);
        if(!line.size() || line[0] == '#') continue;
        //获取键值对
        int idx = line.find('=');
        if(idx == -1) continue;
        std::string key(line.substr(0, idx));
        trim(key);
        std::string value(line.substr(idx + 1, line.size() - idx - 1));
        trim(value);
        _configMap.insert({key, value});
    }
}

std::string MprpcConfig::query(const std::string& key) {
    auto it = _configMap.find(key);
    if(it == _configMap.end()) return "";
    return it->second;
}

void MprpcConfig::trim(std::string& str) {
    //去掉开头的空格
    int idx = str.find_first_not_of(' ');
    if(idx == -1) return; 
    str = str.substr(idx, str.size() - idx);
    //去掉结尾的空格
    idx = str.find_last_not_of(' ');
    if(idx == -1) return;
    str = str.substr(0, idx + 1);
}
