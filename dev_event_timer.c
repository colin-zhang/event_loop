#include <sys/timerfd.h>
#include "dev_event.h"
#include <string.h>

#define ONE_SECOND 1000000000

typedef struct _priv_date_t
{
	dev_timer_ev_t **heap;
	int size;
	int count;
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


struct itimerspec *
get_it_itimerspec(struct itimerspec *spec, double it_timeout, double interval_timeout) 
{
    spec->it_value = get_it_timespec(it_timeout);
    spec->it_interval = get_it_timespec(interval_timeout);
    return spec;
}



int dev_event_timer_handler(void *ptr)
{
	void * data = (void *)dev_event_get_data(ptr);

	
	printf("%s\n", "ev1_handler");
	return 0;
}

dev_event_t *
dev_event_timer_creat(int num, void *data)
{
	dev_event_t *ev_ptr;
	struct itimerspec newValue;
	int fd;

	if ((fd = timerfd_create(CLOCK_MONOTONIC, 0)) < 0) {
		dbg_Print("timerfd_create\n");
		return -1;
	}

	memset(&newValue, 0, sizeof(newValue));

	if (timerfd_settime(fd, 0, &newValue, NULL) != 0) {
		dbg_Print("timerfd_settime\n");
		return -1;
	}
	ev_ptr = dev_event_creat(fd, DEV_EVENT_TIMER, EPOLLIN | DEV_EPOLLET, sizeof(priv_data_t));
	if (ev_ptr == NULL) {
		dbg_Print("dev_event_creat\n");
		return -1;
	}

	DEV_DECL_PRIV(ev_ptr, priv);
	priv->count = 0;
	priv->heap = calloc(num, sizeof(dev_timer_ev_t));
	priv->size = num;

	dev_set_relative_timerfd(fd, 0, 0);
	dev_event_set_data(ev_ptr, data, dev_event_timer_handler, NULL);

	return ev_ptr;
}



dev_event_timer_add(int num, void *data)
{

}
