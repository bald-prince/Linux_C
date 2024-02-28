#ifndef __SOCKET_H
#define __SOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <event2/event.h>

#define SERVER_PORT     8888        //端口号
#define SERVER_IP   "192.168.0.50"
#define MAX_EVENTS      10

//使用poll
void client_data(int client_fd);
int epoll_receive_data(int epoll_fd, int fd,int timeout_ms,void (*callback)(int)) ;
int start_epoll_server(int port);
//使用事件触发
void handle_client_data(evutil_socket_t client_fd, short events, void *arg) ;
void handle_accept(evutil_socket_t listener, short events, void *arg) ;
int start_libevent_server(int port) ;
void* listen_socket_data(void *arg);


#endif // !__SOCK
