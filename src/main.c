#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "ssd1306.h"
#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "network_monitor.h"
#include "ip_address.h"
#include "i2c_scanner.h"

#define FPS 60
#define FRAME_TIME_NS (1000000000L / FPS)  // 60fps = 16.67ms per frame

static volatile int running = 1;

void signal_handler(int sig __attribute__((unused))) {
    running = 0;
}

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
    const char *i2c_device = NULL;
    
    // 注册信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // 扫描I2C总线，查找SSD1306设备
    printf("正在扫描I2C总线，查找SSD1306设备...\n");
    i2c_device_info_t device_info = scan_for_ssd1306();
    
    if (device_info.bus_number < 0) {
        fprintf(stderr, "错误: 未找到SSD1306设备\n");
        fprintf(stderr, "请确保:\n");
        fprintf(stderr, "1. SSD1306已正确连接到I2C总线\n");
        fprintf(stderr, "2. I2C设备已启用\n");
        fprintf(stderr, "3. 已加载i2c-dev模块 (sudo modprobe i2c-dev)\n");
        return 1;
    }
    
    i2c_device = device_info.device_path;
    printf("找到SSD1306设备:\n");
    printf("  总线: i2c-%d\n", device_info.bus_number);
    printf("  地址: 0x%02X\n", device_info.address);
    printf("  路径: %s\n", device_info.device_path);
    
    // 初始化SSD1306
    printf("\n正在初始化SSD1306 OLED显示器...\n");
    if (ssd1306_init(i2c_device) < 0) {
        fprintf(stderr, "错误: 无法初始化SSD1306显示器\n");
        return 1;
    }
    
    printf("SSD1306初始化成功！\n");
    printf("开始显示系统仪表盘...\n");
    printf("按 Ctrl+C 退出\n");
    
    // 初始化监控模块
    get_cpu_usage(); // 初始化CPU监控
    struct timespec ts_init = {0, 200000000}; // 200ms
    nanosleep(&ts_init, NULL);
    get_cpu_usage(); // 第二次读取，初始化完成
    
    network_monitor_init(); // 初始化网络监控
    
    int frame_count = 0;
    int update_counter = 0;
    struct timespec frame_start, frame_end, sleep_time;
    struct timespec last_network_update = {0, 0};
    
    // 存储数据
    int cpu_usage = 0;
    int mem_usage = 0;
    float rx_speed = 0.0, tx_speed = 0.0;
    const char *ip_addr = NULL;
    
    printf("以 %d FPS 运行中...\n", FPS);
    
    while (running) {
        clock_gettime(CLOCK_MONOTONIC, &frame_start);
        
        // 每2秒更新一次数据（60fps * 2 = 120帧）
        if (update_counter % 120 == 0) {
            cpu_usage = get_cpu_usage();
            mem_usage = get_memory_usage();
            ip_addr = get_ip_address();
        }
        
        // 每秒更新一次网络速度（60fps * 1 = 60帧）
        if (update_counter % 60 == 0) {
            clock_gettime(CLOCK_MONOTONIC, &last_network_update);
            get_network_speed(&rx_speed, &tx_speed);
        }
        
        update_counter++;
        
        // 清空显示缓冲区
        ssd1306_clear();
        
        // ========== 仪表盘布局 ==========
        
        // 1. 顶部：IP地址 (第0-7行)
        if (ip_addr) {
            char ip_line[32];
            snprintf(ip_line, sizeof(ip_line), "IP:%s", ip_addr);
            ssd1306_draw_string(2, 0, ip_line);
        } else {
            ssd1306_draw_string(2, 0, "IP:No Network");
        }
        
        // 分隔线
        ssd1306_draw_line(0, 9, 128, 9);
        
        // 2. CPU使用率 (第11-20行)
        ssd1306_draw_string(2, 11, "CPU:");
        char cpu_str[16];
        snprintf(cpu_str, sizeof(cpu_str), "%3d%%", cpu_usage);
        ssd1306_draw_string(30, 11, cpu_str);
        ssd1306_draw_progress_bar(2, 19, 124, 6, cpu_usage);
        
        // 3. 内存使用率 (第28-37行)
        ssd1306_draw_string(2, 28, "MEM:");
        char mem_str[16];
        snprintf(mem_str, sizeof(mem_str), "%3d%%", mem_usage);
        ssd1306_draw_string(30, 28, mem_str);
        ssd1306_draw_progress_bar(2, 36, 124, 6, mem_usage);
        
        // 4. 网络速度 (第45-54行)
        ssd1306_draw_string(2, 45, "DN:");
        char rx_str[16];
        if (rx_speed < 1024.0) {
            snprintf(rx_str, sizeof(rx_str), "%5.1fKB/s", rx_speed);
        } else {
            snprintf(rx_str, sizeof(rx_str), "%5.2fMB/s", rx_speed / 1024.0);
        }
        ssd1306_draw_string(24, 45, rx_str);
        
        ssd1306_draw_string(2, 54, "UP:");
        char tx_str[16];
        if (tx_speed < 1024.0) {
            snprintf(tx_str, sizeof(tx_str), "%5.1fKB/s", tx_speed);
        } else {
            snprintf(tx_str, sizeof(tx_str), "%5.2fMB/s", tx_speed / 1024.0);
        }
        ssd1306_draw_string(24, 54, tx_str);
        
        // 更新显示
        ssd1306_display();
        
        // 每120帧打印一次调试信息（约每2秒一次，60fps * 2 = 120帧）
        if (frame_count % 120 == 0) {
            printf("CPU: %d%%, MEM: %d%%, RX: %.1fKB/s, TX: %.1fKB/s\n", 
                   cpu_usage, mem_usage, rx_speed, tx_speed);
        }
        frame_count++;
        
        // 计算帧时间并等待
        clock_gettime(CLOCK_MONOTONIC, &frame_end);
        long elapsed_ns = (frame_end.tv_sec - frame_start.tv_sec) * 1000000000L + 
                         (frame_end.tv_nsec - frame_start.tv_nsec);
        long sleep_ns = FRAME_TIME_NS - elapsed_ns;
        
        if (sleep_ns > 0) {
            sleep_time.tv_sec = sleep_ns / 1000000000L;
            sleep_time.tv_nsec = sleep_ns % 1000000000L;
            nanosleep(&sleep_time, NULL);
        }
    }
    
    // 清理
    printf("\n正在关闭显示器...\n");
    ssd1306_deinit();
    printf("程序已退出\n");
    
    return 0;
}
