#include "net.h"
#include "config.h"

int32_t create_server()
{
    int32_t sockfd = -1, reuse;
    struct sockaddr_in addr = {0};
    
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sockfd)
    {
        printf("create server socket failed, %s, %d\n", __FILE__, __LINE__);
        exit(-1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    reuse = 1;
//     if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)))
//     {
//         printf("set socket reuse failed.\n");
//     }
    ioctl(sockfd, FIONBIO, &reuse);
    
    if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        printf("bind socket failed, %s, %d\n", __FILE__, __LINE__);
        exit(-1);
    }
    
    if (-1 == listen(sockfd, SERVER_LOGBACK))
    {
        printf("listen socket failed, %s, %d\n", __FILE__, __LINE__);
        exit(-1);
    }

    return sockfd;
}

int32_t create_event(int32_t sockfd)
{
    int32_t epollfd = -1;
    
    if (-1 == sockfd)
    {
        printf("invalid sockfd, %s, %d\n", __FILE__, __LINE__);
        exit(-1);
    }
    
    epollfd = epoll_create1(0);
    if (-1 == epollfd)
    {
        printf("create epollfd failed, %s, %d\n", __FILE__, __LINE__);
        exit(-1);
    }
    
    return epollfd;
}

int32_t add_event(int32_t sockfd, int32_t epollfd, uint32_t events)
{
    struct epoll_event event = {0};
    
    if (-1 == sockfd || -1 == epollfd)
    {
        printf("invalid sockfd or epollfd, %s, %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    event.data.fd = sockfd;
    event.events = events;
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event))
    {
        printf("add epoll event failed, %s, %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    return 0;
}

int32_t mod_event(int32_t sockfd, int32_t epollfd, uint32_t events)
{
    struct epoll_event event = {0};
    
    if (-1 == sockfd || -1 == epollfd)
    {
        printf("invalid sockfd or epollfd, %s, %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    event.data.fd = sockfd;
    event.events = events;
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &event))
    {
        printf("add epoll event failed, %s, %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    return 0;
}

int32_t del_event(int32_t sockfd, int32_t epollfd)
{
    struct epoll_event event = {0};
    
    if (-1 == sockfd || -1 == epollfd)
    {
        printf("invalid sockfd or epollfd, %s, %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    event.data.fd = sockfd;
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &event))
    {
        printf("add epoll event failed, %s, %d\n", __FILE__, __LINE__);
        return -1;
    }
    
    return 0;
}

int32_t handle_event(int32_t sockfd, int32_t epollfd)
{
    struct epoll_event events[SERVER_LOGBACK] = {0};
    int32_t epoll_num = SERVER_LOGBACK;
    int32_t timeout = SERVER_TIMEOUT;
    int32_t num, i, j;
    char buf[1024] = {0};
    int32_t err;

    while(1)
    {
        while(1)
        {
            num = epoll_wait(epollfd, events, epoll_num, timeout);
            if (num > 0)
                break;
        }
        
        for(i = 0; i < num; i++)
        {
            if (events[i].data.fd != sockfd || !(events[i].events & EPOLLIN))
            {
                continue;
            }
            
            // accept
            struct sockaddr_in addr = {0};
            int32_t http_sockfd/*, http_epollfd*/, http_num, http_epoll_num = 4, addr_len = sizeof(addr);
            struct epoll_event http_events[4] = {0};
            http_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addr_len);
            if (-1 == http_sockfd)
            {
                printf("accept failed, %s, %d\n", __FILE__, __LINE__);
                continue;
            }
//             http_epollfd = create_event(http_sockfd);
            add_event(http_sockfd, epollfd, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP);
            while(1)
            {
                while(1)
                {
                    http_num = epoll_wait(epollfd, http_events, http_epoll_num, timeout);
                    if (http_num > 0)
                        break;
                }
                for (j=0; j<http_num; j++)
                {
                    if (http_events[i].data.fd != http_sockfd/* || !(http_events[i].events & EPOLLIN)*/)
                    {
                        continue;
                    }
                    
                    if (http_events[i].events & EPOLLIN)
                    {
                        // recv
                        memset(buf, 0, sizeof(buf));
                        err = recv(http_sockfd, buf, sizeof(buf), 0);// MSG_WAITALL
                        if (err <= 0)
                        {
                            goto clear_http;
                        }
                        
                        printf("buf:\n%s\n", buf);
                        char *method = buf;
                        char *pos = strchr(buf, ' ');
                        if (NULL == pos)
                        {
                            goto clear_http;
                        }
                        *pos++ = 0;
                        printf("method:%s\n", method);
                        if (strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0)
                        {
                            printf("invalid method, use GET or POST\n");
                            goto clear_http;
                        }
                        
                        char *path = pos;
                        pos = strchr(pos, ' ');
                        if (NULL == pos)
                        {
                            goto clear_http;
                        }
                        *pos++ = 0;
                        printf("path:%s\n", path);
                        
                        char *protocol = pos;
                        pos = strstr(pos, "\r\n");
                        if (NULL == pos)
                        {
                            goto clear_http;
                        }
                        *pos++ = 0;
                        printf("protocol:%s\n", protocol);
                        continue;
                    }
                    else
                    {
                        // send
                        int len, send_len;
                        memset(buf, 0, sizeof(buf));
                        snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\nthe EJ web server test OK!\r\n\r\n");
                        len = strlen(buf);
                        send_len = send(http_sockfd, buf, len, 0);
                        if (send_len != len)
                        {
                            printf("send failed\n");
                            goto clear_http;
                        }
    //                     del_event(http_sockfd, epollfd);
    //                     close(http_sockfd);
    //                     close(http_epollfd);
                    }
                    
    clear_http:
                    del_event(http_sockfd, epollfd);
                    close(http_sockfd);
                } // for http
            }
        }
    }
    
    return 0;
}