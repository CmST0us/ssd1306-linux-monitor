#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

// 获取内存使用率 (0-100)
int get_memory_usage(void);

// 获取总内存 (MB)
long get_total_memory(void);

// 获取已用内存 (MB)
long get_used_memory(void);

#endif // MEMORY_MONITOR_H


