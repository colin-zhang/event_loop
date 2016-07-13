#include <sys/timerfd.h>
#include <stdint.h>

typedef int  (*timer_handler_t)(void *);


typedef struct _dev_timer_ev_t 
{
    uint8_t repeat;        
    double timeout;
    struct timespec ts;
    timer_handler_t cb; 
    void * ptr;
} dev_timer_ev_t;