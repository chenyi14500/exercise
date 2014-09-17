#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_EVENT_NUMBER 1024
#define MAX_BUFFER_LEN 1024
void print_event(struct epoll_event ev)
{
    int event_types[5] = { EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLERR, EPOLLET};
    char event_name[5][32] = { "EPOLLIN", "EPOLLOUT", "EPOLLRDHUP", "EPOLLERR", "EPOLLET"};
    for(int i = 0; i < (int)sizeof(event_types); i++ ) {
        if( ev.events & event_types[i] ) {
            printf("fd: %d, event: %s \n", ev.data.fd, event_name[i]);   
        } 
    }
}
int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

int epoll_write(int fd, char *buffer, int len)
{
    int ret = write(fd, buffer, len);
    return (ret < len ) ? 0 : ret;
}

int epoll_read(int fd, char *buffer, int len)
{
    return read( fd, buffer, len);
}

void epoll_add(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;// | EPOLLRDHUP | EPOLLONESHOT;
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

void epoll_mod(int epollfd, int fd, int ev, void * ptr)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.data.ptr = ptr;
    event.events = ev;// | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
}

void epoll_remove(int epollfd, int fd)
{
    epoll_ctl( epollfd, EPOLL_CTL_DEL, fd, 0 );
    close( fd );
}

int accept_client(int epollfd, int listenfd )
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof( client_addr_len );
    int connfd = accept( listenfd, ( struct sockaddr * )&client_addr, &client_addr_len );
    if( connfd <= 0 ) {
        printf("accept new client failed!\n");
    } else {
        printf("accept new client , fd = %d\n", connfd);
        epoll_add(epollfd, connfd);
    }
    return connfd;
}

