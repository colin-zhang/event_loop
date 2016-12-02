#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dev_event.h"
#include "dev_event_timer.h"

int
timer1_hander(void *ptr, void *ptr_self)
{
    fprintf(stdout, "timer ev:%s", (char *)ptr);
    fprintf(stdout, "timer1_hander: %s", (char *)ptr_self);
    return 0;
}


int 
timer2_hander(void *ptr, void *ptr_self)
{
    fprintf(stdout, "timer2_hander\n");
    return 0;
}


int 
timer3_hander(void *ptr, void *ptr_self)
{
    fprintf(stdout, "timer3_hander: %s", (char *)ptr_self);
    return 0;
}


int 
ev_loop_cb(void *data, uint32_t events)
{
    dev_event_t *ev = (dev_event_t *)data;
    ev->handler(data);
    return 0;
}

dev_event_loop_t *Loop = NULL;

int main(int argc, char const *argv[])
{
    dev_event_t *timer_ev = NULL;
    dev_timer_ev_t *timer1 = NULL, *timer2 = NULL, *timer3 = NULL;

    Loop = dev_event_loop_creat(10, ev_loop_cb);

    char *ev_ptr = malloc(512);
    snprintf(ev_ptr, 512, "public\n");    
    timer_ev = dev_event_timer_creat(100, ev_ptr);
    if (timer_ev == NULL) {
        fprintf(stderr, "%s\n", "timer event creat error\n");
        exit(-1);
    }

    char *timer1_ptr = malloc(512);
    snprintf(timer1_ptr, 512, "timer public\n");
    timer1 = dev_sub_timer_creat(2.1, 5, timer1_hander, timer1_ptr);
    if (timer1 == NULL) {
        fprintf(stderr, "%s\n", "sub timer creat error\n");
        exit(-1);
    }

    timer2 = dev_sub_timer_creat(4.1, 0, timer2_hander, NULL);
    if (timer2 == NULL) {
        fprintf(stderr, "%s\n", "sub timer creat error\n");
        exit(-1);
    }

    timer3 = dev_sub_timer_creat(4.1, 0, timer3_hander, timer1_ptr);
    if (timer3 == NULL) {
        fprintf(stderr, "%s\n", "sub timer creat error\n");
        exit(-1);
    }

    dev_event_timer_add(timer_ev, timer1);
    dev_event_timer_add(timer_ev, timer2);
    dev_event_timer_add(timer_ev, timer3);
   
    dev_event_loop_add(Loop, timer_ev);

    dev_event_loop_run(Loop);
    
    return 0;
}