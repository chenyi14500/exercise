#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>


using namespace std;


#define MAX_LEN 1024
#define MAX_BUFFER_LEN 1024


#define MSG_REQUEST_IP_TYPE 1
#define MSG_CHAT_CONTENT_TYPE 2
#define MSG_SUBSERVER_IP_TYPE 3
#define MSG_RESPONSE_IP_TYPE 4

class Request
{
public:
	Request(char *buffer, int len, int fd);
	void process();
public:
	int m_fd;
private:
	void displayMsg();
public:
	int type;
	char data[MAX_LEN];
	
};

Request::Request(char *buffer, int len, int fd): m_fd(fd) {
	if ( buffer == NULL || len <= 0 ) return ;
	type = (int)buffer[0];

	memset(data, 0, MAX_LEN );
	memcpy(data, buffer+1, len-1);
}

void Request::displayMsg() {
	cout << "fd( " << m_fd << " ) : " << data << endl;
}
void Request::process()
{
	displayMsg();

}

class Role
{
public:
	Role();
	void readFrom(int fd);
	void writeTo(int fd);
public:
	char  buffer[MAX_BUFFER_LEN];
	char *buffer_ptr;
};
Role::Role()
{
	memset(buffer, 0, MAX_BUFFER_LEN);
	buffer_ptr = buffer;
}
void Role::readFrom(int fd)
{
	int  len = MAX_BUFFER_LEN - (buffer_ptr - buffer);
	memset(buffer_ptr, 0, len);
	int ret = recv(fd, buffer_ptr, len, 0);
	if( ret < 0 ) {
		cout << "read fd( " << fd << " ) msg failed " << endl;
	}
}
void Role::writeTo(int fd)
{
	int ret = send(fd, buffer_ptr, strlen(buffer_ptr), 0);
	if( ret < len ) {
		cout << "send fd( " << fd << " ) msg failed " << endl;
	}
}
class Talker : public  Role
{
public:
	void sendChatMsg(char *chatContent, int len);
	void displayChatMsg();
public:
	int chatroomfd;
};
void Talker::sendChatMsg()
{
	buffer_ptr = buffer+1;
	readFrom(0);
	buffer[0] = MSG_CHAT_CONTENT_TYPE;
	buffer_ptr = buffer;
	writeTo(chatroomfd);
};

class ChatRoom : public Role
{
public:
	void connMaster();
	void broadcastMsg();
	void receiveMsg(int fd);
	void receiveIP();
	int connChatRoom(char *ip, int port);
public:
	vector<int> talkerfd;
	queue<string> msgQueue;
	int masterfd;
};
void ChatRoom::receiveMsg(int fd)
{
	buffer_ptr = buffer;
	readFrom(fd);
	string msg(buffer);
	msgQueue.push_back(msg);
}
void ChatRoom::broadcastMsg()
{
	while(msgQueue.size() > 0) {
		string msg = msgQueue.front();
		buffer_ptr = msg.data();
		for(int i = 0; i < (int)talkerfd.size(); i++ ) {
			int fd = talkerfd.at(i);
			writeTo(fd);
		}
		msgQueue.pop();
	}
}

class Master
{
public:
	void sendIP(int fd);
	void handleRequest(char *buffer, int fd);
}