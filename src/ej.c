/*
 * Elisa Wang and John Jiang(EJ) web lob server
 * author:johnjiang1985@gmail.com
 */


#include "ej.h"
#include "net.h"

int main(int argc, char *argv[])
{
    int32_t sockfd, epollfd;
    
    sockfd = create_server();
    epollfd = create_event(sockfd);
    add_event(sockfd, epollfd, EPOLLIN);
    handle_event(sockfd, epollfd);
    close(sockfd);
    close(epollfd);
    
    return 0;
}