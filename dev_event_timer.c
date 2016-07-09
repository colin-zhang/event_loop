#include <sys/timerfd.h>
#include "dev_event.h"
#include <string.h>


typedef struct _priv_date_t
{
	dev_timer_ev_t **heap;
	int size;
	int count;
} priv_data_t;

int dev_event_timer_handler(void *ptr)
{
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
	priv->heap = calloc(num, sizeof(priv_data_t));
	priv->size = num;

	dev_event_set_data(ev_ptr, data, dev_event_timer_handler, NULL);

	return ev_ptr;
}



dev_event_timer_add(int num, void *data)
{

}
