#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "dev_event_loop.h"
#include "dev_event.h"
#include "dev_signalfd.h"

#define ONE_SECOND 1000000000

dev_event_loop_t *Loop = NULL;
dev_event_t *g_ev1, *g_ev2;
char *g_ev1_prt;

int 
ev2_handler(void *ptr)
{
     dev_event_t *ev = (dev_event_t *)ptr;
     if (memcmp(g_ev1_prt, "ev1", 3) == 0) {
        dev_event_loop_add(dev_event_deafult_loop(), g_ev1);
     }
     memset(g_ev1_prt, 0 , 100);
     read(dev_event_get_fd(ev), dev_event_get_data(ev), 100);
     printf("%s\n", (char *)dev_event_get_data(ev));
     return 0;
}

void 
set_ev2(dev_event_t *ev)
{
    g_ev1_prt = calloc(1024, 1);
    strcpy(g_ev1_prt, "test ev1");
    dev_event_set_data(ev, g_ev1_prt, ev2_handler, NULL);
}

typedef struct _priv_date_t
{
    int count;
    struct itimerspec timerspec;
} priv_data_t;


static struct timespec 
get_it_timespec(double timeout) 
{
    long long int sec = (long long int)timeout;
    long long int nsec = (long long int)((timeout - (double)sec) * ONE_SECOND);
    struct timespec ts;

    ts.tv_sec = sec;
    ts.tv_nsec = nsec;
    return ts;
}

struct itimerspec *
get_it_itimerspec(struct itimerspec *spec, double it_timeout, double interval_timeout) 
{
    spec->it_value = get_it_timespec(it_timeout);
    spec->it_interval = get_it_timespec(interval_timeout);
    return spec;
}

int 
dev_set_relative_timerfd(int fd, double it_timeout, double interval_timeout)
{
    struct itimerspec newValue;

    memset(&newValue, 0, sizeof(newValue));
    newValue.it_value = get_it_timespec(it_timeout);
    newValue.it_interval = get_it_timespec(interval_timeout);
    if (timerfd_settime(fd, 0, &newValue, NULL) != 0) {
        fprintf(stderr, "ERROR: timerfd_settime\n");
        return -1;
    }
    return 0;
}

int 
dev_event_timer_handler(void *ptr)
{
    DEV_DECL_FD(ptr, fd);
    DEV_DECL_PRIV(ptr, priv);
    priv->count++;

    timerfd_settime(fd, 0, get_it_itimerspec(&priv->timerspec, 5, 0), NULL);

    if (priv->count > 2) {
        g_ev2= dev_event_creat(0, DEV_EVENT_IO, EPOLLIN | DEV_EPOLLET, 0);
        set_ev2(g_ev2);
        dev_event_loop_add(dev_event_deafult_loop(), g_ev2);
        dev_event_loop_remove(dev_event_deafult_loop(), (dev_event_t *)ptr);
    }

    printf("count = %d \n", priv->count);

    return 0;
}

dev_event_t *
dev_event_timer_creat(void *data)
{
    dev_event_t *ev_ptr;
    struct itimerspec newValue;
    int fd;

    if ((fd = timerfd_create(CLOCK_MONOTONIC, 0)) < 0) {
        fprintf(stderr, "ERROR: timerfd_create\n");
        return NULL;
    }

    memset(&newValue, 0, sizeof(newValue));
    newValue.it_value = get_it_timespec(2);

    if (timerfd_settime(fd, 0, &newValue, NULL) != 0) {
        fprintf(stderr, "ERROR: timerfd_settime\n");
        return NULL;
    }
    ev_ptr = dev_event_creat(fd, DEV_EVENT_TIMER, EPOLLIN | DEV_EPOLLET, sizeof(priv_data_t));
    if (ev_ptr == NULL) {
        fprintf(stderr, "ERROR: dev_event_creat\n");
        return NULL;
    }

    DEV_DECL_PRIV(ev_ptr, priv);
    priv->count = 0;

    dev_event_set_data(ev_ptr, data, dev_event_timer_handler, NULL);

    return ev_ptr;
}

int 
ev_loop_cb(void *data, uint32_t events)
{
    dev_event_t *ev = (dev_event_t *)data;
    ev->handler_t(data);
}

int main(int argc, char const *argv[])
{

    Loop = dev_event_loop_creat(10, ev_loop_cb);

    g_ev1 = dev_event_timer_creat(NULL);
   
    dev_event_loop_add(dev_event_deafult_loop(), g_ev1);

    dev_event_loop_add(dev_event_deafult_loop(), dev_defualt_signalfd(NULL));

    dev_event_loop_run(dev_event_deafult_loop());
    
    return 0;
}

