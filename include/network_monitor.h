#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <time.h>

typedef struct {
    long rx_bytes;      // 接收字节数
    long tx_bytes;      // 发送字节数
    long rx_bytes_prev; // 上一次接收字节数
    long tx_bytes_prev; // 上一次发送字节数
    time_t last_update; // 上次更新时间
} network_stats_t;

// 初始化网络监控
void network_monitor_init(void);

// 获取网络速度 (KB/s)
// rx_speed: 接收速度 (下载)
// tx_speed: 发送速度 (上传)
void get_network_speed(float *rx_speed, float *tx_speed);

#endif // NETWORK_MONITOR_H

