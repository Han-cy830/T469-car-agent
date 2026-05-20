#!/bin/bash
# T469 车载交叉编译助手 - 一键安装脚本
# 用法: curl -fsSL https://raw.githubusercontent.com/Han-cy830/T469-car-agent/main/install.sh | bash

set -e

REPO="Han-cy830/T469-car-agent"
VERSION="latest"
INSTALL_DIR="$HOME/T469-car-agent"

# 颜色
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

info()  { echo -e "${BLUE}[i]${NC} $1"; }
ok()    { echo -e "${GREEN}[✓]${NC} $1"; }
warn()  { echo -e "${YELLOW}[!]${NC} $1"; }
fail()  { echo -e "${RED}[✗]${NC} $1"; exit 1; }

echo ""
echo "=========================================="
echo "  T469 车载交叉编译助手 - 安装程序"
echo "=========================================="
echo ""

# 检测平台
OS="$(uname -s)"
ARCH="$(uname -m)"
info "检测平台: $OS $ARCH"

case "$OS" in
    Linux)
        PLATFORM="linux"
        ARCHIVE="t469-car-agent-linux-x86_64.tar.gz"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        PLATFORM="windows"
        ARCHIVE="t469-car-agent-windows-x86_64.zip"
        warn "Windows 用户建议直接下载 ZIP 包解压使用"
        fail "请从 GitHub Releases 页面手动下载: https://github.com/$REPO/releases"
        ;;
    Darwin)
        PLATFORM="macos"
        warn "macOS 不提供预编译版本，将从源码构建"
        ;;
    *)
        fail "不支持的操作系统: $OS"
        ;;
esac

# 检查依赖
check_dep() {
    if command -v "$1" &>/dev/null; then
        ok "$1 已安装"
    else
        warn "$1 未安装"
        return 1
    fi
}

info "检查依赖..."
DEPS_OK=true
check_dep curl || DEPS_OK=false
check_dep tar || DEPS_OK=false

if [ "$DEPS_OK" = false ]; then
    fail "缺少必要依赖，请先安装 curl 和 tar"
fi

# 选择安装方式
echo ""
info "选择安装方式:"
echo "  1) 下载预编译版本（推荐）"
echo "  2) 从源码构建（需要 Qt 开发环境）"
echo ""
read -p "请输入选项 [1]: " CHOICE
CHOICE=${CHOICE:-1}

if [ "$CHOICE" = "1" ]; then
    # 下载预编译版本
    echo ""
    info "下载 $ARCHIVE ..."

    # 获取最新 Release URL
    DOWNLOAD_URL="https://github.com/$REPO/releases/latest/download/$ARCHIVE"

    if command -v wget &>/dev/null; then
        wget -q --show-progress -O "/tmp/$ARCHIVE" "$DOWNLOAD_URL" 2>&1 || \
        wget -O "/tmp/$ARCHIVE" "$DOWNLOAD_URL"
    else
        curl -L -o "/tmp/$ARCHIVE" "$DOWNLOAD_URL" --progress-bar
    fi

    if [ ! -f "/tmp/$ARCHIVE" ]; then
        fail "下载失败。请手动从 https://github.com/$REPO/releases 下载"
    fi

    ok "下载完成"

    # 解压
    info "解压到 $INSTALL_DIR ..."
    mkdir -p "$INSTALL_DIR"
    tar -xzf "/tmp/$ARCHIVE" -C "$INSTALL_DIR" --strip-components=1 2>/dev/null || \
    tar -xzf "/tmp/$ARCHIVE" -C "$INSTALL_DIR"
    rm -f "/tmp/$ARCHIVE"

    ok "解压完成"

    # 设置权限
    chmod +x "$INSTALL_DIR/t469-car-agent" 2>/dev/null || true
    chmod +x "$INSTALL_DIR/"*.sh 2>/dev/null || true

elif [ "$CHOICE" = "2" ]; then
    # 从源码构建
    echo ""

    # 检查 Qt
    if ! command -v qmake &>/dev/null; then
        info "安装 Qt 开发环境..."
        if command -v apt &>/dev/null; then
            sudo apt update
            sudo apt install -y qt5-default qtbase5-dev build-essential
        elif command -v yum &>/dev/null; then
            sudo yum install -y qt5-qtbase-devel gcc-c++ make
        elif command -v brew &>/dev/null; then
            brew install qt5
        else
            fail "无法自动安装 Qt，请手动安装 Qt 5.12+"
        fi
    fi
    ok "Qt 已就绪"

    # 克隆仓库
    info "克隆仓库..."
    if [ -d "$INSTALL_DIR" ]; then
        warn "目录已存在，使用 git pull 更新"
        cd "$INSTALL_DIR"
        git pull
    else
        git clone "https://github.com/$REPO.git" "$INSTALL_DIR"
        cd "$INSTALL_DIR"
    fi

    # 构建
    info "构建 GUI..."
    cd gui
    qmake gui.pro
    make -j$(nproc)
    cd ..

    ok "构建完成"
    chmod +x gui/t469-car-agent 2>/dev/null || true
    chmod +x diagnose.sh fix.sh build.sh 2>/dev/null || true

else
    fail "无效选项"
fi

# 完成
echo ""
echo "=========================================="
ok "安装完成！"
echo "=========================================="
echo ""
info "安装位置: $INSTALL_DIR"
echo ""
info "启动方式:"
echo "  图形界面:  $INSTALL_DIR/t469-car-agent"
echo "  命令行:    $INSTALL_DIR/diagnose.sh"
echo ""
info "快速开始:"
echo "  cd $INSTALL_DIR"
echo "  ./diagnose.sh        # 环境诊断"
echo "  ./fix.sh             # 自动修复"
echo "  ./build.sh all       # 编译全部"
echo "  ./t469-car-agent     # 启动 GUI"
echo ""
info "GitHub: https://github.com/$REPO"
echo ""
