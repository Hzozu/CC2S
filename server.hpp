// 已经和 config.hpp 进行了融合，如果想要不融合，请将 config.hpp 改为如下：
/*
#pragma once
#include "server.hpp"
#include <vector>
#include <string>

namespace Config {
    std::string getConfigPath();
    void load(std::vector<Server>& servers);
    void save(const std::vector<Server>& servers);

    bool isUtf8(const std::string& path);
    void convertToUtf8(const std::string& path);
}

*/

#pragma once
#include <string>

struct Server {
    std::string name;
    std::string user;
    std::string host;
    std::string keyPath;
    int port = 22; // 默认端口
};
