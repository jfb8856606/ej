#ifndef _NET_H_
#define _NET_H_

#include "ej.h"

int32_t create_server();
int32_t create_event(int32_t sockfd);
int32_t add_event(int32_t sockfd, int32_t epollfd, uint32_t events);
int32_t mod_event(int32_t sockfd, int32_t epollfd, uint32_t events);
int32_t del_event(int32_t sockfd, int32_t epollfd);
int32_t handle_event(int32_t sockfd, int32_t epollfd);

#endif