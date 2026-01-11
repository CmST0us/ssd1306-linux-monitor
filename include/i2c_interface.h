#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include <stdint.h>

int i2c_open(const char *device, uint8_t addr);
void i2c_close(void);
int i2c_write_command(uint8_t cmd);
int i2c_write_data(uint8_t *data, int len);
int i2c_write_data_continuous(uint8_t *data, int len);
int i2c_write_byte(uint8_t byte);

#endif // I2C_INTERFACE_H

