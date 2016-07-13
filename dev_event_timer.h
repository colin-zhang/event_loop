#include <sys/timerfd.h>
#include <stdint.h>
#include "dev_event.h"

typedef int  (*timer_handler_t)(void *);


typedef struct _dev_timer_ev_t 
{
    uint8_t repeat;        
    double timeout;
    struct timespec ts;
    timer_handler_t cb; 
    void * ptr;
} dev_timer_ev_t;


dev_event_t *dev_event_timer_creat(int num, void *data);
int dev_event_timer_add(dev_event_t *ev, dev_timer_ev_t *tm);
dev_timer_ev_t * dev_sub_timer_creat(double timeout, char repeat, timer_handler_t handler, void * data);

