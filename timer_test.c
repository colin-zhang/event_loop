#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dev_event_timer.h"
#include "dev_event_loop.h"
#include "dev_event.h"


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


int main(int argc, char const *argv[])
{
    dev_event_t *timer_ev = NULL;
    dev_timer_ev_t *timer1 = NULL, *timer2 = NULL, *timer3 = NULL;

    if (dev_event_deafult_loop_init(100) == NULL) {
        fprintf(stderr, "Fail to create deafult loop\n");
        exit(-1);
    }

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
   
    dev_event_loop_add(dev_event_deafult_loop(), timer_ev);

    dev_event_loop_run(dev_event_deafult_loop());
    
    return 0;
}