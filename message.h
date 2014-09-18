class messge
{
public:
	message(int type, char *content);
	message(char *buffer);
	int fill(char *content);
	void setType(int type);
	int getType();
	char * getContent();
public:
	char buffer[MAX_BUFFER_LEN];
	int type;
};

class IPMessage : public message
{
public:
	char * getIP();
	int getPort();
	void set(char *ip, int port);
public:
	char ip_addr[32];
	int port;
};
	

