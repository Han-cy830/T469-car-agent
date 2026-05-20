# ALSA 音频播放器示例

演示如何交叉编译使用 ALSA 库的音频应用。

## 前提条件

已交叉编译 alsa-lib 并安装到 `compiled/alsa-lib-1.2.2/build/`

## 编译

```bash
make

# 验证
file alsa-demo
# 输出: ELF 32-bit LSB executable, ARM, EABI5
```

## 部署

```bash
# 复制 ALSA 库
cp ../../../compiled/alsa-lib-1.2.2/build/lib/libasound.so* deploy/lib/

# 复制可执行文件
cp alsa-demo deploy/bin/
```

## 运行

```bash
# 在 ARM 设备上
export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
./bin/alsa-demo

# 输出:
# ALSA Cross-Compilation Demo
# ALSA library version: 1.2.2
# 音频设备打开成功!
# 采样率: 44100 Hz
# 声道数: 2
# 格式: 16-bit little-endian
# 设备关闭成功
```
