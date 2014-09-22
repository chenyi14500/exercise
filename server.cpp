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
#include "epollutil.h"
#include "task.h"
using namespace std;

int main(int argc, char *argv[])
{
	if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );

    int listenfd = connection::buildServer(ip, port);
    epoll server_epoll(MAX_EVENT_NUMBER, 3000);
    server_epoll.add(listenfd);
  

    struct epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5); 
    assert( epollfd != -1 );
    epoll_add( epollfd, listenfd);

    cout << "listenfd = " << listenfd << endl;

    Threadpool<Task> pool(2, 1000);
    std::vector<int> fdv;
    locker fdvlocker;

    char data[MAX_BUFFER_LEN];

    Task *basetask = new Task(NULL);
    basetask->m_pool = &pool;
    basetask->m_fdvlocker = &fdvlocker;
    basetask->m_fdv = &fdv;

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
                    fdvlocker.lock();
                    fdv.push_back(connfd);
                    fdvlocker.unlock();
                }
            } else {
                //print_event(events[i]);

                if(events[i].events & EPOLLIN) {
                    memset(data, 0, MAX_BUFFER_LEN);
                    int rc = epoll_read(events[i].data.fd, data, MAX_BUFFER_LEN);
                    if( rc <= 0 ) continue ;
                    Task *task = new Task(basetask);
                    task->m_type = DISPLAY_MSG_TASK_TYPE;
                    task->m_data = string(data);
                    task->m_pool->append(task);
                    task->m_sendfd = events[i].data.fd;
                }
            }
    		
    	}
    }


}
