PRJ_TARGET = dev_ev_test
PRJ_TARGET_TYPE = exe

ifndef PRJ_DEBUG
PRJ_DEBUG = yes
endif

PRJ_SRC = dev_event.c \
	dev_event_loop.c \
	dev_event_test.c \


include ./std.mk
