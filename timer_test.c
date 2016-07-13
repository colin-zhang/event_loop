#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dev_event_timer.h"
#include "dev_event_loop.h"
#include "dev_event.h"


int 
timer1_hander(void *ptr)
{
    fprintf(stdout, "timer1_hander\n");
    return 0;
}


int 
timer2_hander(void *ptr)
{
    fprintf(stdout, "timer2_hander\n");
    return 0;
}

int main(int argc, char const *argv[])
{
    dev_event_t *timer_ev = NULL;
    dev_timer_ev_t *timer1 = NULL, *timer2 = NULL;

    if (dev_event_deafult_loop_init(100) == NULL) {
        fprintf(stderr, "Fail to create deafult loop\n");
        exit(-1);
    }

    timer_ev = dev_event_timer_creat(100, NULL);
    if (timer_ev == NULL) {
        fprintf(stderr, "%s\n", "timer event creat error\n");
        exit(-1);
    }

    timer1 = dev_sub_timer_creat(2.1, 0, timer1_hander, NULL);
    if (timer1 == NULL) {
        fprintf(stderr, "%s\n", "sub timer creat error\n");
        exit(-1);
    }


    timer2 = dev_sub_timer_creat(5, 0, timer2_hander, NULL);
    if (timer1 == NULL) {
        fprintf(stderr, "%s\n", "sub timer creat error\n");
        exit(-1);
    }

    dev_event_timer_add(timer_ev, timer1);
    dev_event_timer_add(timer_ev, timer2);
   
    dev_event_loop_add(dev_event_deafult_loop(), timer_ev);

    dev_event_loop_run(dev_event_deafult_loop());
    
    return 0;
}