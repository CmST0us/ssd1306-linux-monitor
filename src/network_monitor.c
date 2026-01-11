#include "network_monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static network_stats_t stats = {0, 0, 0, 0, 0};
static int initialized = 0;

void network_monitor_init(void) {
    // 读取初始值
    FILE *file = fopen("/proc/net/dev", "r");
    if (!file) {
        return;
    }
    
    char line[256];
    // 跳过前两行标题
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return;
    }
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return;
    }
    
    long total_rx = 0, total_tx = 0;
    
    while (fgets(line, sizeof(line), file)) {
        char ifname[16];
        long rx_bytes, tx_bytes;
        // 解析格式: interface: rx_bytes rx_packets ... tx_bytes tx_packets ...
        if (sscanf(line, "%15[^:]: %ld %*d %*d %*d %*d %*d %*d %*d %*d %ld",
                   ifname, &rx_bytes, &tx_bytes) == 3) {
            // 跳过lo回环接口
            if (strncmp(ifname, "lo", 2) != 0) {
                total_rx += rx_bytes;
                total_tx += tx_bytes;
            }
        }
    }
    fclose(file);
    
    stats.rx_bytes_prev = total_rx;
    stats.tx_bytes_prev = total_tx;
    stats.rx_bytes = total_rx;
    stats.tx_bytes = total_tx;
    stats.last_update = time(NULL);
    initialized = 1;
}

void get_network_speed(float *rx_speed, float *tx_speed) {
    if (!initialized) {
        network_monitor_init();
        *rx_speed = 0.0;
        *tx_speed = 0.0;
        return;
    }
    
    FILE *file = fopen("/proc/net/dev", "r");
    if (!file) {
        *rx_speed = 0.0;
        *tx_speed = 0.0;
        return;
    }
    
    char line[256];
    // 跳过前两行标题
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return;
    }
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return;
    }
    
    long total_rx = 0, total_tx = 0;
    
    while (fgets(line, sizeof(line), file)) {
        char ifname[16];
        long rx_bytes, tx_bytes;
        if (sscanf(line, "%15[^:]: %ld %*d %*d %*d %*d %*d %*d %*d %*d %ld",
                   ifname, &rx_bytes, &tx_bytes) == 3) {
            // 跳过lo回环接口
            if (strncmp(ifname, "lo", 2) != 0) {
                total_rx += rx_bytes;
                total_tx += tx_bytes;
            }
        }
    }
    fclose(file);
    
    // 计算时间间隔
    time_t now = time(NULL);
    double time_diff = difftime(now, stats.last_update);
    
    // 如果时间间隔太小或太大，重置
    if (time_diff < 0.5 || time_diff > 10.0) {
        stats.rx_bytes_prev = total_rx;
        stats.tx_bytes_prev = total_tx;
        stats.last_update = now;
        *rx_speed = 0.0;
        *tx_speed = 0.0;
        return;
    }
    
    // 计算速度差
    long rx_diff = total_rx - stats.rx_bytes_prev;
    long tx_diff = total_tx - stats.tx_bytes_prev;
    
    // 转换为KB/s
    *rx_speed = (rx_diff / 1024.0) / time_diff;
    *tx_speed = (tx_diff / 1024.0) / time_diff;
    
    // 更新统计
    stats.rx_bytes_prev = stats.rx_bytes;
    stats.tx_bytes_prev = stats.tx_bytes;
    stats.rx_bytes = total_rx;
    stats.tx_bytes = total_tx;
    stats.last_update = now;
    
    // 限制负值（重启后可能重置）
    if (*rx_speed < 0) *rx_speed = 0;
    if (*tx_speed < 0) *tx_speed = 0;
}

