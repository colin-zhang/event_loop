#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>

#include "dev_event_def.h"
#include "dev_event_loop.h"
#include "dev_event.h"
#include "dev_udp.h"
#include "dev_if_so.h"

int g_if_broad_cast = 0;

typedef struct _dev_buffer
{
    char *data;
    int  len;
    int  alloced;
}dev_buffer_t;

dev_buffer_t*
dev_buffer_creat(int n)
{
    dev_buffer_t * b = NULL;

    b = calloc(1, sizeof(dev_buffer_t));
    if (b == NULL) {
        dbg_Print("alloc failed");
        exit(1);
    }
    b->data = calloc(n, 1);
    if (b->data == NULL) {
        dbg_Print("alloc buffer data failed");
        exit(1);
    }
    b->len = 0;
    b->alloced = n;

    return b;
}

void 
dev_buffer_reset(dev_buffer_t *buff)
{
    if (buff) {
        memset(buff->data, 0, buff->alloced);
        buff->len = 0;
    }
}

int 
dev_buffer_write(dev_buffer_t* buff, char *str, int str_len)
{
    if (str == NULL || buff == NULL || str_len < 0) {
        return -1;
    }

    int wr_len = buff->alloced - buff->len ;
    wr_len = wr_len < str_len ? str_len : wr_len;
    memcpy(buff->data + buff->len, str, str_len);

    return 0;
}

int
dev_buffer_foward_n(dev_buffer_t* buff, int n)
{
    int nn = 0, al = 0;

    nn  = n;
    al  = buff->len;
    if (al < nn) {
        return -1;
    }

    memcpy(buff->data, buff->data + nn, al - nn);
    return 0;
}

char*
dev_buffer_get_str(dev_buffer_t* buff)
{
    if (buff != NULL) {
        return buff->data;
    } else {
        return NULL;
    }
}

void
dev_buffer_free(dev_buffer_t* buff)
{
    if (buff != NULL) {
        if (buff->data != NULL) {
            free(buff->data);
        }
        free(buff);
    }
}

void 
help(void)
{
    fprintf(stdout, 
            "Usage:"
            "\t-i, the ID\n"
            "\t-p, port\n"
            "\t-s, server\n"
            "\t-c, client\n"
            "\t-b, if broadcast\n");
    fflush(stdout);
}

struct udp_server_data
{
    dev_buffer_t *buff;

};

typedef  struct udp_server_data udp_server_data_t;

int 
udp_server_read_handler(void *ptr)
{
    struct sockaddr_in cl_addr;
    char ip_str[128];
    int n;

    DEV_DECL_FD(ptr, fd);
    udp_server_data_t * srv_data = (udp_server_data_t *)dev_event_get_data(ptr);
    dev_buffer_t * buffer = srv_data->buff;
    
    dev_buffer_reset(buffer);

    dev_udp_receive(fd, buffer->data, buffer->alloced, &cl_addr);

    dev_afinet_ntop(ip_str, sizeof(ip_str), &cl_addr);


    fprintf(stdout, "msg=%s\n", dev_buffer_get_str(buffer));
    fprintf(stdout, "client ip= %s\n", ip_str);
    fprintf(stdout, "client port = %d\n", ntohs(cl_addr.sin_port));

    return 0;
}

int
udp_server_event(int id, int port)
{
    dev_event_t *ev_srv = NULL;
    struct udp_server_data *server_data;
    int fd = 0;

    fd = dev_udp_port_creat(id, port, 0);
    ev_srv = dev_event_creat(fd, DEV_EVENT_TIMER, EPOLLIN , 0);
    if (ev_srv == NULL) {
        dbg_Print("ev_srv, dev_event_creat\n");
        return -1;
    }

    server_data = malloc(sizeof(struct udp_server_data));
    if (server_data == NULL) {
        dbg_Print("error to malloc");
        exit(1);
    }

    server_data->buff = dev_buffer_creat(4096);
 
    dev_event_set_data(ev_srv, server_data, udp_server_read_handler, NULL);
    dev_event_loop_add(dev_event_deafult_loop(), ev_srv);

    return 0;
}

