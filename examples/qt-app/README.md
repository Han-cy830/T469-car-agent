# Qt ARM 示例

简单的 Qt 应用交叉编译示例。

## 前提条件

已交叉编译 Qt 5.12.12 并安装到 `/opt/qt5-arm/`

## 编译

```bash
# 使用 ARM 版 qmake 生成 Makefile
/opt/qt5-arm/bin/qmake qt-arm-demo.pro

# 编译
make

# 验证
file qt-arm-demo
# 输出: ELF 32-bit LSB executable, ARM, EABI5
```

## 部署

```bash
# 创建部署目录
mkdir -p deploy/bin deploy/lib

# 复制可执行文件
cp qt-arm-demo deploy/bin/

# 复制 Qt 库
for lib in libQt5Core libQt5Gui libQt5Widgets; do
    cp /opt/qt5-arm/lib/${lib}.so* deploy/lib/
done

# 创建启动脚本
cat > deploy/run.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
export QT_QPA_PLATFORM=linuxfb
"$SCRIPT_DIR/bin/qt-arm-demo"
EOF
chmod +x deploy/run.sh
```

## 运行

```bash
# 传输到 ARM 设备
scp -r deploy/ user@arm-device:~/

# 在 ARM 设备上运行
ssh user@arm-device
cd deploy
./run.sh
```
