#ifndef _DEV_IF_SO_H
#define _DEV_IF_SO_H 

int dev_get_addr_by_id(int slot_id, struct sockaddr_in *addr);
int dev_socket_set_broad_cast(int socket_fd, bool bcst);
int dev_sockst_set_reuse(int socket_fd, bool rs);

#endif
