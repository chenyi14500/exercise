#include <iostream>
#include <string>
#include "threadpool.h"
#include <vector>
using namespace std;

#define INVAILD_TASK_TYPE -1
#define DISPLAY_MSG_TASK_TYPE 1
#define SEND_MSG_TASK_TYPE 2
#define BAORDCAST_MSG_TASK_TYPE 3
#define NEW_MSG_TASK_TYPE 4


class Task
{
public:
	Task(int tasktype, int msg_fd, int index);
	void sendmsg();
	void boardcast();
	void display();
	void conn();
	void process();
	string getmsg();
public:
	int type;
	int msg_index;
	int fd;
	resource *res;
};
typedef struct resource
{
	std::vector<int> *fdv;
	locker *fdvlocker;
	Threadpool<Task> *pool;
	int epollfd;
} resource;
Task::conn()
{	
	string str = getmsg();
	char *p = str.data() + 1;
	char ip[32] = { 0 };
	int port;
	sscanf(p, "%d %s", &port, ip);
	struct sockaddr_in server_address;
    bzero( &server_address, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &server_address.sin_addr );
    server_address.sin_port = htons( port );

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( sockfd >= 0 );
    if ( connect( sockfd, ( struct sockaddr* )&server_address, sizeof( server_address ) ) < 0 )
    {
        cout << "connect ip ( " << ip << " ) failed" << endl;
        close( sockfd );
    } else {
    	epoll_add(res->epollfd, sockfd);
    }
}
Taspk::Task(int tasktype, int msg_fd, int index):
	type(tasktype), fd(msg_fd), msg_index(index){

}
string Task::getmsg()
{
	std::vector<int> v = *(res->fdv);
	return v.at(msg_index);
}
void Task::sendmsg()
{
	string str = getmsg();
	cout << "send msg:" << str;
	int ret = write(m_sendfd, str.data(), str.length());
	if(ret < (int)str.length()) {
		cout << "send fd:" << fd << " failed" << endl; 
	}
}
void Task::boardcast()
{
	cout << "barodcast msg:" << getmsg();
	res->fdvlocker->lock();
	for( int i = 0 ; i < (int)(*(res->fdv)).size(); i++ ) {
		if( (*(res->fdv)).at(i) != fd ) {
			Task *newtask = new Task(SEND_MSG_TASK_TYPE, (*(res->fdv)).at(i), index);
			newtask->m_pool->append(newtask);
		}
	}
	res->fdvlocker->unlock();
}

void Task::display()
{
	cout << "display msg: " << getmsg();
}

void Task::process()
{
	if( m_type == INVAILD_TASK_TYPE ) {
		return ;
	}
	cout << endl;
	switch( m_type ) {
		case DISPLAY_MSG_TASK_TYPE:
			display(); 
			break;
		case SEND_MSG_TASK_TYPE:
			sendmsg();
			break;
		case BAORDCAST_MSG_TASK_TYPE:
			boardcast();
		default:
			break;

	}
}