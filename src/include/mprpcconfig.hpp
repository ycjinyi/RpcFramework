#pragma once
#include <unordered_map>
#include <string>
class MprpcConfig{
public:
    void loadConfig(const std::string& path);
    std::string query(const std::string& key);
private:
    std::unordered_map<std::string, std::string> _configMap;
private:
    void trim(std::string& str);//去掉字符串的首尾空格
};