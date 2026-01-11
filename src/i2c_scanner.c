#define _GNU_SOURCE
#include "i2c_scanner.h"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// SSD1306可能的I2C地址
#define SSD1306_ADDR_1 0x3C
#define SSD1306_ADDR_2 0x3D

// 测试设备是否为SSD1306
static int test_ssd1306_device(int bus_fd, uint8_t addr) {
    // 尝试设置从设备地址
    if (ioctl(bus_fd, I2C_SLAVE, addr) < 0) {
        return 0;
    }
    
    // SSD1306在初始化时会响应命令
    // 尝试发送一个简单的命令（DISPLAYOFF = 0xAE）
    uint8_t cmd = 0xAE;
    uint8_t buffer[2] = {0x00, cmd}; // 0x00 = 命令模式
    
    // 尝试写入命令，如果成功则可能是SSD1306
    if (write(bus_fd, buffer, 2) == 2) {
        // 给设备一点时间响应
        struct timespec ts = {0, 1000000}; // 1ms
        nanosleep(&ts, NULL);
        return 1;
    }
    
    return 0;
}

int scan_i2c_bus(int bus_number, uint8_t *addresses, int max_addresses) {
    char device_path[32];
    snprintf(device_path, sizeof(device_path), "/dev/i2c-%d", bus_number);
    
    int fd = open(device_path, O_RDWR);
    if (fd < 0) {
        return 0;
    }
    
    int count = 0;
    
    // 扫描所有可能的I2C地址（0x08-0x77）
    for (uint8_t addr = 0x08; addr <= 0x77 && count < max_addresses; addr++) {
        // 尝试设置从设备地址
        if (ioctl(fd, I2C_SLAVE, addr) >= 0) {
            // 尝试写入一个字节，如果设备存在通常会响应
            uint8_t test_byte = 0x00;
            if (write(fd, &test_byte, 1) >= 0) {
                addresses[count++] = addr;
            }
        }
    }
    
    close(fd);
    return count;
}

i2c_device_info_t scan_for_ssd1306(void) {
    i2c_device_info_t result = {-1, 0, ""};
    
    // 打开/sys/bus/i2c/devices目录，查找所有i2c总线
    DIR *dir = opendir("/sys/bus/i2c/devices");
    if (!dir) {
        // 如果无法打开，尝试直接扫描/dev/i2c-*
        dir = opendir("/dev");
        if (!dir) {
            return result;
        }
    }
    
    struct dirent *entry;
    int bus_numbers[16] = {0}; // 存储找到的总线号
    int bus_count = 0;
    
    // 查找所有i2c总线
    while ((entry = readdir(dir)) != NULL && bus_count < 16) {
        int bus_num;
        if (sscanf(entry->d_name, "i2c-%d", &bus_num) == 1) {
            bus_numbers[bus_count++] = bus_num;
        }
    }
    closedir(dir);
    
    // 如果没有找到，尝试直接扫描/dev目录
    if (bus_count == 0) {
        dir = opendir("/dev");
        if (dir) {
            while ((entry = readdir(dir)) != NULL && bus_count < 16) {
                int bus_num;
                if (sscanf(entry->d_name, "i2c-%d", &bus_num) == 1) {
                    bus_numbers[bus_count++] = bus_num;
                }
            }
            closedir(dir);
        }
    }
    
    // 如果还是没找到，尝试常见的总线号
    if (bus_count == 0) {
        for (int i = 0; i <= 10; i++) {
            char test_path[32];
            snprintf(test_path, sizeof(test_path), "/dev/i2c-%d", i);
            if (access(test_path, F_OK) == 0) {
                bus_numbers[bus_count++] = i;
            }
        }
    }
    
    // 对总线号排序
    for (int i = 0; i < bus_count - 1; i++) {
        for (int j = i + 1; j < bus_count; j++) {
            if (bus_numbers[i] > bus_numbers[j]) {
                int temp = bus_numbers[i];
                bus_numbers[i] = bus_numbers[j];
                bus_numbers[j] = temp;
            }
        }
    }
    
    // 扫描每个总线，查找SSD1306
    uint8_t ssd1306_addresses[] = {SSD1306_ADDR_1, SSD1306_ADDR_2};
    
    for (int i = 0; i < bus_count; i++) {
        int bus_num = bus_numbers[i];
        char device_path[32];
        snprintf(device_path, sizeof(device_path), "/dev/i2c-%d", bus_num);
        
        int fd = open(device_path, O_RDWR);
        if (fd < 0) {
            continue;
        }
        
        // 测试SSD1306的两个可能地址
        for (int j = 0; j < 2; j++) {
            uint8_t addr = ssd1306_addresses[j];
            if (test_ssd1306_device(fd, addr)) {
                result.bus_number = bus_num;
                result.address = addr;
                int len = snprintf(result.device_path, sizeof(result.device_path), "%s", device_path);
                if (len >= (int)sizeof(result.device_path)) {
                    result.device_path[sizeof(result.device_path) - 1] = '\0';
                }
                close(fd);
                return result;
            }
        }
        
        close(fd);
    }
    
    return result;
}

