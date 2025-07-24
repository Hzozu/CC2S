#include "config.hpp"
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>


constexpr char SEP = '\x1E';

std::string Config::ConfigPath() {
    const char* appData = std::getenv("APPDATA");
    if (!appData) {
        std::cerr << "[CC2S] 无法获取 APPDATA 环境变量\n";
        return "servers.cc2s";
    }
    return std::string(appData) + "\\CC2S\\servers.cc2s";
}

void Config::load(std::vector<Server>& servers) {
    std::ifstream file(ConfigPath());

    // file.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>)); // 已被弃用的方案

    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        size_t pos1 = line.find(SEP);
        size_t pos2 = line.find(SEP, pos1 + 1);
        size_t pos3 = line.find(SEP, pos2 + 1);
        size_t pos4 = line.find(SEP, pos3 + 1);

        if (pos1 == std::string::npos || pos2 == std::string::npos ||
            pos3 == std::string::npos || pos4 == std::string::npos)
            continue;

        Server s;
        s.name    = line.substr(0, pos1);
        s.user    = line.substr(pos1 + 1, pos2 - pos1 - 1);
        s.host    = line.substr(pos2 + 1, pos3 - pos2 - 1);
        s.keyPath = line.substr(pos3 + 1, pos4 - pos3 - 1);
        s.port    = std::stoi(line.substr(pos4 + 1));

        servers.push_back(s);
    }
}

void Config::save(const std::vector<Server>& servers) {
    std::string path = ConfigPath();
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    for (const auto& s : servers) {
        file << s.name    << SEP
             << s.user    << SEP
             << s.host    << SEP
             << s.keyPath << SEP
             << s.port    << "\n";
    }
}

bool Config::utf8(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return true;  // 文件不存在默认跳过

    unsigned char c;
    int expected = 0;
    while (file.read(reinterpret_cast<char*>(&c), 1)) {
        if (expected == 0) {
            if ((c & 0x80) == 0x00) continue;
            else if ((c & 0xE0) == 0xC0) expected = 1;
            else if ((c & 0xF0) == 0xE0) expected = 2;
            else if ((c & 0xF8) == 0xF0) expected = 3;
            else return false;
        } else {
            if ((c & 0xC0) != 0x80) return false;
            expected--;
        }
    }

    return expected == 0;
}

void Config::convert(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return;

    std::ostringstream oss;
    std::string line;
    while (std::getline(file, line)) {
        oss << line << "\n";
    }
    file.close();

    std::string content = oss.str();

    // 转换编码
    int wideLen = MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, nullptr, 0);
    std::wstring wideStr(wideLen, 0);
    MultiByteToWideChar(CP_ACP, 0, content.c_str(), -1, &wideStr[0], wideLen);

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Str(utf8Len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Len, nullptr, nullptr);

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(utf8Str.c_str(), utf8Str.size());
    out.close();
}