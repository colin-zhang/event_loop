#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef UWARE_DEFAULT_IP_DOMAIN
#define UWARE_DEFAULT_IP_DOMAIN 192.168.168.0
#endif

int 
dev_afinet_pton(char *ip, struct sockaddr_in *addr)
{
    return inet_pton(AF_INET, ip, &addr->sin_addr);
}

const char* 
dev_afinet_ntop(char *ip, int ip_len, struct sockaddr_in *addr)
{
    return inet_ntop(AF_INET, &addr->sin_addr, ip, ip_len);
}

int 
dev_get_addr_by_id(int slot_id, struct sockaddr_in *addr)
{
    int a, b, c, d;
    char ip[32] = {0};

    sscanf(UWARE_DEFAULT_IP_DOMAIN, "%d.%d.%d.%d", &a, &b, &c, &d);
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d", a, b, c, d + slot_id);
    inet_pton(AF_INET, ip, &addr->sin_addr);
    return 0;
}

int 
dev_socket_set_broad_cast(int socket_fd, bool bcst)
{
    int option = bcst;
    return setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &option, sizeof(option));
}

int
dev_sockst_set_reuse(int socket_fd, bool rs)
{
    int on = rs;
    return setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}


int 
dev_set_nonblocking(int fd, bool if_noblock) 
{
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        perror("setnonblocking error");
        return -1;
    }
    if (if_noblock) {
        flags = flags | O_NONBLOCK;
    } else {
        flags = flags & ~O_NONBLOCK;
    }

    return fcntl(fd, F_SETFL, flags);
}

int 
dev_get_io_buff_len(int fd, int *buff_len)
{
    return ioctl(fd, FIONREAD, buff_len);
}


