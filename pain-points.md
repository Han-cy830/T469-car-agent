# 嵌入式交叉编译痛点知识库

本文档记录嵌入式 Linux 交叉编译项目中的常见痛点、诊断方法和解决方案。
Claude Code Agent 在遇到相关问题时会参考本文档。

## 痛点 1: Windows → Linux 文件传输

### 1.1 换行符 (CRLF)

**症状：**
```
/bin/sh^M: 解释器错误: 没有那个文件或目录
bash: ./configure: /bin/sh^M: bad interpreter: No such file or directory
unrecognized debug output level '\x0d'
```

**根因：** Windows 使用 `\r\n` (CRLF)，Linux 使用 `\n` (LF)。从 Windows 传输源码到 Linux 时，`configure`、`Makefile`、`.sh` 等脚本会带有 `\r`。

**诊断：**
```bash
file configure
# 如果显示 "CRLF line terminators" 就有问题

# 批量检测
find . -name 'configure' -exec file {} + | grep CRLF
```

**修复：**
```bash
# 单个文件
dos2unix configure

# 批量修复
find . -type f \( -name '*.sh' -o -name 'configure' -o -name 'Makefile*' \) -exec dos2unix {} +

# 如果没有 dos2unix
sed -i 's/\r$//' configure
```

**预防：** 在 Windows 上使用 Git 时配置 `git config --global core.autocrlf input`

### 1.2 符号链接丢失

**症状：**
```
alsa/alsa/alsa/alsa/alsa/... 无限递归嵌套
路径长度超过 260/4096 字符限制
xftp 传输失败
```

**根因：** Linux 的符号链接（如 `alsa -> .`）在 Windows 上没有对应概念。复制时符号链接变成真实目录，造成递归。

**诊断：**
```bash
# 检测递归目录
find . -path "*/alsa/alsa/alsa" -type d

# 检测符号链接
find . -type l -ls
```

**修复：**
```bash
# 删除递归嵌套
find . -path "*/alsa/alsa" -type d -exec rm -rf {} +

# 或者在传输前处理
# Linux 上打包时使用 tar -h (跟随符号链接)
# 或者在 Windows 上使用支持符号链接的传输工具
```

### 1.3 路径过长

**症状：**
```
xftp 传输失败
scp 报错 "name too long"
Windows 资源管理器无法访问
```

**根因：** 中文目录名 + 深层嵌套 + 版本号，容易超过 Windows 的 260 字符路径限制。

**诊断：**
```bash
find . -type f | awk '{print length, $0}' | sort -rn | head -5
```

**修复：**
```bash
# 缩短目录名
mv "交叉编译已经好的工具-我用的arm-buildroot-linux-gnueabi" compiled
mv "交叉编译用到的工具" src

# 扁平化冗余嵌套
mv compiled/havecompiled/* compiled/
rmdir compiled/havecompiled
```

## 痛点 2: 交叉编译工具链

### 2.1 工具链选择

| 目标架构 | 工具链前缀 | Ubuntu 包名 |
|---------|-----------|------------|
| ARM 32-bit (软浮点) | `arm-linux-gnueabi` | `gcc-arm-linux-gnueabi` |
| ARM 32-bit (硬浮点) | `arm-linux-gnueabihf` | `gcc-arm-linux-gnueabihf` |
| ARM 64-bit (AArch64) | `aarch64-linux-gnu` | `gcc-aarch64-linux-gnu` |
| RISC-V 64-bit | `riscv64-linux-gnu` | `gcc-riscv64-linux-gnu` |
| MIPS | `mips-linux-gnu` | `gcc-mips-linux-gnu` |

### 2.2 环境变量遗漏

**症状：** 编译出的二进制文件是 x86 架构而非 ARM。

**必须设置的变量：**
```bash
export CC=arm-linux-gnueabi-gcc
export CXX=arm-linux-gnueabi-g++
export AR=arm-linux-gnueabi-ar
export STRIP=arm-linux-gnueabi-strip
export RANLIB=arm-linux-gnueabi-ranlib
```

**诊断：**
```bash
# 检查编译出的文件架构
file output_binary
# 期望: ELF 32-bit LSB executable, ARM, EABI5
# 错误: ELF 64-bit LSB executable, x86-64
```

## 痛点 3: pkg-config 路径问题

### 3.1 硬编码路径

**症状：**
```
configure: error: Sufficiently new version of libasound not found.
Package zlib was not found
```

**根因：** 库的 `.pc` 文件中 `prefix=` 指向了原始构建机器的路径。

**诊断：**
```bash
cat compiled/alsa-lib/build/lib/pkgconfig/alsa.pc | grep prefix
# 如果显示 prefix=/home/book/... 就是硬编码了
```

**修复：**
```bash
# 方法 1: 手动修正 .pc 文件
sed -i "s|prefix=.*|prefix=$(pwd)/compiled/alsa-lib/build|" \
    compiled/alsa-lib/build/lib/pkgconfig/alsa.pc

# 方法 2: 通过环境变量覆盖
export PKG_CONFIG_PATH=$(pwd)/compiled/alsa-lib/build/lib/pkgconfig

# 方法 3: 重新 configure 并指定 --prefix
```

