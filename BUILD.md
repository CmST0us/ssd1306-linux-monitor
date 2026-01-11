# 构建说明

## 快速开始

### 使用CMake构建（推荐）

```bash
# 1. 创建构建目录
mkdir -p build
cd build

# 2. 运行CMake配置
cmake ..

# 3. 编译
make

# 4. 运行（需要root权限）
sudo ./SSD1306_Monitor
```

### 清理构建

```bash
cd build
make clean
```

或者删除整个构建目录：

```bash
rm -rf build
```

## 安装

### 安装到系统

```bash
cd build
sudo make install
```

默认安装路径：
- 可执行文件：`/usr/local/bin/SSD1306_Monitor`
- 脚本：`/usr/local/scripts/`
- 服务文件：`/etc/systemd/system/ssd1306-monitor.service`

### 自定义安装路径

```bash
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

## 开发模式

### 调试构建

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### 发布构建

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## 依赖检查

### 检查CMake版本

```bash
cmake --version
```

需要CMake 3.10或更高版本。

### 检查编译器

```bash
gcc --version
```

需要支持C99标准的GCC编译器。

## 常见问题

### CMake找不到

安装CMake：
```bash
sudo apt-get update
sudo apt-get install cmake
```

### 编译错误：找不到头文件

确保在 `build` 目录中运行 `cmake ..`，而不是在项目根目录。

### 链接错误：找不到数学库

这是正常的，CMakeLists.txt已经配置了链接数学库。

## 交叉编译

如果需要交叉编译，设置交叉编译工具链：

```bash
cd build
cmake -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc ..
make
```

