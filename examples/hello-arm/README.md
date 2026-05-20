# Hello ARM 示例

最简单的 ARM 交叉编译示例。

## 编译

```bash
# 确保已安装交叉编译器
sudo apt install gcc-arm-linux-gnueabi

# 编译
make

# 验证
file hello-arm
# 输出: ELF 32-bit LSB executable, ARM, EABI5
```

## 运行

将 `hello-arm` 传输到 ARM 设备上运行：

```bash
scp hello-arm user@arm-device:~/
ssh user@arm-device ./hello-arm
```
