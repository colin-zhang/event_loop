#include <stdio.h>
#include "dev_event_loop.h"
#include "dev_event.h"

int ev1_handler(void *ptr)
{
	printf("%s\n", "ev1_handler");
	return 0;
}

int main(int argc, char const *argv[])
{

	dev_event_loop_t *loop;
	dev_event_t *ev1;
	loop = dev_event_loop_creat(100);

	ev1 = dev_event_creat(0, DEV_EVENT_IO, EPOLLIN | DEV_EPOLLET, 0);

	//dev_event_set_ep_type(ev1, DEV_EPOLLET | EPOLLIN);
	dev_event_set_data(ev1, NULL, ev1_handler, NULL);

	dev_event_loop_add(loop, ev1);


	dev_event_loop_run(loop);
	
	return 0;
}