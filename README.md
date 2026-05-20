# T469 车载交叉编译助手

AI 辅助的嵌入式 Linux 交叉编译工具，自动诊断和修复跨平台开发中的常见痛点。提供命令行脚本和图形界面两种使用方式。

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)]()
[![Qt](https://img.shields.io/badge/Qt-5.12%20%7C%205.15-green.svg)]()

---

## 下载安装

### 方式 1: 下载预编译版本（推荐）

从 [GitHub Releases](https://github.com/Han-cy830/T469-car-agent/releases) 页面下载对应平台的安装包：

| 平台 | 文件 | 说明 |
|------|------|------|
| **Windows** | `t469-car-agent-windows-x86_64.zip` | 解压即用，包含 Qt 运行时 |
| **Linux** | `t469-car-agent-linux-x86_64.tar.gz` | 解压后运行可执行文件 |

> **首次发布前**：需要先在 GitHub 创建一个 Release，触发自动构建。详见下方 [创建发布版本](#创建发布版本)。

#### Windows 安装步骤

```
1. 从 Releases 页面下载 t469-car-agent-windows-x86_64.zip
2. 解压到任意目录（如 D:\T469-car-agent\）
3. 双击 t469-car-agent.exe 启动
```

#### Linux 安装步骤

```bash
# 下载
wget https://github.com/Han-cy830/T469-car-agent/releases/latest/download/t469-car-agent-linux-x86_64.tar.gz

# 解压
tar -xzf t469-car-agent-linux-x86_64.tar.gz
cd t469-car-agent-linux-x86_64

# 添加执行权限
chmod +x t469-car-agent

# 运行
./t469-car-agent
```

### 方式 2: Git Clone

```bash
git clone https://github.com/Han-cy830/T469-car-agent.git
cd T469-car-agent
```

### 方式 3: 下载 ZIP 包

```bash
wget https://github.com/Han-cy830/T469-car-agent/archive/refs/heads/main.zip
unzip main.zip
cd T469-car-agent-main
```

### 方式 4: GitHub CLI

```bash
gh repo clone Han-cy830/T469-car-agent
```

### 方式 5: 只下载命令行脚本（轻量）

```bash
curl -O https://raw.githubusercontent.com/Han-cy830/T469-car-agent/main/diagnose.sh
curl -O https://raw.githubusercontent.com/Han-cy830/T469-car-agent/main/fix.sh
curl -O https://raw.githubusercontent.com/Han-cy830/T469-car-agent/main/build.sh
chmod +x diagnose.sh fix.sh build.sh
```

---

## 快速开始

### 命令行方式

```bash
# 1. 运行环境诊断
./diagnose.sh

# 2. 自动修复发现的问题
./fix.sh

# 3. 编译（可选目标：all / qt / alsa / car / deploy）
./build.sh all
```

### 图形界面方式

```bash
# Linux: 直接运行
./t469-car-agent

# Windows: 双击 t469-car-agent.exe
```

启动后左侧导航栏有 6 个功能页：

| 页面 | 功能 |
|------|------|
| 概览 | 环境状态卡片，快捷操作入口 |
| 诊断 | 一键扫描编译环境，表格展示结果 |
| 修复 | 7 项自动修复，支持单项或全部 |
| 编译 | 选择编译目标，实时日志输出 |
| 部署 | SSH 配置，一键传输到 ARM 设备 |
| 设置 | 工具链路径、SSH 参数等配置 |

---

## 从源码构建 GUI

如果你想自己编译 GUI 应用：

### 依赖

- Qt 5.12+ (含 QtWidgets 模块)
- qmake
- C++11 编译器 (GCC / MSVC / MinGW)

### Linux 构建

```bash
# 安装 Qt 开发包
sudo apt install qt5-default qtbase5-dev

# 构建
cd gui
qmake gui.pro
make -j$(nproc)

# 运行
./t469-car-agent
```

### Windows 构建 (MSVC)

```cmd
:: 打开 Qt 命令提示符（开始菜单 → Qt → Qt Command Prompt）
cd gui
qmake gui.pro
nmake

:: 运行
t469-car-agent.exe
```

### Windows 构建 (MinGW)

```cmd
cd gui
qmake gui.pro
mingw32-make

t469-car-agent.exe
```

---

## 创建发布版本

仓库维护者可以通过 Git Tag 触发 GitHub Actions 自动构建并发布：

```bash
# 创建标签
git tag v1.0.0

# 推送标签，自动触发构建和发布
git push origin v1.0.0
```

推送后 GitHub Actions 会自动：
1. 在 Linux 和 Windows 上分别编译 GUI
2. 打包为 `t469-car-agent-linux-x86_64.tar.gz` 和 `t469-car-agent-windows-x86_64.zip`
3. 创建 GitHub Release 并附带下载文件

---

## 详细教程

### 教程 1: 从零开始的 ARM 交叉编译

#### 1.1 安装工具链

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi \
    build-essential pkg-config dos2unix unzip rsync curl git
```

**CentOS/RHEL:**
```bash
sudo yum install gcc-arm-linux-gnu-gcc gcc-arm-linux-gnu-gcc-c++ \
    make pkgconfig dos2unix unzip rsync curl git
```

**Windows (WSL2):**
```powershell
wsl --install -d Ubuntu
# 重启后在 WSL 中执行 Ubuntu 的安装命令
```

#### 1.2 下载项目

```bash
# 方式 A: 下载预编译 GUI（推荐）
# 从 Releases 页面下载解压即可

# 方式 B: 克隆仓库
git clone https://github.com/Han-cy830/T469-car-agent.git
cd T469-car-agent
chmod +x diagnose.sh fix.sh build.sh
```

#### 1.3 运行诊断

```bash
./diagnose.sh

# 输出示例：
# === 1. 交叉编译工具链 ===
#   [✓] CC: arm-linux-gnueabi-gcc (Ubuntu 9.4.0) 9.4.0
#   [✓] CXX: arm-linux-gnueabi-g++
#
# === 2. 换行符检测 (CRLF → LF) ===
#   [✗] 检测到 15 个文件含有 CRLF 换行符
#
# === 3. 符号链接 / 递归目录检测 ===
#   [✓] 未检测到递归目录
```

#### 1.4 修复问题

```bash
./fix.sh

# 输出示例：
# [FIX] 修复 15 个文件的换行符...
# [DONE] 换行符修复完成
```

#### 1.5 设置环境变量

```bash
# fix.sh 会自动生成 env_setup.sh
source env_setup.sh
```

#### 1.6 编译

```bash
# 使用 Agent 自动编译完整依赖链
./build.sh all

# 或手动编译单个组件
./build.sh qt       # 只编译 Qt
./build.sh alsa     # 只编译 ALSA
./build.sh car      # 只编译车载应用
```

#### 1.7 验证产物

```bash
file deploy/bin/car
# 期望: ELF 32-bit LSB executable, ARM, EABI5
```

---

### 教程 2: Windows 源码传输到 Linux

#### 2.1 打包源码

```powershell
# Git Bash
tar -czf myproject.tar.gz myproject/

# PowerShell
Compress-Archive -Path myproject -DestinationPath myproject.zip

# 7-Zip
7z a myproject.zip myproject/
```

#### 2.2 传输到 Linux

```bash
# scp
scp myproject.tar.gz user@linux-host:~/

# rsync
rsync -avz myproject/ user@linux-host:~/myproject/

# xftp/sftp 图形工具
```

#### 2.3 解压并修复

```bash
tar -xzf myproject.tar.gz

# 下载修复脚本
curl -O https://raw.githubusercontent.com/Han-cy830/T469-car-agent/main/fix.sh
chmod +x fix.sh
./fix.sh
```

---

### 教程 3: Qt 交叉编译

```bash
# 下载 Qt 源码
wget https://download.qt.io/archive/qt/5.12/5.12.12/single/qt-everywhere-opensource-src-5.12.12.tar.xz
tar -xf qt-everywhere-opensource-src-5.12.12.tar.xz

# 修复换行符（如从 Windows 传输）
cd qt-everywhere-src-5.12.12
find . -type f \( -name "*.sh" -o -name "*.pro" -o -name "*.pri" -o -name "Makefile*" \) \
    -exec dos2unix {} +

# 配置
mkdir ../build-qt && cd ../build-qt
../qt-everywhere-src-5.12.12/configure \
    -release -opensource -confirm-license \
    -xplatform linux-arm-gnueabi-g++ \
    -prefix /opt/qt5-arm \
    -no-opengl -no-vulkan -no-openssl -no-xcb \
    -nomake examples -nomake tests

# 编译安装
make -j$(nproc)
sudo make install
```

---

### 教程 4: 部署到 ARM 设备

```bash
# 创建部署目录
mkdir -p deploy/bin deploy/lib

# 复制可执行文件和库
cp car deploy/bin/
cp /opt/qt5-arm/lib/libQt5*.so* deploy/lib/
cp compiled/alsa-lib-1.2.2/build/lib/libasound.so* deploy/lib/

# 创建启动脚本
cat > deploy/run.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
export QT_QPA_PLATFORM=linuxfb
"$SCRIPT_DIR/bin/car"
EOF
chmod +x deploy/run.sh

# 传输到设备
scp -r deploy/ user@arm-device:~/

# 在设备上运行
ssh user@arm-device
cd deploy
./run.sh
```

---

## 解决的痛点

| 痛点 | 症状 | 自动修复 |
|------|------|----------|
| CRLF 换行符 | `#!/bin/sh^M: 解释器错误` | `dos2unix` 批量修复 |
| 递归嵌套目录 | `alsa/alsa/alsa/...` 无限循环 | 检测并删除循环目录 |
| 路径过长 | xftp/scp 传输失败 | 自动缩短目录名 |
| pkg-config 路径 | 库找不到 | 重写 .pc 文件路径 |
| 环境变量遗漏 | 隐蔽的编译错误 | 自动生成 env_setup.sh |
| Qt OpenGL 报错 | `QOpenGLTexture does not name a type` | 自动加 `-no-opengl -no-vulkan` |
| 编译产物架构错误 | 编出 x86 而非 ARM | `file` + `readelf` 自动验证 |
| 运行时库缺失 | `libQt5Core.so.5 not found` | `ldd` 检查并收集依赖 |

---

## 项目结构

```
T469-car-agent/
├── README.md              # 本文档
├── CLAUDE.md              # Claude Code 项目上下文
├── PROMPT.md              # 提示词模板
├── diagnose.sh            # 环境诊断脚本（10 项检测）
├── fix.sh                 # 自动修复脚本（7 项修复）
├── build.sh               # 自动化构建脚本
├── pain-points.md         # 痛点分析和解决方案
├── gui/                   # 图形界面应用
│   ├── gui.pro            # Qt 项目文件
│   ├── main.cpp           # 入口 + 暗色主题
│   ├── core/              # 核心逻辑
│   │   ├── ScriptEngine   # 脚本引擎
│   │   ├── OutputParser   # 输出解析
│   │   ├── SettingsManager# 配置管理
│   │   ├── SshConnector   # SSH 连接
│   │   └── BuildDependency# 依赖排序
│   ├── models/            # 数据模型
│   ├── widgets/           # UI 组件（6 个 Tab 页）
│   └── utils/             # 工具类
├── examples/              # 示例项目
│   ├── hello-arm/         # ARM Hello World
│   ├── qt-app/            # Qt 应用示例
│   └── alsa-player/       # ALSA 音频播放器
├── .github/workflows/     # CI/CD 自动构建
└── LICENSE                # MIT 许可证
```

---

## 常见问题

**Q: 权限不足？**
```bash
chmod +x diagnose.sh fix.sh build.sh
```

**Q: 没有 dos2unix？**
```bash
sudo apt install dos2unix        # Ubuntu
sudo yum install dos2unix        # CentOS
brew install dos2unix            # macOS
# 或手动: sed -i 's/\r$//' filename
```

**Q: 交叉编译器找不到？**
```bash
which arm-linux-gnueabi-gcc
sudo apt install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
```

**Q: 编译出 x86 而非 ARM？**
```bash
echo $CC  # 应输出 arm-linux-gnueabi-gcc
source env_setup.sh
```

**Q: Qt configure 报错 OpenGL？**
```bash
./configure -no-opengl -no-vulkan ...
```

**Q: 设备运行时报库缺失？**
```bash
LD_LIBRARY_PATH=deploy/lib ldd deploy/bin/car | grep "not found"
# 复制缺失的库到 deploy/lib/
```

**Q: GUI 无法启动（Linux）？**
```bash
# 安装 Qt 运行时
sudo apt install libqt5widgets5

# 或使用 X11 转发
export QT_QPA_PLATFORM=xcb
```

**Q: GUI 无法启动（Windows）？**
```
确保解压了所有 DLL 文件和 platforms/ 目录
t469-car-agent.exe 必须和 Qt*.dll 在同一目录
```

---

## 贡献

1. Fork 本仓库
2. 创建特性分支: `git checkout -b feature/amazing-feature`
3. 提交更改: `git commit -m 'Add amazing feature'`
4. 推送分支: `git push origin feature/amazing-feature`
5. 创建 Pull Request

贡献方向：新的痛点场景、更多目标平台 (RISC-V, MIPS, AArch64)、诊断脚本改进、教程完善。

---

## 许可证

MIT License - 详见 [LICENSE](LICENSE)

---

## 致谢

- [Qt](https://www.qt.io/) - 跨平台 GUI 框架
- [ALSA](https://www.alsa-project.org/) - 音频系统
- [Claude Code](https://claude.ai/code) - AI 辅助开发
