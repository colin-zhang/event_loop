#include "dev_event_loop.h"
#include "dev_event_def.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "dev_event.h"

static dev_event_loop_t * deafult_loop = NULL;


dev_event_loop_t * 
dev_event_loop_creat(int max_event, loop_cb_t cb) 
{
    dev_event_loop_t *loop;
    loop = (dev_event_loop_t *)calloc(1, sizeof(dev_event_loop_t));

    loop->ev_max = max_event;
    loop->ep_fd = epoll_create(max_event);
    if (loop->ep_fd == -1) {
        dbg_Print("create epoll:%s\n", strerror(errno));
        return NULL;
    }

    loop->ep_events = (struct epoll_event *)calloc(max_event, sizeof(struct epoll_event));
    if (loop->ep_events == NULL) {
        dbg_Print("No enough memory for loop->ep_events\n");
        return NULL;
    }
    loop->cb = cb;
    return loop;
}

int 
dev_event_loop_run(dev_event_loop_t* loop) 
{
    int i, num;

    for(;;) {
        num = epoll_wait(loop->ep_fd, loop->ep_events, loop->ev_max, -1);
        if (num == -1) {
            if (errno == EINTR)
                continue;
            dbg_Print("epoll_wait: %s\n", strerror(errno));
            return -1;
        }
        for (i = 0; i < num; i++) {
            struct epoll_event *ev = &loop->ep_events[i];
            uint32_t events = 0;

            if (ev) {
                if (ev->events & EPOLLERR) {
                    events |= EVENT_ERR;
                }

                if (ev->events & (EPOLLIN | EPOLLHUP)) {
                    events |= EVENT_READ;
                }

                if (ev->events & EPOLLOUT) {
                    events |= EVENT_WRITE;
                }
               
                if (ev->data.ptr) {
                    dev_event_t *dev_event = (dev_event_t *)ev->data.ptr;
                    dev_event->handler(ev->, events);
                }
                
                
                handler(ev->data.ptr, events)

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
            dbg_Print("epoll_ctl: %s\n", strerror(errno));
            return -1;
        }
    } else {
        dbg_Print("exist");
    }
    
    dev_event_set_property(event_ptr, 1);

    return 0;
}

int 
dev_event_loop_remove(dev_event_loop_t* loop, dev_event_t *event_ptr)
{
    struct epoll_event ev;
    int fd;

    dbg_Print("list num = %d \n", dev_event_list_get_num(loop->event_list));
    dev_event_list_remove(loop->event_list, event_ptr);
    dbg_Print("list num = %d \n", dev_event_list_get_num(loop->event_list));

    fd = dev_event_get_fd(event_ptr);
    ev.data.fd = fd;
    ev.events = dev_event_get_ep_type(event_ptr);
    if (-1 == epoll_ctl(loop->ep_fd, EPOLL_CTL_DEL, fd, &ev)) {
        dbg_Print("epoll_ctl: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int 
dev_event_loop_pause(dev_event_loop_t* loop, dev_event_t *event_ptr)
{
    dev_epoll_type_t ev_type;
    struct epoll_event ev;
    int fd;

    fd = dev_event_get_fd(event_ptr);
    ev.data.fd = fd;
    ev_type = dev_event_get_ep_type(event_ptr);
    if (ev_type & DEV_EPOLLIN) ev_type &= (~DEV_EPOLLIN);
    ev.events = ev_type;
    if (-1 == epoll_ctl(loop->ep_fd, EPOLL_CTL_MOD, fd, &ev)) {
        dbg_Print("epoll_ctl: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int 
dev_event_loop_resume(dev_event_loop_t* loop, dev_event_t *event_ptr)
{
    return 0;
}

dev_event_loop_t * 
dev_event_deafult_loop_init(int max_event)
{
    if (deafult_loop == NULL) {
        deafult_loop = dev_event_loop_creat(max_event);
    }
    return deafult_loop;
}

dev_event_loop_t * 
dev_event_deafult_loop(void)
{
    return deafult_loop;
}
