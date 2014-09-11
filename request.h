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



#define MSG_REQUEST_IP_TYPE 1
#define MSG_CHAT_CONTENT_TYPE 2
#define MSG_SUBSERVER_IP_TYPE 3
#define MSG_RESPONSE_IP_TYPE 4

struct message
{
	int type;
	char data[MAX_LEN];
};


char * msgToStr(struct message msg) {
	char data[MAX_LEN + 8];
	memset(data, 0, MAX_LEN + 8);
	sprintf(data, "%c%s", msg.type, msg.data);
	return data;
}

struct message strToMsg(char *data) {
	struct message msg;
	memset(&msg, 0, sizeof(msg));
	msg.type = (int)data[0];
	memcpy(msg.data, data + 1, strlen(data) - 1);
	return msg;
}
struct message createMsg(int type, char *data)
{
	struct message msg;
	msg.type = type;
	memcpy(msg.data, data, strlen(data));
	return msg;
}


class Request
{
public:
	Request(struct message msg, int fd);
	struct message getMsg();
	bool isChatContentRequest();
	void process();

public:
	int m_fd;
private:
	void handleRequestIP();
	void handleChatContent();	
	void handleSubServerIP();
	void handleResponseIP();
private:
	struct message m_msg;
	
};

Request::Request(struct message msg, int fd) : 
			m_msg(msg), m_fd(fd){

}
struct message Request::getMsg() {
	return m_msg;
}
void Request::handleChatContent() {
	cout << "content: " << m_msg.data << endl;
}

bool Request::isChatContentRequest()
{
	return (m_msg.type == MSG_CHAT_CONTENT_TYPE) ;
}
void Request::handleSubServerIP()
{

}
void Request::handleResponseIP()
{

}
void Request::handleRequestIP()
{

}
void Request::process()
{
	cout << "msg type: " << m_msg.type << endl;
	switch( m_msg.type ) {
		
		case MSG_CHAT_CONTENT_TYPE:
			handleChatContent();
			break;
		case MSG_REQUEST_IP_TYPE:
			handleRequestIP();

		case MSG_RESPONSE_IP_TYPE:
			handleResponseIP();

		case MSG_SUBSERVER_IP_TYPE:
			handleSubServerIP();

		default:
			cout<< "process other handle !" << endl;
			break;
	};
}