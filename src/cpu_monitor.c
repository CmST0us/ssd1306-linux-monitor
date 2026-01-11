#include "cpu_monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned long long last_idle = 0;
static unsigned long long last_total = 0;
static int initialized = 0;

int get_cpu_usage(void) {
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        return 0;
    }
    
    char line[256];
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }
    fclose(file);
    
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total, total_idle, diff_idle, diff_total;
    
    // 解析 /proc/stat 的第一行 (cpu行)
    sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    
    total_idle = idle + iowait;
    total = user + nice + system + idle + iowait + irq + softirq + steal;
    
    // 第一次调用时，只保存值，返回0
    if (!initialized) {
        last_idle = total_idle;
        last_total = total;
        initialized = 1;
        return 0;
    }
    
    diff_idle = total_idle - last_idle;
    diff_total = total - last_total;
    
    last_idle = total_idle;
    last_total = total;
    
    if (diff_total == 0) {
        return 0;
    }
    
    int usage = 100 - (int)((diff_idle * 100) / diff_total);
    
    if (usage < 0) usage = 0;
    if (usage > 100) usage = 100;
    
    return usage;
}

