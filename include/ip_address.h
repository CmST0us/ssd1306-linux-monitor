#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

// 获取IP地址字符串
// 返回IP地址字符串，如果没有则返回NULL
// 调用者不需要释放内存，使用静态缓冲区
const char* get_ip_address(void);

#endif // IP_ADDRESS_H


