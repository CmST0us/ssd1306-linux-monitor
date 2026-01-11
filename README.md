# SSD1306 System Monitor

一个在SSD1306 OLED显示器上显示系统监控信息的C语言程序，支持实时显示CPU使用率、内存使用率、网络速度和IP地址。

## 功能特性

- ✅ 自动扫描I2C总线，查找SSD1306设备
- ✅ 实时显示CPU使用率（带进度条）
- ✅ 实时显示内存使用率（带进度条）
- ✅ 实时显示网络上传/下载速度
- ✅ 显示系统IP地址
- ✅ 60fps流畅刷新
- ✅ 支持开机自启动（systemd服务）

## 项目结构

```
SSD1306/
├── CMakeLists.txt          # CMake构建配置
├── README.md               # 项目说明文档
├── .gitignore             # Git忽略文件
├── src/                   # 源代码目录
│   ├── main.c            # 主程序
│   ├── ssd1306.c         # SSD1306驱动
│   ├── i2c_interface.c   # I2C通信接口
│   ├── i2c_scanner.c     # I2C总线扫描
│   ├── cpu_monitor.c     # CPU监控
│   ├── memory_monitor.c  # 内存监控
│   ├── network_monitor.c # 网络监控
│   └── ip_address.c       # IP地址获取
├── include/               # 头文件目录
│   └── *.h               # 所有头文件
├── scripts/               # 脚本目录
│   ├── install_service.sh    # 服务安装脚本
│   ├── uninstall_service.sh  # 服务卸载脚本
│   └── *.sh              # 其他工具脚本
├── docs/                  # 文档目录
│   └── *.md              # 详细文档
└── build/                 # 构建目录（自动生成）
```

## 编译要求

- CMake 3.10 或更高版本
- GCC编译器（支持C99）
- Linux系统（需要I2C支持）
- 数学库（libm）

## 安装方式

### 方式1：使用Debian包（推荐）

#### 安装构建依赖

首先安装构建deb包所需的依赖：

```bash
sudo ./INSTALL_DEPS.sh
```

或者手动安装：

```bash
sudo apt-get update
sudo apt-get install devscripts debhelper cmake build-essential
```

#### 构建deb包

```bash
./build_deb.sh
```

#### 安装deb包

```bash
cd ..
sudo dpkg -i ssd1306-monitor_*.deb

# 如果提示依赖问题
sudo apt-get install -f
```

### 方式2：从源码编译

#### 1. 创建构建目录

```bash
mkdir -p build
cd build
```

#### 2. 运行CMake配置

```bash
cmake ..
```

#### 3. 编译

```bash
make
```

编译完成后，可执行文件位于 `build/SSD1306_Monitor`

## 运行

### 直接运行

如果从源码编译：
```bash
sudo ./build/SSD1306_Monitor
```

如果安装了deb包：
```bash
sudo SSD1306_Monitor
```

### 安装为系统服务（开机自启动）

#### 如果使用deb包安装

服务文件已自动安装，只需启用：

```bash
sudo systemctl enable ssd1306-monitor
sudo systemctl start ssd1306-monitor
```

#### 如果从源码编译

```bash
cd scripts
sudo ./install_service.sh
sudo systemctl start ssd1306-monitor
```

服务管理命令：
```bash
# 启动服务
sudo systemctl start ssd1306-monitor

# 停止服务
sudo systemctl stop ssd1306-monitor

# 查看状态
sudo systemctl status ssd1306-monitor

# 查看日志
sudo journalctl -u ssd1306-monitor -f

# 禁用自启动
sudo systemctl disable ssd1306-monitor
```

## 硬件要求

- SSD1306 OLED显示器（128x64）
- I2C连接（SDA, SCL）
- 树莓派或其他Linux系统

## 软件要求

- Linux I2C驱动（i2c-dev模块）
- 系统权限（访问I2C设备需要root或i2c组权限）

## 配置

### I2C速度设置

默认情况下，程序会自动扫描所有I2C总线并查找SSD1306设备。如果需要设置特定的I2C速度，请参考 `docs/README_I2C_SPEED.md`。

### 显示布局

程序在128x64 OLED上显示以下信息：
- **顶部**：IP地址
- **CPU使用率**：百分比和进度条
- **内存使用率**：百分比和进度条
- **网络速度**：下载(DN)和上传(UP)速度

## 故障排除

### 1. 无法找到SSD1306设备

- 检查I2C连接是否正确
- 确认I2C已启用：`sudo raspi-config` -> Interface Options -> I2C
- 加载模块：`sudo modprobe i2c-dev`
- 检查设备：`sudo i2cdetect -y <bus_number>`

### 2. 权限错误

- 使用sudo运行程序
- 或将用户添加到i2c组：`sudo usermod -a -G i2c $USER`

### 3. 编译错误

- 确保已安装CMake：`sudo apt-get install cmake`
- 确保已安装构建工具：`sudo apt-get install build-essential`

## 打包

### 构建Debian包

```bash
./build_deb.sh
```

详细说明请参考 `docs/DEBIAN_PACKAGE.md`

## 开发

### 代码结构

- **src/**: 所有C源文件
- **include/**: 所有头文件
- **scripts/**: 工具脚本
- **docs/**: 文档
- **debian/**: Debian打包文件

### 添加新功能

1. 在 `src/` 目录添加新的 `.c` 文件
2. 在 `include/` 目录添加对应的 `.h` 文件
3. 在 `CMakeLists.txt` 的 `SOURCES` 列表中添加新文件
4. 重新编译

## 许可证

本项目采用MIT许可证。

## 贡献

欢迎提交Issue和Pull Request！

## 更新日志

### v1.0.0
- 初始版本
- 支持CPU、内存、网络监控
- 自动I2C总线扫描
- 60fps刷新率
- systemd服务支持

