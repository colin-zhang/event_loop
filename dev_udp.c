#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "dev_if_so.h"
#include "dev_common.h"

int 
dev_udp_port_creat(int id, unsigned short port, int if_broad) 
{
    int ret;
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return sockfd;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    dev_get_addr_by_id(id, &servaddr);
    
    dev_socket_set_broad_cast(sockfd, if_broad);
    ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        close(sockfd);
        return ret;
    }

    return sockfd;
}


int 
dev_udp_client_creat(int if_broad)
{
    int sockfd;   
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }
    dev_socket_set_broad_cast(sockfd, if_broad);
    return sockfd;
}

int 
dev_udp_client_creat_con(int server_id, unsigned short server_port)
{
    int ret;
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t servlen = sizeof(struct sockaddr_in);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return sockfd;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    dev_get_addr_by_id(server_id, &servaddr);
    
    ret = connect(sockfd, (struct sockaddr *)&servaddr, servlen);
    if (ret < 0) {
        close(sockfd);
        return ret;
    }
    
    return sockfd;
}


int 
dev_udp_send_con(int sockfd, char *msg, int msg_len)
{
    return writen(sockfd, msg, msg_len);
}

int
dev_udp_receive_con(int sockfd, char *rsv, int rsv_len)
{
    return readn(sockfd, rsv, rsv_len);
}


int 
dev_udp_send_to_id(int sockfd, int id, unsigned short port, char *msg, int msg_len)
{
    int ret;
    struct sockaddr_in servaddr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    dev_get_addr_by_id(id, &servaddr);

    ret = sendto(sockfd, msg, msg_len, 0, (struct sockaddr *)&servaddr, addrlen);

    return 0;
}


int 
dev_udp_send(int sockfd, char *msg, int msg_len, struct sockaddr_in *peer_addr)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    ret = sendto(sockfd, msg, msg_len, 0, (struct sockaddr *)peer_addr, addrlen);

    return 0;
}


int 
dev_udp_receive(int sockfd, char *rsv, int rsv_len, struct sockaddr_in *peer_addr)
{
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    ret = recvfrom(sockfd, rsv, rsv_len, 0, (struct sockaddr *)peer_addr, &addrlen);
    return 0;
}


