#include "memory_monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_memory_usage(void) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        return 0;
    }
    
    long mem_total = 0, mem_free = 0, mem_available = 0, buffers = 0, cached = 0;
    char line[256];
    
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) continue;
        if (sscanf(line, "MemFree: %ld kB", &mem_free) == 1) continue;
        if (sscanf(line, "MemAvailable: %ld kB", &mem_available) == 1) continue;
        if (sscanf(line, "Buffers: %ld kB", &buffers) == 1) continue;
        if (sscanf(line, "Cached: %ld kB", &cached) == 1) continue;
    }
    fclose(file);
    
    if (mem_total == 0) {
        return 0;
    }
    
    // 使用MemAvailable（更准确）或计算已用内存
    long mem_used;
    if (mem_available > 0) {
        mem_used = mem_total - mem_available;
    } else {
        mem_used = mem_total - mem_free - buffers - cached;
    }
    
    int usage = (int)((mem_used * 100) / mem_total);
    if (usage < 0) usage = 0;
    if (usage > 100) usage = 100;
    
    return usage;
}

long get_total_memory(void) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        return 0;
    }
    
    long mem_total = 0;
    char line[256];
    
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) {
            break;
        }
    }
    fclose(file);
    
    return mem_total / 1024; // 转换为MB
}

long get_used_memory(void) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        return 0;
    }
    
    long mem_total = 0, mem_free = 0, mem_available = 0, buffers = 0, cached = 0;
    char line[256];
    
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) continue;
        if (sscanf(line, "MemFree: %ld kB", &mem_free) == 1) continue;
        if (sscanf(line, "MemAvailable: %ld kB", &mem_available) == 1) continue;
        if (sscanf(line, "Buffers: %ld kB", &buffers) == 1) continue;
        if (sscanf(line, "Cached: %ld kB", &cached) == 1) continue;
    }
    fclose(file);
    
    if (mem_total == 0) {
        return 0;
    }
    
    long mem_used;
    if (mem_available > 0) {
        mem_used = mem_total - mem_available;
    } else {
        mem_used = mem_total - mem_free - buffers - cached;
    }
    
    return mem_used / 1024; // 转换为MB
}


