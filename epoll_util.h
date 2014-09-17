#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EVENT_NUMBER 1024
#define MAX_BUFFER_LEN 1024
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



