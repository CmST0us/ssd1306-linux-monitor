#include "ip_address.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>

const char* get_ip_address(void) {
    static char ip_str[16] = {0};
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        return NULL;
    }
    
    // 查找第一个非回环的IPv4地址
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        
        // 跳过回环接口
        if (strncmp(ifa->ifa_name, "lo", 2) == 0) continue;
        
        // 只处理IPv4地址
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
            const char *ip = inet_ntoa(sin->sin_addr);
            if (ip) {
                strncpy(ip_str, ip, sizeof(ip_str) - 1);
                freeifaddrs(ifaddr);
                return ip_str;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    return NULL;
}


