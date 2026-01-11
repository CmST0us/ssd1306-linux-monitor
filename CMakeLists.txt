cmake_minimum_required(VERSION 3.10)
project(SSD1306_Monitor VERSION 1.0.0 LANGUAGES C)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 源文件
set(SOURCES
    src/main.c
    src/ssd1306.c
    src/i2c_interface.c
    src/cpu_monitor.c
    src/memory_monitor.c
    src/network_monitor.c
    src/ip_address.c
    src/i2c_scanner.c
)

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接数学库
target_link_libraries(${PROJECT_NAME} m)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
