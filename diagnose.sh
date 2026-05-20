#!/bin/bash
# Embedded Cross-Compile Agent - Environment Diagnostic Script
# Automatically detects common pain points in cross-compilation projects

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

pass() { echo -e "  ${GREEN}[✓]${NC} $1"; }
fail() { echo -e "  ${RED}[✗]${NC} $1"; }
warn() { echo -e "  ${YELLOW}[!]${NC} $1"; }
info() { echo -e "  ${BLUE}[i]${NC} $1"; }
header() { echo -e "\n${BLUE}=== $1 ===${NC}"; }

ISSUES_FOUND=0

# ============================================================
# 1. Toolchain Detection
# ============================================================
header "1. 交叉编译工具链"

detect_toolchain() {
    local prefix="$1"
    local cc="${prefix}-gcc"
    local cxx="${prefix}-g++"

    if command -v "$cc" &>/dev/null; then
        local version=$("$cc" --version 2>/dev/null | head -1)
        pass "CC: $version"
    else
        fail "CC: $cc 未找到"
        echo "       修复: sudo apt install gcc-${prefix} g++-${prefix}"
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    fi

    if command -v "$cxx" &>/dev/null; then
        pass "CXX: $cxx"
    else
        fail "CXX: $cxx 未找到"
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    fi
}

# Auto-detect target architecture from project
if [ -f "build.sh" ]; then
    TOOLCHAIN_PREFIX=$(grep -oP '(?<=export CC=)[^ ]+' build.sh 2>/dev/null | sed 's/-gcc$//' || echo "")
fi

if [ -z "$TOOLCHAIN_PREFIX" ]; then
    # Try common prefixes
    for prefix in arm-linux-gnueabi arm-linux-gnueabihf aarch64-linux-gnu riscv64-linux-gnu mips-linux-gnu; do
        if command -v "${prefix}-gcc" &>/dev/null; then
            TOOLCHAIN_PREFIX="$prefix"
            break
        fi
    done
fi

if [ -n "$TOOLCHAIN_PREFIX" ]; then
    info "检测到工具链前缀: $TOOLCHAIN_PREFIX"
    detect_toolchain "$TOOLCHAIN_PREFIX"
else
    fail "未检测到任何交叉编译工具链"
    echo "       常见安装命令:"
    echo "       ARM:    sudo apt install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi"
    echo "       ARM64:  sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu"
    echo "       RISC-V: sudo apt install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu"
    ISSUES_FOUND=$((ISSUES_FOUND + 1))
fi

# Check build essentials
for tool in make cmake pkg-config; do
    if command -v "$tool" &>/dev/null; then
        pass "$tool: $(command -v $tool)"
    else
        fail "$tool: 未找到"
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    fi
done

# ============================================================
# 2. Line Ending Detection (CRLF)
# ============================================================
header "2. 换行符检测 (CRLF → LF)"

if command -v dos2unix &>/dev/null; then
    pass "dos2unix 已安装"
else
    warn "dos2unix 未安装"
    echo "       修复: sudo apt install dos2unix"
fi

