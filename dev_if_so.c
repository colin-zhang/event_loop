#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define UWARE_DEFAULT_IP_DOMAIN "192.168.168.10"

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

