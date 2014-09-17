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

#include "util.h"
char buffer[MAX_BUFFER_LEN] = { 0 };

#define SERVER_IP_REQUEST 1
#define SERVER_IP_RESPONSE 2

int main( int argc, char* argv[] )
{
    if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );
	  char server_ip[32] = { 0 };
	  int server_port;
    int sockfd = conn_server(ip, port);
	  if(sockfd < 0) {
		  cout << "connect master server failed !" << endl;
		  return 0;
	  }

	  while( 1 ) {
      memset(buffer, 0, MAX_BUFFER_LEN);
      buffer[0] = (char)SERVER_IP_REQUEST;
      int ret = write( sockfd, buffer, 1);
      if( ret < 0 ){
        continue;
		  }
      ret = read( sockfd, buffer, MAX_BUFFER_LEN);
      if( ret <= 0 ) {
        continue;
      }
      if( get_msg_type(buffer) != SERVER_IP_RESPONSE ) {
        continue ;
      }
      sscanf((buffer+1), "%d %s", &server_port, server_ip);
      cout << "server ip:" << server_ip << " , server port : " << server_port << endl;
      break;
	  }
	
    sockfd = conn_server( server_ip, server_port);
    if( sockfd < 0 ) {
      cout << "connect chat room server failed !" << endl;
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
            memset(buffer, 0, MAX_BUFFER_LEN);
            char *buffer_p;
            if( events[i].data.fd == 0 ) {
              buffer_p = buffer+1;
              if( epoll_read(0, buffer_p, MAX_BUFFER_LEN-1) <= 0) {
                continue;
              }
              buffer[0] = SEND_MSG_TASK_TYPE;
              if( epoll_write(sockfd, buffer, strlen(buffer)) <= 0) {
                continue;
              }
            } else if ( events[i].data.fd == sockfd) {
              buffer_p = buffer;
              if( epoll_read(sockfd, buffer_p, MAX_BUFFER_LEN) <= 0) {
                continue;
              }
              buffer_p = buffer+1;
              if( epoll_write(0, buffer, strlen(buffer)) <= 0) {
                continue;
              }
            } else {  
			      }
		}

    close( sockfd );
    return 0;
}
