#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <cstdlib>
#include "config.hpp"
#include <limits>


void menu(const std::vector<Server>& servers) {
    std::cout << "=== CC2S - CLI Connect to SSH ===\n";
    std::cout << "  * 目前 CC2S 只支持 Linux 的连接 *\n\n";
    for (size_t i = 0; i < servers.size(); ++i) {
        const auto& s = servers[i];
        std::cout << "[" << i + 1 << "] " << s.name << " (" << s.user << "@" << s.host << ")\n";
    }
    std::cout << "[0] 删除或者新增服务器"
                 "\n";
    std::cout << "[q] 退出\n\n";
    std::cout << "输入选项: ";
}

void connect(const Server& s) {
    std::string cmd = "ssh -tt -p " + std::to_string(s.port) + " ";
    if (!s.keyPath.empty()) {
        cmd += "-i \"" + s.keyPath + "\" ";
    }
    cmd += s.user + "@" + s.host;
    cmd += R"( "echo __CC2S_CONNECTED__; exec bash")";

    std::cout << ">> 正在连接 " << s.user << "@" << s.host << ":" << s.port << "...\n";
    std::cout.flush();

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "无法启动 SSH\n";
        return;
    }

    bool connected = false;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        std::cout << line;
        if (!connected && line.find("__CC2S_CONNECTED__") != std::string::npos) {
            connected = true;
            std::cout << "\033[2J\033[H";
        }
    }
    _pclose(pipe);

    std::cout << "\n\n[CC2S] 连接已结束，按回车返回主菜单...";
    std::cin.sync();
    std::cin.get();
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

void add(std::vector<Server>& servers) {
    Server s;

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

    std::cout << "服务器名称: ";
    std::getline(std::cin, s.name);
    std::cout << "用户名: ";
    std::getline(std::cin, s.user);
    std::cout << "主机地址: ";
    std::getline(std::cin, s.host);
    std::cout << "私钥路径（可空）: ";
    std::getline(std::cin, s.keyPath);
    std::cout << "端口号（默认 22）: ";
    std::string portStr;
    std::getline(std::cin, portStr);
    if (!portStr.empty()) s.port = std::stoi(portStr);

    servers.push_back(s);
    Config::save(servers);
    std::cout << "已保存服务器: " << s.name << "\n";
}

void del(std::vector<Server>& servers) {

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

    if (servers.empty()) {
        std::cout << "没有可删除的服务器\n";
        return;
    }

    std::cout << "要删除哪个服务器？（编号）\n";
    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << servers[i].name << "\n";
    }
    std::cout << "[b] 返回\n> ";

    std::string input;
    std::getline(std::cin, input);
    if (input == "b") return;

    try {
        int idx = std::stoi(input);
        if (idx >= 1 && idx <= (int)servers.size()) {
            std::cout << "确定删除 " << servers[idx - 1].name << "？(y/n): ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm == "y") {
                servers.erase(servers.begin() + idx - 1);
                Config::save(servers);
                std::cout << "已删除\n";
            }
        } else {
            std::cout << "无效编号\n";
            system("cls");
        }
    } catch (...) {
        std::cout << "输入无效\n";
        system("cls");
    }
}

void manage(std::vector<Server>& servers) {
    while (true) {

        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

        std::cout << "\n=== 管理服务器 ===\n";
        std::cout << "[1] 新增服务器\n[2] 删除服务器\n[b] 返回\n> ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "1") {
            add(servers);
        } else if (input == "2") {
            del(servers);
        } else if (input == "b") {
            break;
        } else {
            std::cout << "无效输入\n";
            system("cls");
        }
    }
}

int main() {

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios::sync_with_stdio(false);

    std::string path = Config::ConfigPath();
    if (!Config::utf8(path)) {
        std::cout << "[CC2S] 配置文件不是 UTF-8，正在自动转换...\n";
        Config::convert(path);
    }

    std::vector<Server> servers;
    Config::load(servers);

    while (true) {
        menu(servers);
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") break;
        if (input == "0") {
            manage(servers);
            continue;
        }

        try {
            int index = std::stoi(input);
            if (index >= 1 && index <= (int)servers.size()) {
                connect(servers[index - 1]);
            } else {
                std::cout << "无效编号\n";
                system("cls");
            }
        } catch (...) {
            std::cout << "输入无效\n";
            system("cls");
        }

        std::cout << "\n";
    }

    return 0;
}