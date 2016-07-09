PRJ_TARGET = dev_udp_test
PRJ_TARGET_TYPE = exe

ifndef PRJ_DEBUG
PRJ_DEBUG = yes
endif

PRJ_SRC = dev_event.c \
	dev_event_loop.c \
	dev_udp_test.c \
	dev_common.c \
	dev_ipc.c \
	dev_udp.c \
	dev_if_so.c \


include ./std.mk