CRLF_COUNT=0
CRLF_FILES=""
if [ -d "src" ]; then
    CRLF_FILES=$(find src/ -maxdepth 5 -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.sh' -o -name 'configure' -o -name 'Makefile*' -o -name '*.pro' -o -name '*.pri' -o -name '*.conf' -o -name '*.json' \) -exec file {} + 2>/dev/null | grep -c "CRLF" || true)
    CRLF_COUNT=$CRLF_FILES
fi

if [ "$CRLF_COUNT" -gt 0 ]; then
    fail "检测到 $CRLF_COUNT 个文件含有 CRLF 换行符"
    echo "       这会导致: #!/bin/sh^M: 解释器错误"
    echo "       修复: find src/ -type f -name '*.sh' -exec dos2unix {} +"
    echo "       修复: find src/ -type f -name 'configure' -exec dos2unix {} +"
    ISSUES_FOUND=$((ISSUES_FOUND + 1))
else
    pass "未检测到 CRLF 问题"
fi

# ============================================================
# 3. Symbolic Link / Recursive Directory Detection
# ============================================================
header "3. 符号链接 / 递归目录检测"

RECURSIVE_DIRS=0
if [ -d "." ]; then
    # Find directories that contain themselves (recursive symlinks)
    RECURSIVE_DIRS=$(find . -maxdepth 10 -type d -name "alsa" -path "*/alsa/alsa*" 2>/dev/null | wc -l || true)

    if [ "$RECURSIVE_DIRS" -gt 0 ]; then
        fail "检测到递归嵌套目录 (alsa/alsa/alsa/...)"
        echo "       这会导致: 路径过长，xftp/scp 传输失败"
        echo "       修复: find . -path '*/alsa/alsa' -type d -exec rm -rf {} + 2>/dev/null"
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    else
        pass "未检测到递归目录"
    fi

    # Check for broken symlinks
    BROKEN_SYMLINKS=$(find . -maxdepth 5 -type l ! -exec test -e {} \; -print 2>/dev/null | wc -l || true)
    if [ "$BROKEN_SYMLINKS" -gt 0 ]; then
        warn "检测到 $BROKEN_SYMLINKS 个损坏的符号链接"
    fi
fi

# ============================================================
# 4. Path Length Check
# ============================================================
header "4. 路径长度检测"

MAX_PATH_LEN=0
LONGEST_PATH=""
if [ -d "." ]; then
    while IFS= read -r -d '' file; do
        len=${#file}
        if [ "$len" -gt "$MAX_PATH_LEN" ]; then
            MAX_PATH_LEN=$len
            LONGEST_PATH=$file
        fi
    done < <(find . -maxdepth 15 -print0 2>/dev/null)
fi

if [ "$MAX_PATH_LEN" -gt 200 ]; then
    fail "最长路径: $MAX_PATH_LEN 字符 (超过 200 字符限制)"
    echo "       路径: $LONGEST_PATH"
    echo "       这会导致: Windows 文件系统/xftp 传输失败"
    echo "       修复: 缩短目录名，删除冗余嵌套"
    ISSUES_FOUND=$((ISSUES_FOUND + 1))
elif [ "$MAX_PATH_LEN" -gt 150 ]; then
    warn "最长路径: $MAX_PATH_LEN 字符 (接近限制)"
    echo "       路径: $LONGEST_PATH"
else
    pass "最长路径: $MAX_PATH_LEN 字符 (正常)"
fi

# ============================================================
# 5. pkg-config Path Detection
# ============================================================
header "5. pkg-config 路径检测"

if [ -n "$PKG_CONFIG_PATH" ]; then
    pass "PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
else
    warn "PKG_CONFIG_PATH 未设置"
    echo "       交叉编译时需要指向目标平台的 .pc 文件"
    echo "       修复: export PKG_CONFIG_PATH=/path/to/target/lib/pkgconfig"
fi

# Check for hardcoded paths in .pc files
if [ -d "compiled" ]; then
    HARDCODED_PC=$(find compiled/ -name "*.pc" -exec grep -l "prefix=/home/book\|prefix=/home/[^h]" {} + 2>/dev/null | wc -l || true)
    if [ "$HARDCODED_PC" -gt 0 ]; then
        fail "检测到 $HARDCODED_PC 个 .pc 文件含有硬编码路径"
        echo "       这会导致: pkg-config 找不到库文件"
        echo "       修复: 重新运行 configure，或手动修正 .pc 文件中的 prefix"
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    else
        pass ".pc 文件路径正常"
    fi
fi

# ============================================================
# 6. Qt Cross-Compilation Detection
# ============================================================
header "6. Qt 交叉编译检测"

if [ -d "/opt/qt5-arm" ] || [ -d "$HOME/qt5-arm" ]; then
    QT_PREFIX="/opt/qt5-arm"
    [ -d "$HOME/qt5-arm" ] && QT_PREFIX="$HOME/qt5-arm"

    if [ -f "$QT_PREFIX/bin/qmake" ]; then
        QMAKE_ARCH=$(file "$QT_PREFIX/bin/qmake" 2>/dev/null | grep -o "x86-64\|ARM\|aarch64" || echo "unknown")
        pass "qmake: $QT_PREFIX/bin/qmake ($QMAKE_ARCH)"

        # Check if qmake is host arch (correct) or target arch (wrong)
        if echo "$QMAKE_ARCH" | grep -q "x86-64"; then
            pass "qmake 是宿主机架构 (正确 - qmake 应在宿主机运行)"
        elif echo "$QMAKE_ARCH" | grep -q "ARM\|aarch64"; then
            fail "qmake 是目标机架构 (错误 - qmake 应在宿主机运行)"
            echo "       这会导致: 无法在开发机上运行 qmake"
            ISSUES_FOUND=$((ISSUES_FOUND + 1))
        fi
    fi

    # Check Qt libraries
    QT_LIB_COUNT=$(find "$QT_PREFIX/lib" -name "libQt5*.so*" 2>/dev/null | wc -l || true)
    if [ "$QT_LIB_COUNT" -gt 0 ]; then
        pass "Qt 库文件: $QT_LIB_COUNT 个"

        # Verify they are ARM binaries
        SAMPLE_LIB=$(find "$QT_PREFIX/lib" -name "libQt5Core.so.5*" -type f 2>/dev/null | head -1)
        if [ -n "$SAMPLE_LIB" ]; then
            LIB_ARCH=$(file "$SAMPLE_LIB" | grep -o "ARM\|x86-64\|aarch64" || echo "unknown")
            if echo "$LIB_ARCH" | grep -q "ARM\|aarch64"; then
                pass "Qt 库架构: $LIB_ARCH (正确)"
            else
                fail "Qt 库架构: $LIB_ARCH (应为目标机架构)"
                ISSUES_FOUND=$((ISSUES_FOUND + 1))
            fi
        fi
    else
        warn "未找到 Qt 库文件"
    fi
else
    warn "未检测到交叉编译的 Qt"
    echo "       如果项目需要 Qt，请先交叉编译 Qt"
fi

# ============================================================
# 7. Compiled Library Detection
# ============================================================
header "7. 已编译库检测"

if [ -d "compiled" ]; then
    for lib_dir in compiled/*/; do
        lib_name=$(basename "$lib_dir")

        # Find the main binary/library
        main_file=""
        if [ -f "$lib_dir/build/bin/mplayer" ]; then
            main_file="$lib_dir/build/bin/mplayer"
        elif [ -f "$lib_dir/build/lib/libasound.so" ]; then
            main_file="$lib_dir/build/lib/libasound.so"
        elif [ -f "$lib_dir/libz.so" ] || [ -f "$lib_dir/libz.a" ]; then
            main_file="$lib_dir/libz.so"
        fi

        if [ -n "$main_file" ] && [ -f "$main_file" ]; then
            file_type=$(file "$main_file" 2>/dev/null | grep -o "ELF.*" | head -1)
            if echo "$file_type" | grep -q "ARM"; then
                pass "$lib_name: ARM ELF ✓"
            elif echo "$file_type" | grep -q "x86-64"; then
                fail "$lib_name: x86-64 (应为 ARM!)"
                ISSUES_FOUND=$((ISSUES_FOUND + 1))
            else
                info "$lib_name: $file_type"
            fi
        else
            info "$lib_name: 未找到编译产物"
        fi
    done
else
    warn "未找到 compiled/ 目录"
fi

# ============================================================
# 8. Binary Verification
# ============================================================
header "8. 二进制文件架构验证"

if [ -f "car-app/car" ]; then
    car_arch=$(file "car-app/car" 2>/dev/null | grep -o "ARM\|x86-64\|aarch64" || echo "unknown")
    if echo "$car_arch" | grep -q "ARM"; then
        pass "车载应用 car: ARM ELF"
    else
        fail "车载应用 car: $car_arch (架构不匹配!)"
        ISSUES_FOUND=$((ISSUES_FOUND + 1))
    fi
elif [ -d "car-app" ]; then
    warn "car-app/ 存在但未找到 car 二进制文件"
else
    info "未找到 car-app/ 目录"
fi

# ============================================================
# 9. Environment Variables Check
# ============================================================
header "9. 环境变量检测"

check_env() {
    local var_name="$1"
    local var_value="${!var_name}"
    if [ -n "$var_value" ]; then
        pass "$var_name=$var_value"
    else
        warn "$var_name 未设置"
    fi
}

check_env "CC"
check_env "CXX"
check_env "AR"
check_env "STRIP"
check_env "PKG_CONFIG_PATH"
check_env "CFLAGS"
check_env "LDFLAGS"

# ============================================================
# 10. Deployment Directory Check
# ============================================================
header "10. 部署目录检测"

if [ -d "deploy" ]; then
    deploy_size=$(du -sh deploy/ 2>/dev/null | cut -f1)
    pass "deploy/ 目录存在 ($deploy_size)"

    # Check for missing libraries
    if [ -f "deploy/bin/car" ]; then
        missing=$(LD_LIBRARY_PATH=deploy/lib ldd deploy/bin/car 2>/dev/null | grep "not found" || true)
        if [ -n "$missing" ]; then
            fail "缺少运行时库:"
            echo "$missing" | while read line; do
                echo "       $line"
            done
            ISSUES_FOUND=$((ISSUES_FOUND + 1))
        else
            pass "所有运行时库完整"
        fi
    fi
else
    warn "deploy/ 目录不存在 (尚未创建部署包)"
fi

# ============================================================
# Summary
# ============================================================
header "诊断总结"

if [ "$ISSUES_FOUND" -eq 0 ]; then
    echo -e "\n  ${GREEN}未发现问题，环境正常！${NC}\n"
else
    echo -e "\n  ${RED}发现 $ISSUES_FOUND 个问题需要修复${NC}"
    echo -e "  运行 ${BLUE}./fix.sh${NC} 尝试自动修复\n"
fi

exit $ISSUES_FOUND