struct udp_client_data
{
    dev_buffer_t *buff_stdin;
    dev_buffer_t *buff_srv;
    int id;
    int port;
};

int 
udp_client_handler(void *ptr)
{
    DEV_DECL_FD(ptr, fd);
    struct udp_client_data *client_data = (struct udp_client_data *)dev_event_get_data(ptr);
    dev_buffer_t * srv_b = client_data->buff_srv;
    struct sockaddr_in cl_addr;

    dev_buffer_reset(srv_b);

    dev_udp_receive(fd, srv_b->data, srv_b->alloced, &cl_addr);

    //dev_afinet_ntop(ip_str, sizeof(ip_str), &cl_addr);


    fprintf(stdout, "msg=%s\n", dev_buffer_get_str(srv_b));


    return 0;
}

int 
stdin_handler(void *ptr)
{
    DEV_DECL_FD(ptr, fd);
    struct udp_client_data *client_data = (struct udp_client_data *)dev_event_get_data(ptr);
    dev_buffer_t * stdin_b = client_data->buff_stdin;


    printf("stdin_handler,stdin_b->alloced = %d, fd=%d\n", stdin_b->alloced, fd);

    dev_buffer_reset(stdin_b);
    dev_get_io_buff_len(fd, &stdin_b->len);
    readn(fd, stdin_b->data, stdin_b->len);

    printf("stdin:%s\n", stdin_b->data);

    int udp_cl_fd = dev_udp_client_creat(0);
    dev_socket_set_broad_cast(udp_cl_fd, g_if_broad_cast);

    dev_udp_send_to_id(udp_cl_fd, client_data->id, client_data->port, stdin_b->data, stdin_b->len);

    dev_event_t *ev_udp_cl = dev_event_creat(udp_cl_fd, DEV_EVENT_IO, EPOLLIN, 0);
    dev_event_set_data(ev_udp_cl, client_data, udp_client_handler, NULL);
    dev_event_loop_add(dev_event_deafult_loop(), ev_udp_cl);

    return 0;
}

int
udp_client_event(int id, int port)
{
    dev_event_t *ev_stdin = NULL;

    struct udp_client_data *client_data = malloc(sizeof(struct udp_client_data));
    if (client_data == NULL) {
        dbg_Print("error to malloc");
        exit(1);
    }

    client_data->buff_stdin = dev_buffer_creat(512);
    client_data->buff_srv = dev_buffer_creat(4096);
    client_data->id = id;
    client_data->port = port;

    ev_stdin = dev_event_creat(0, DEV_EVENT_IO, EPOLLIN | DEV_EPOLLET, 0);
    dev_event_set_data(ev_stdin, client_data, stdin_handler, NULL);
    dev_event_loop_add(dev_event_deafult_loop(), ev_stdin);

    return 0;
}


int main(int argc, char *argv[])
{   
    int opt = 0;
    int id = 0, type = -1, port = 0;
    int if_bsc = 0;

    if (argc < 4) {
        help();
        return 0;
    }

    while( (opt = getopt(argc, argv, "i:p:scb")) != -1) {
        switch(opt) {
        case 'i':
            id = atoi(optarg);
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 's':
            type = 0;
            break;
        case 'c':
            type = 1;
            break;
        case 'b':
            if_bsc = 1;
            break;
        }
    }

    if (dev_event_deafult_loop_init(100) == NULL) {
        fprintf(stderr, "Fail to create deafult loop\n");
        exit(-1);
    }


    if (if_bsc && type == 1) {
        g_if_broad_cast = 1;
        id = 255;
    }

    printf("id = %d, port = %d, type = %d \n", id , port, type);


    switch(type) {
        case 0:
            udp_server_event(id, port);
            break;
        case 1:
            udp_client_event(id, port);
            break;

        default:
            break;

    }

    dev_event_loop_run(dev_event_deafult_loop());
    
    return 0;
}

