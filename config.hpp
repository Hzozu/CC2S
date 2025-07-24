#pragma once
#include <string>
#include <vector>

struct Server {
    std::string name;
    std::string user;
    std::string host;
    std::string keyPath;
    int port;
};

class Config {
public:
    static std::string ConfigPath();
    static void load(std::vector<Server>& servers);
    static void save(const std::vector<Server>& servers);

    static bool utf8(const std::string& path);
    static void convert(const std::string& path);
};
