PRJ_TARGET = test
PRJ_TARGET_TYPE = exe

ifndef PRJ_DEBUG
PRJ_DEBUG = yes
endif
MK_DIR ?= $(PWD)

PRJ_SRC = dev_event.c \
	dev_heap.c \
	dev_event_timer.c \
	dev_common.c \
	dev_ipc.c \
	dev_udp.c \
	dev_if_so.c \
	timer_test.c \

#PRJ_CFLAG 
PRJ_LDFLAG = -lrt

include ${MK_DIR}/std.mk
