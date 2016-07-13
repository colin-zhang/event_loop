#ifndef _DEV_RING_BUFFER_H
#define _DEV_RING_BUFFER_H

typedef struct _dev_rb
{
    char *buffer;
    int length;
    int start;
    int end;
} dev_rb_t;

#endif