### 3.2 PKG_CONFIG_PATH 未设置

**症状：** configure 找不到已编译的库。

**修复：**
```bash
export PKG_CONFIG_PATH=/path/to/compiled/lib/pkgconfig:$PKG_CONFIG_PATH
```

## 痛点 4: Qt 交叉编译

### 4.1 OpenGL/Vulkan 编译错误

**症状：**
```
'QOpenGLTexture' does not name a type
'GlFormat' has not been declared
```

**修复：** configure 时加上：
```bash
./configure -no-opengl -no-vulkan ...
```

### 4.2 skip 不存在的模块

**症状：**
```
Project ERROR: -skip command line argument used with non-existent module 'qtlottie'.
```

**修复：** 检查 Qt 版本支持的模块，移除不存在的 skip 项。
```bash
# 列出所有可用模块
ls src/qt-everywhere-src-5.12.12/ | grep "^qt"
```

### 4.3 qmake 架构错误

**症状：** qmake 是 ARM 二进制，无法在 x86 开发机上运行。

**根因：** qmake 应该是宿主机架构（x86），而不是目标机架构（ARM）。

**诊断：**
```bash
file /opt/qt5-arm/bin/qmake
# 正确: ELF 64-bit LSB ... x86-64
# 错误: ELF 32-bit LSB ... ARM
```

### 4.4 缺少 sysroot 警告

**症状：**
```
WARNING: Cross compiling without sysroot. Disabling pkg-config
```

**影响：** Qt 编译时无法使用 pkg-config 查找系统库。

**修复：** 创建 sysroot 或使用 `-sysroot` 参数：
```bash
./configure -sysroot /path/to/target/sysroot ...
```

## 痛点 5: 库依赖链

### 5.1 编译顺序

正确的依赖顺序：
```
1. zlib (无依赖)
2. alsa-lib (依赖 zlib)
3. alsa-utils (依赖 alsa-lib)
4. MPlayer/FFmpeg (依赖 zlib, alsa-lib)
5. Qt (依赖 zlib)
6. 应用程序 (依赖 Qt, 可能依赖其他库)
```

### 5.2 循环依赖

**症状：** A 依赖 B，B 也依赖 A。

**解决：** 先编译一个最小版本，再编译另一个，最后重新编译第一个。

### 5.3 运行时库缺失

**症状：**
```
error while loading shared libraries: libQt5Core.so.5: cannot open shared object file
```

**诊断：**
```bash
ldd ./car | grep "not found"
```

**修复：**
```bash
# 设置 LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/qt/lib:$LD_LIBRARY_PATH

# 或者把库复制到 /usr/lib (需要 root)
# 或者使用 rpath 编译
```

## 痛点 6: 编译产物验证

### 6.1 架构验证

```bash
# 方法 1: file 命令
file binary
# ARM: ELF 32-bit LSB executable, ARM, EABI5

# 方法 2: readelf
readelf -h binary | grep Machine
# ARM: Machine: ARM

# 方法 3: 批量验证
find . -executable -type f -exec file {} + | grep -v "ARM"
# 如果有输出，说明有些二进制不是 ARM 的
```

### 6.2 依赖验证

```bash
# 检查动态库依赖
readelf -d binary | grep NEEDED

# 检查是否有缺失
LD_LIBRARY_PATH=/path/to/libs ldd binary | grep "not found"
```

## 痛点 7: 部署打包

### 7.1 最小化部署包

```bash
# 只复制必要的库
for lib in libQt5Core libQt5Gui libQt5Widgets libQt5Network; do
    cp /opt/qt5-arm/lib/${lib}.so* deploy/lib/
done

# Strip 二进制文件减小体积
arm-linux-gnueabi-strip deploy/bin/*
```

### 7.2 启动脚本

```bash
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
export QT_QPA_PLATFORM=linuxfb  # 嵌入式设备通常用 framebuffer
"$SCRIPT_DIR/bin/car" "$@"
```

## 痛点 8: 常见编译错误速查表

| 错误信息 | 原因 | 解决方案 |
|---------|------|----------|
| `#!/bin/sh^M` | CRLF 换行符 | `dos2unix file` |
| `unrecognized debug output level '\x0d'` | CRLF 在 Makefile | `dos2unix Makefile` |
| `Permission denied (publickey,password)` | SSH 认证失败 | 检查 SSH 密钥或密码 |
| `cannot find -lasound` | 找不到库文件 | 设置 `LDFLAGS=-L/path/to/lib` |
| `No such file or directory` | 文件不存在或路径错误 | 检查路径，注意相对/绝对路径 |
| `arm-linux-gnueabi-gcc: not found` | 工具链未安装 | `apt install gcc-arm-linux-gnueabi` |
| `Makefile: recipe for target 'xxx' failed` | 编译错误 | 查看具体错误信息 |
| `libQt5Core.so.5: cannot open` | 运行时库缺失 | 设置 `LD_LIBRARY_PATH` |
| `Cross compiling without sysroot` | 缺少 sysroot | 配置 `-sysroot` 参数 |
