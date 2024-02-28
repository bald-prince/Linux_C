#include "socket.h"

void client_data(int client_fd)
{
    char buffer[1024];
    int read_size = read(client_fd,buffer,sizeof(buffer));

    if(read_size == 0)
    {
        //连接关闭
        printf("Connection closed. Client_fd: %d\n",client_fd);
        close(client_fd);
    }
    else if(read_size < 0)
    {
        perror("Read failed");
    }
    else
    {
        buffer[read_size] = '\0';
        printf("Received data from client fd_%d:%s\n",client_fd,buffer);
    }
}

int epoll_receive_data(int epoll_fd, int fd,int timeout_ms,void (*callback)(int)) 
{
    struct epoll_event events[MAX_EVENTS];  // Adjust the array size as needed
    int num_events = epoll_wait(epoll_fd, events, sizeof(events) / sizeof(events[0]), timeout_ms);

    if (num_events > 0) 
    {
        for (int i = 0; i < num_events; ++i) 
        {
            if (events[i].data.fd == fd) 
            {
                // 新连接
                int client_fd = accept(fd, NULL, NULL);
                if (client_fd == -1) 
                {
                    perror("accept failed");
                } else {
                    // 设置 client_fd 为非阻塞
                    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);

                    // 添加 client_fd 到 epoll 实例
                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) 
                    {
                        perror("epoll_ctl failed");
                        return -1;
                    }

                    printf("New connection established. Client FD: %d\n", client_fd);
                }
            } 
            else if (events[i].events & EPOLLIN) 
            {
                // 已连接的 socket 有数据可读
                callback(events[i].data.fd);
            }
        }
        return 1;  // 有数据
    }
    return 0;  // 没有数据
}

int start_epoll_server(int port)
{
    int server_fd,epoll_fd,ret;
    struct sockaddr_in address;
    struct epoll_event event;

    //创建socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > server_fd)
    {
        perror("socket failed");
        return -1;
    }

    //设置地址结构
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    //绑定地址和端口
    ret = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(0 > ret)
    {
        perror("bind error");
        close(server_fd);
        return -1;
    }

    //监听
    ret = listen(server_fd, 50);
    if(0 > ret)
    {
        perror("listen error");
        close(server_fd);
        return -1;
    }

    //创建epoll实例
    epoll_fd = epoll_create1(0);
    if(-1 == epoll_fd)
    {
        perror("Epoll create error");
        return -1;
    }

    //设置server_fd为非阻塞
    fcntl(server_fd, F_SETFL,fcntl(server_fd, F_SETFL,0) | O_NONBLOCK);

    //添加server_fd到epoll实例
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD,server_fd,&event);
    if(-1 == ret)
    {
        perror("Epoll_ctl failed");
        return -1;
    }

    printf("Server linsten on port %d...\n", port);

    while (1)
    {
        epoll_receive_data(epoll_fd,server_fd,0,client_data);
    }

    close(server_fd);
    return 0;
    
}


void handle_client_data(evutil_socket_t client_fd, short events, void *arg) 
{
    char buffer[1024];
    ssize_t read_size = read(client_fd, buffer, sizeof(buffer));

    if (read_size <= 0) 
    {
        // 连接关闭或读取错误
        printf("Connection closed or read failed. Client_fd: %d\n", client_fd);
        close(client_fd);
    } 
    else 
    {
        buffer[read_size] = '\0';
        printf("Received data from client fd_%d: %s\n", client_fd, buffer);
    }
}

void handle_accept(evutil_socket_t listener, short events, void *arg) 
{
    struct event_base *base = (struct event_base *)arg;
    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(listener, (struct sockaddr *)&client_addr, &client_len);
    if (-1 == client_fd) 
    {
        perror("accept failed");
        return;
    }

    // 设置 client_fd 为非阻塞
    evutil_make_socket_nonblocking(client_fd);

    // 创建一个新的event用于监听client_fd的读事件
    struct event *client_event = event_new(base, client_fd, EV_READ | EV_PERSIST, handle_client_data, NULL);
    event_add(client_event, NULL);

    printf("New connection established. Client_fd: %d\n", client_fd);
}

int start_libevent_server(int port) 
{
    int ret;
    struct event_base *base = event_base_new();
    if (!base) 
    {
        fprintf(stderr, "Couldn't create event base\n");
        return -1;
    }

    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listener) 
    {
        perror("socket failed");
        return -1;
    }

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    ret = bind(listener, (struct sockaddr *)&sin, sizeof(sin));
    if ( 0 > ret) 
    {
        perror("bind error");
        close(listener);
        return -1;
    }
    ret = listen(listener, 16);
    if ( 0 > ret) 
    {
        perror("listen error");
        close(listener);
        return -1;
    }

    // 设置 listener 为非阻塞
    evutil_make_socket_nonblocking(listener);

    // 创建一个新的event用于监听listener的读事件（新连接）
    struct event *listener_event = event_new(base, listener, EV_READ | EV_PERSIST, handle_accept, (void *)base);
    event_add(listener_event, NULL);

    printf("Server listen on port %d...\n", port);

    // 进入事件循环，不再需要while(1)循环
    event_base_dispatch(base);
    //event_base_loop(base, EVLOOP_ONCE);

    // 清理资源
    event_base_free(base);
    close(listener);

    return 0;
}

void* listen_socket_data(void *arg)
{
    start_libevent_server(8888);
    return NULL;
}
