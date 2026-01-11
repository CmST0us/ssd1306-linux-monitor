#include "ssd1306.h"
#include "i2c_interface.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static uint8_t buffer[SSD1306_PAGES * SSD1306_WIDTH];
static bool initialized = false;

int ssd1306_init(const char *i2c_device) {
    if (i2c_open(i2c_device, SSD1306_I2C_ADDR) < 0) {
        return -1;
    }
    
    // 初始化序列
    i2c_write_command(SSD1306_DISPLAYOFF);
    i2c_write_command(SSD1306_SETDISPLAYCLOCKDIV);
    i2c_write_command(0x80);
    i2c_write_command(SSD1306_SETMULTIPLEX);
    i2c_write_command(SSD1306_HEIGHT - 1);
    i2c_write_command(SSD1306_SETDISPLAYOFFSET);
    i2c_write_command(0x0);
    i2c_write_command(SSD1306_SETSTARTLINE | 0x0);
    i2c_write_command(SSD1306_CHARGEPUMP);
    i2c_write_command(0x14);
    i2c_write_command(SSD1306_MEMORYMODE);
    i2c_write_command(0x00);
    i2c_write_command(SSD1306_SEGREMAP | 0x1);
    i2c_write_command(SSD1306_COMSCANDEC);
    i2c_write_command(SSD1306_SETCOMPINS);
    i2c_write_command(0x12);
    i2c_write_command(SSD1306_SETCONTRAST);
    i2c_write_command(0xCF);
    i2c_write_command(SSD1306_SETPRECHARGE);
    i2c_write_command(0xF1);
    i2c_write_command(SSD1306_SETVCOMDETECT);
    i2c_write_command(0x40);
    i2c_write_command(SSD1306_DISPLAYALLON_RESUME);
    i2c_write_command(SSD1306_NORMALDISPLAY);
    i2c_write_command(SSD1306_DISPLAYON);
    
    ssd1306_clear();
    ssd1306_display(); // 立即清屏并显示
    initialized = true;
    return 0;
}

void ssd1306_deinit(void) {
    if (initialized) {
        i2c_write_command(SSD1306_DISPLAYOFF);
        i2c_close();
        initialized = false;
    }
}

void ssd1306_clear(void) {
    memset(buffer, 0, sizeof(buffer));
}

void ssd1306_display(void) {
    if (!initialized) return;
    
    i2c_write_command(SSD1306_COLUMNADDR);
    i2c_write_command(0);
    i2c_write_command(SSD1306_WIDTH - 1);
    i2c_write_command(SSD1306_PAGEADDR);
    i2c_write_command(0);
    i2c_write_command(SSD1306_PAGES - 1);
    
    // 一次性发送所有数据（1024字节，足够128x64/8=1024字节）
    i2c_write_data_continuous(buffer, sizeof(buffer));
}

void ssd1306_set_pixel(int x, int y, bool on) {
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT) {
        return;
    }
    
    int page = y / 8;
    int bit = y % 8;
    int index = page * SSD1306_WIDTH + x;
    
    if (on) {
        buffer[index] |= (1 << bit);
    } else {
        buffer[index] &= ~(1 << bit);
    }
}

void ssd1306_draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    int x = x0;
    int y = y0;
    
    while (1) {
        ssd1306_set_pixel(x, y, true);
        
        if (x == x1 && y == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void ssd1306_draw_rect(int x, int y, int w, int h, bool fill) {
    if (fill) {
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                ssd1306_set_pixel(x + j, y + i, true);
            }
        }
    } else {
        // 只画边框
        for (int i = 0; i < w; i++) {
            ssd1306_set_pixel(x + i, y, true);
            ssd1306_set_pixel(x + i, y + h - 1, true);
        }
        for (int i = 0; i < h; i++) {
            ssd1306_set_pixel(x, y + i, true);
            ssd1306_set_pixel(x + w - 1, y + i, true);
        }
    }
}

void ssd1306_draw_waveform(int *data, int data_len, int x, int y, int width, int height) {
    if (data_len < 2) return;
    
    // 绘制边框
    ssd1306_draw_rect(x, y, width, height, false);
    
    // 计算数据点之间的间距
    float step = (float)width / (data_len - 1);
    
    // 找到数据的最大值和最小值
    int min_val = data[0];
    int max_val = data[0];
    for (int i = 1; i < data_len; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    // 如果所有值相同，居中显示
    if (max_val == min_val) {
        int center_y = y + height / 2;
        for (int i = 0; i < data_len - 1; i++) {
            int x0 = x + (int)(i * step);
            int x1 = x + (int)((i + 1) * step);
            ssd1306_draw_line(x0, center_y, x1, center_y);
        }
        return;
    }
    
    // 归一化并绘制波形
    float range = max_val - min_val;
    for (int i = 0; i < data_len - 1; i++) {
        int x0 = x + (int)(i * step);
        int x1 = x + (int)((i + 1) * step);
        
        // 将数据值映射到显示区域
        int y0 = y + height - 1 - (int)((data[i] - min_val) * (height - 2) / range);
        int y1 = y + height - 1 - (int)((data[i + 1] - min_val) * (height - 2) / range);
        
        // 限制在显示区域内
        if (y0 < y + 1) y0 = y + 1;
        if (y0 > y + height - 2) y0 = y + height - 2;
        if (y1 < y + 1) y1 = y + 1;
        if (y1 > y + height - 2) y1 = y + height - 2;
        
        ssd1306_draw_line(x0, y0, x1, y1);
    }
}

// 简单的5x7字体数据（ASCII 32-126）
static const uint8_t font_5x7[95][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 空格 (32)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x00, 0xA0, 0x60, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0 (48)
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x59, 0x51, 0x3E}, // @
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, // A (65)
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // backslash
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a (97)
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x40, 0x80, 0x84, 0x7D, 0x00}, // j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0xFC, 0x24, 0x24, 0x24, 0x18}, // p
    {0x18, 0x24, 0x24, 0x18, 0xFC}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x1C, 0xA0, 0xA0, 0xA0, 0x7C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x10, 0x08, 0x08, 0x10, 0x08}, // ~
};

void ssd1306_draw_char(int x, int y, char c) {
    if (c < 32 || c > 126) return;
    
    const uint8_t *font_data = font_5x7[c - 32];
    
    for (int col = 0; col < 5; col++) {
        uint8_t col_data = font_data[col];
        for (int row = 0; row < 7; row++) {
            if (col_data & (1 << row)) {
                ssd1306_set_pixel(x + col, y + row, true);
            }
        }
    }
}

void ssd1306_draw_string(int x, int y, const char *str) {
    int pos_x = x;
    while (*str && pos_x < SSD1306_WIDTH - 5) {
        ssd1306_draw_char(pos_x, y, *str);
        pos_x += 6; // 字符宽度5 + 间距1
        str++;
    }
}

void ssd1306_draw_progress_bar(int x, int y, int width, int height, int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    
    // 绘制边框
    ssd1306_draw_rect(x, y, width, height, false);
    
    // 计算填充宽度
    int fill_width = ((width - 2) * percent) / 100;
    if (fill_width > 0) {
        // 绘制填充
        ssd1306_draw_rect(x + 1, y + 1, fill_width, height - 2, true);
    }
}

