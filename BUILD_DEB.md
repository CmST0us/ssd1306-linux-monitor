# DEB包构建说明

本文档说明如何构建和安装 SSD1306 Monitor 的 DEB 包。

## 前置要求

在构建 DEB 包之前，需要安装以下依赖：

```bash
sudo apt update
sudo apt install -y devscripts debhelper cmake build-essential
```

## 构建 DEB 包

### 方法一：使用构建脚本（推荐）

```bash
./build_deb.sh
```

构建完成后，DEB 包将生成在项目目录的上一级目录中（`../ssd1306-monitor_*.deb`）。

### 方法二：手动构建

```bash
# 在项目根目录执行
dpkg-buildpackage -b -us -uc
```

参数说明：
- `-b`: 只构建二进制包
- `-us`: 不签名源码包
- `-uc`: 不签名变更文件

## 安装 DEB 包

```bash
# 安装
sudo dpkg -i ../ssd1306-monitor_*.deb

# 如果提示依赖问题，运行以下命令修复
sudo apt install -f
```

## 安装后配置

安装完成后，服务文件已安装到 `/etc/systemd/system/ssd1306-monitor.service`，但默认不会自动启动。

要启用服务（开机自启动）：

```bash
# 启用服务
sudo systemctl enable ssd1306-monitor

# 启动服务
sudo systemctl start ssd1306-monitor

# 查看服务状态
sudo systemctl status ssd1306-monitor
```

## 卸载

```bash
# 卸载包
sudo dpkg -r ssd1306-monitor

# 或者完全删除（包括配置文件）
sudo dpkg -P ssd1306-monitor
```

## 服务管理

```bash
# 启动服务
sudo systemctl start ssd1306-monitor

# 停止服务
sudo systemctl stop ssd1306-monitor

# 重启服务
sudo systemctl restart ssd1306-monitor

# 查看状态
sudo systemctl status ssd1306-monitor

# 查看日志
sudo journalctl -u ssd1306-monitor -f

# 禁用开机自启动
sudo systemctl disable ssd1306-monitor
```

## DEB 包结构

构建的 DEB 包包含以下内容：

- `/usr/bin/SSD1306_Monitor` - 主程序
- `/etc/systemd/system/ssd1306-monitor.service` - systemd 服务文件

## 故障排除

### 构建失败

1. 确保已安装所有构建依赖
2. 检查 CMake 版本是否 >= 3.10
3. 查看构建日志中的错误信息

### 安装后服务无法启动

1. 检查程序文件是否存在：`ls -l /usr/bin/SSD1306_Monitor`
2. 检查服务文件：`cat /etc/systemd/system/ssd1306-monitor.service`
3. 查看服务日志：`sudo journalctl -u ssd1306-monitor -n 50`
4. 检查 I2C 设备是否可用：`i2cdetect -y 1`

