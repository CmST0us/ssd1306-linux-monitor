#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>

// SSD1306 I2C地址
#define SSD1306_I2C_ADDR 0x3C

// SSD1306命令
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// 屏幕尺寸
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_PAGES (SSD1306_HEIGHT / 8)

// 函数声明
int ssd1306_init(const char *i2c_device);
void ssd1306_deinit(void);
void ssd1306_clear(void);
void ssd1306_display(void);
void ssd1306_set_pixel(int x, int y, bool on);
void ssd1306_draw_line(int x0, int y0, int x1, int y1);
void ssd1306_draw_rect(int x, int y, int w, int h, bool fill);
void ssd1306_draw_waveform(int *data, int data_len, int x, int y, int width, int height);
void ssd1306_draw_char(int x, int y, char c);
void ssd1306_draw_string(int x, int y, const char *str);
void ssd1306_draw_progress_bar(int x, int y, int width, int height, int percent);

#endif // SSD1306_H

