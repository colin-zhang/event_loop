#include "dev_event_loop.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

dev_event_list_t* 
dev_event_list_creat(int num) 
{
    dev_event_list_t *list = NULL;

    list = calloc(1, sizeof(dev_event_list_t));
    if (list == NULL) {
        return NULL;
    }
    list->event_cnt = 0;
    list->event_max = num;
    list->head = NULL;
    list->tail = NULL;

    return list;
}


void 
dev_event_list_destroy(dev_event_list_t *list) {
    dev_event_t *ev_ptr = NULL;

}

int 
dev_event_list_add(dev_event_list_t *list, dev_event_t *event_ptr) 
{
    if (list == NULL || event_ptr == NULL) {
        return -1;
    }

    if (list->head == NULL) {
        list->head = event_ptr;
        list->tail = event_ptr;
    } else {
        dev_event_set_next(list->tail, event_ptr);
        list->tail = event_ptr;
    }
    list->event_cnt++;

    return 0;
}



dev_event_t *
dev_event_list_find_by_fd(dev_event_list_t *list, int fd) 
{
    dev_event_t *ev_ptr = NULL;

    if (list == NULL) {
        return NULL;
    } else {
        ev_ptr = list->head;
    }

    while (ev_ptr) {
        if (dev_event_get_fd(ev_ptr) == fd) {
            break;
        } else {
            ev_ptr = dev_event_get_next(ev_ptr);
        }
    }

    return ev_ptr;
}

dev_event_loop_t * 
dev_event_loop_creat(int max_event) 
{
    dev_event_loop_t *loop;
    loop = (dev_event_loop_t *)calloc(1, sizeof(dev_event_loop_t));

    loop->ev_max = max_event;
    loop->ep_fd = epoll_create1(0);
    if (loop->ep_fd == -1) {
        fprintf(stderr, "create epoll error:%s\n", strerror(errno));
        return NULL;
    }

    loop->ep_events = (struct epoll_event *)calloc(max_event, sizeof(struct epoll_event));
    if (loop->ep_events == NULL) {
        fprintf(stderr, "No enough memory for loop->ep_events\n");
        return NULL;
    }

    loop->event_list = dev_event_list_creat(max_event);
    if (loop->event_list == NULL) {
        fprintf(stderr, "No enough memory for loop->ep_events\n");
        return NULL;
    }

    return loop;
}

int 
dev_event_loop_run(dev_event_loop_t* loop) 
{
    int i, num, fd;
    dev_event_t *ev_ptr = NULL;
    handler_t handler;
    void * handle_ptr = NULL;
    bool handling;

    for(;;) {
        num = epoll_wait(loop->ep_fd, loop->ep_events, loop->ev_max, -1);
        if (num == -1) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "ERROR: epoll wait error: %s\n", strerror(errno));
            return -1;
        }
        for (i = 0; i < num; i++) {
            fd = loop->ep_events[i].data.fd;
            ev_ptr = dev_event_list_find_by_fd(loop->event_list, fd);
            if (ev_ptr) {
                handler = dev_event_get_handler(ev_ptr, &handle_ptr, &handling);
                if (handler != NULL && handle_ptr != NULL && handling) {
                    handler(handle_ptr);
                }
            }
        }
    }

    return 0;
}


int 
dev_event_loop_add(dev_event_loop_t* loop, dev_event_t *event_ptr)
{
    dev_epoll_type_t ep_type;
    struct epoll_event ev;
    int ev_fd;

    if (loop == NULL || event_ptr == NULL) {
        return -1;
    } else {
       ep_type = dev_event_get_ep_type(event_ptr);
       ev_fd = dev_event_get_fd(event_ptr);
    }

    if (dev_event_list_find_by_fd(loop->event_list, ev_fd) == NULL) {
        dev_event_list_add(loop->event_list, event_ptr);
        ev.data.fd = ev_fd;
        ev.events  = ep_type;
        if(-1 == epoll_ctl(loop->ep_fd, EPOLL_CTL_ADD, ev_fd, &ev)) {
            fprintf(stderr, "ERROR: epoll_ctl add in ev_register: %s\n", strerror(errno));
            return -1;
        }
    } else {

    }
    
    dev_event_set_property(event_ptr, 1);
    return 0;
}

int 
dev_event_loop_remove()
{
    return 0;
}

int 
dev_event_loop_pause()
{
    return 0;
}

int 
dev_event_loop_resume()
{
    return 0;
}