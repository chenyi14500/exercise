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
using namespace std;

#include "message.h"
#include "util.h"

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
            } else {
                if(events[i].events & EPOLLIN) {
			int fd = events[i].data.fd;
			memset(buffer, 0, MAX_BUFFER_LEN);
			int len = epoll_read(fd, buffer, MAX_BUFFER_LEN);
			int type = get_request_type(buffer);
			if( type == CLIENT_IP_REQUEST ) {
				struct ip_port ipp = get_ip_port_from_server_vector( &s_vec );
				len = fill_client_ip_response(buffer, ipp);
			} else if ( type == SERVER_IP_EWQUEST ) {
				struct ip_port ipp = get_ip_port_from_content( buffer );
				s_vec.push_back( ipp );
				len = fill_server_ip_response(buffer, &s_vec );
			} else {
				cout << "unknoew request type !" << endl;
				continue;
			}
			epoll_write(fd, buffer, len );
		}
            }
    		
    	}
    }


}
