#ifndef _DEV_EVNET_DEF_H
#define _DEV_EVNET_DEF_H

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "dev_common.h"

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})


#define dbg_Print(fmt, args...) \
        do { \
            fprintf(stderr, "DBG:%s(%d)-%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
            fflush(stderr); \
        } while (0)



#endif