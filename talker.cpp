#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "epoll_util.h"

#include "request.h"

void handle_msg(int srcfd, int dstfd)
{
    char data[MAX_LEN] = { 0 };
    char *data_p = NULL;
    int len = 0;

    len = epoll_read(srcfd, data+1, MAX_LEN);
    if( len > 0 ) {
        if( srcfd == 0 ) {
            data[0] = ( char ) MSG_CHAT_CONTENT_TYPE;
            len += 1;
            data_p = data;

        } else {
            data_p = data+2;
            len  = len -1;
        }
        if( epoll_write(dstfd, data_p, len) == 0) {
            cout << "write failue " << endl;
        }
    }
}

int main( int argc, char* argv[] )
{
    if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );

    struct sockaddr_in server_address;
    bzero( &server_address, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &server_address.sin_addr );
    server_address.sin_port = htons( port );

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( sockfd >= 0 );
    if ( connect( sockfd, ( struct sockaddr* )&server_address, sizeof( server_address ) ) < 0 )
    {
        printf( "connection failed\n" );
        close( sockfd );
        return 1;
    }

    struct epoll_event events[2];
    int epollfd = epoll_create(5);

    epoll_add(epollfd, sockfd);
    epoll_add(epollfd, 0);
    while( true ) {
        int ret = epoll_wait(epollfd, events, 2, 3000);

        if( ret <= 0 ) {
            //printf(" time out !\n");
            continue;
        }

        if ( ret < 0 ) {
            printf("epoll failure !\n");
            break;
        }

        for(int i = 0; i < ret; i++ ) {
            if(events[i].data.fd == 0 ) {
                handle_msg(0, sockfd);
            }
            if(events[i].data.fd == sockfd ) {
                handle_msg(sockfd, 1);
            }
        }
    }

    close( sockfd );
    return 0;
}