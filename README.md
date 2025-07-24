
# CC2S - CLI Connect to SSH

**CC2S（CLI Connect to SSH）** 是一个轻量级的 Windows 平台命令行工具，用于快速、可配置地连接多个 SSH 服务器。适合系统管理员、开发者和有多个远程服务器管理需求的用户。


## 特性

- 🔒 支持私钥或密码认证
- 📁 配置保存在 `%APPDATA%\CC2S\servers.cc2s`
- 🧠 自动检测并转换配置文件编码为 UTF-8
- 🎛️ 支持添加、删除、管理服务器信息
- 🔢 支持自定义端口号
- 💾 自定义配置格式，二进制友好，不依赖 JSON 库
- 🪄 启动时清屏、连接时清屏，体验丝滑

---

## 编译方式

手动使用 CMake 与任意支持的 Windows 编译器：

```bash
git clone https://github.com/yourusername/cc2s.git
cd cc2s
cmake -B build
cmake --build build
````

---

## 使用方法

启动程序后，你将看到如下界面：

```bash
=== CC2S - CLI Connect to SSH ===

[0] 删除或者新增服务器
[q] 退出

输入选项:
```

### 新增服务器

* 输入 `0` → `1`，输入服务器信息
* 可填写私钥路径和自定义端口（默认 22）

### 删除服务器

* 输入 `0` → `2`，根据编号删除

### 连接服务器

* 直接输入编号，即可发起连接（如 `1`）

---

## 配置格式（`servers.cc2s`）

每行一台服务器，字段用 ASCII 分隔符 `\x1E` 分割：

```text
服务器名称␞用户名␞主机␞私钥路径␞端口号
```

---

## 项目结构

```
cc2s/
├── config.hpp         // 配置和Server结构体定义（已融合）
├── config.cpp         // 配置读写与编码转换
├── main.cpp           // 主程序入口
├── CMakeLists.txt     // 构建配置
└── README.md
```
