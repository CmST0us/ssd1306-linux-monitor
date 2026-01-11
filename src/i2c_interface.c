#include "i2c_interface.h"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static int i2c_fd = -1;
static uint8_t i2c_addr = 0;

int i2c_open(const char *device, uint8_t addr) {
    i2c_fd = open(device, O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open I2C device");
        return -1;
    }
    
    if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) {
        perror("Failed to set I2C slave address");
        close(i2c_fd);
        i2c_fd = -1;
        return -1;
    }
    
    i2c_addr = addr;
    return 0;
}

void i2c_close(void) {
    if (i2c_fd >= 0) {
        close(i2c_fd);
        i2c_fd = -1;
    }
}

int i2c_write_command(uint8_t cmd) {
    if (i2c_fd < 0) {
        return -1;
    }
    
    uint8_t buffer[2] = {0x00, cmd}; // 0x00 = 命令模式
    if (write(i2c_fd, buffer, 2) != 2) {
        perror("Failed to write I2C command");
        return -1;
    }
    
    return 0;
}

int i2c_write_data(uint8_t *data, int len) {
    if (i2c_fd < 0) {
        return -1;
    }
    
    // 为每个字节添加数据模式前缀 (0x40)
    uint8_t buffer[129]; // 最大128字节数据 + 1字节控制
    buffer[0] = 0x40; // 数据模式
    
    if (len > 128) {
        len = 128;
    }
    
    memcpy(&buffer[1], data, len);
    
    if (write(i2c_fd, buffer, len + 1) != (len + 1)) {
        perror("Failed to write I2C data");
        return -1;
    }
    
    return 0;
}

int i2c_write_data_continuous(uint8_t *data, int len) {
    if (i2c_fd < 0) {
        return -1;
    }
    
    // 连续写入数据，第一个字节是0x40，后续数据直接追加
    uint8_t buffer[1025]; // 最大1024字节数据 + 1字节控制
    buffer[0] = 0x40; // 数据模式
    
    if (len > 1024) {
        len = 1024;
    }
    
    memcpy(&buffer[1], data, len);
    
    if (write(i2c_fd, buffer, len + 1) != (len + 1)) {
        perror("Failed to write I2C data continuously");
        return -1;
    }
    
    return 0;
}

int i2c_write_byte(uint8_t byte) {
    return i2c_write_data(&byte, 1);
}

