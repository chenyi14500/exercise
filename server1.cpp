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
#include <queue>
#include <iostream>
#include "epoll_util.h"
#include "request.h"
using namespace std;


/*void send_all_client_msg(int fd, char *data, int len, vector<int> v)
{
    printf("send data from fd(%d), data length = %d, client num = %d\n",
            fd, len, v.size());
    for ( int i = 0; i < (int)v.size(); i++ ) {
        int sendfd = v.at(i);
        if( sendfd == fd) continue;
        int rc = write(sendfd, data, len);
        printf("send fd(%d), data length = %d\n", sendfd, rc); 
    }
}*/

void print_event(struct epoll_event ev)
{
    int event_types[5] = { EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLERR, EPOLLET};
    char event_name[5][32] = { "EPOLLIN", "EPOLLOUT", "EPOLLRDHUP", "EPOLLERR", "EPOLLET"};
    printf("fd = %d, ", ev.data.fd);
    for(int i = 0; i < (int)sizeof(event_types); i++ ) {
        if( ev.events & event_types[i] ) {
            cout << " " << event_name[i] ;    
        } 
    }
    cout << endl;
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
    epoll_add( epollfd, listenfd);

    cout << "listenfd = " << listenfd << endl;

    char data[1024];
    std::vector<int> fdv;
    std::queue<Request> reqv;
    
    while(true)
    {

    	int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, 3000);
        if( number == 0 ) {
            //cout << "time out !" << endl;
            continue;
        }
    	if( ( number < 0 ) && ( errno != EINTR ) )
    	{
    		cout << "epoll failure ! " << endl;
    		break;
    	}

    	for( int i = 0; i < number ; i++ )
    	{
            if( events[i].data.fd <= 0 ) {
                continue ;
            } else if ( events[i].data.fd == listenfd ) {
                int connfd = accept_client(epollfd, listenfd);
                if( connfd > 0 ) {
                    fdv.push_back(connfd);
                }
            } else {
                print_event(events[i]);

                if(events[i].events & EPOLLIN) {
                    memset(data, 0, MAX_LEN);
                    int rc = epoll_read(events[i].data.fd, data, MAX_LEN);
                    if( rc <= 0 ) continue ;

                    Request req(data, rc, events[i].data.fd);
                    req.process();
                    reqv.push(req);
                    //send_all_client_msg(events[i].data.fd, data, rc, v);

                }
            }
    		
    	}
        while(reqv.size() > 0 ) {
            Request r = reqv.front();
            for( int j = 0; j <  (int)fdv.size(); j++ ) {
                int fd = fdv.at(j);
                if( r.m_fd == fd) continue;
                int ret = epoll_write(fd, r.data, strlen(r.data));
            }
            reqv.pop();
        }
    }


}
