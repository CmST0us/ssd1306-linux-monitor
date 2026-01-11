#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include <stdint.h>

// I2C设备信息结构
typedef struct {
    int bus_number;      // I2C总线号（如8表示i2c-8）
    uint8_t address;     // 设备地址
    char device_path[32]; // 设备路径（如"/dev/i2c-8"）
} i2c_device_info_t;

// 扫描所有I2C总线，查找SSD1306设备
// 返回找到的设备信息，如果未找到则bus_number为-1
i2c_device_info_t scan_for_ssd1306(void);

// 扫描指定I2C总线上的所有设备地址
// bus_number: I2C总线号
// addresses: 输出数组，存储找到的设备地址
// max_addresses: 数组最大长度
// 返回: 找到的设备数量
int scan_i2c_bus(int bus_number, uint8_t *addresses, int max_addresses);

#endif // I2C_SCANNER_H

