#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <vector>
#include "util.h"
#include "request.h"

void req_modfd( int epollfd, int fd, int ev, request *req)
{
    epoll_event event;
    event.data.fd = fd;
    event.data.ptr = (void *)req;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
}

int main(int argc, char *argv[])
{
	if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    struct linger tmp = {1, 0};
    setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    ret = bind( listenfd, ( struct sockaddr * )&address, sizeof( address ) );
    assert( ret >= 0 );

    ret = listen( listenfd, 5 );
    assert( ret >= 0 );

    struct epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5); 
    assert( epollfd != -1 );
    addfd( epollfd, listenfd, false);

    datastore store;
    std::vector<int> fdvector;

    while(true)
    {
        sleep(3);
    	int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, 0);
        //printf("number = %d\n", number);
    	if( ( number < 0 ) && ( errno != EINTR ) )
    	{
    		printf( "epoll failure ! \n" );
    		break;
    	}

    	for( int i = 0; i < number ; i++ )
    	{
    		int sockfd = events[i].data.fd;
    		if( sockfd == listenfd )
    		{
    			struct sockaddr_in client_addr;
    			socklen_t client_addr_len = sizeof( client_addr_len );
    			int connfd = accept( listenfd, ( struct sockaddr * )&client_addr, &client_addr_len );
    			if( connfd <  0 )
    			{
    				printf( "accept error is : %d \n", errno );
    				continue;
    			}

    			struct epoll_event ev;
    			ev.events = EPOLLIN;//| EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
    			ev.data.fd = connfd;

    			request req( connfd, &store );
    			ev.data.ptr = ( void * ) &req;

    			epoll_ctl( epollfd, EPOLL_CTL_ADD, connfd, &ev);
    			//setnonblocking( connfd );

    			//continue;
                
    		}
    		else if ( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) )
    		{
    			//continue;
    		} 
    		else if ( events[i].events & EPOLLIN )
    		{
                printf("server epoll in, fd=%d\n", events[i].data.fd);
    			request *req = ( request * )events[i].data.ptr;
    			if( req != NULL )
    			{
    				if( req->getReceiveData() )
    				{
    					req->process();
    				}
    			}
                req_modfd(epollfd, events[i].data.fd, EPOLLOUT, req);

    		} 
            else if ( events[i].events & EPOLLOUT )
    		{
                printf("server epoll out, fd=%d\n", events[i].data.fd);
    			request *req = ( request * )events[i].data.ptr;
    			if(req->sendResponseData()) {

                }
                req_modfd(epollfd, events[i].data.fd, EPOLLIN, req);
    		} else {

            }
    	}
    }


}
