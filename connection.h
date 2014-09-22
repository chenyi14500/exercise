class connection
{
public:
	int connectConnection(char *ip, int port);
	int acceptConnection(int listenfd);
  int buildServer(char *ip, int port);
  void initAddress(char *ip, int port);
public:
   int sockfd;
   struct sockaddr_in address;
   socklen_t socklen;
};
int connection::connectServer(char *ip, int port) {
    initAddress(ip, port);
    return connect(sockfd, (struct sockaddr *)&address, &socklen);
}

int connection::acceptConnection() {
   int conn = accpet(sockfd, &address, &socklen); 
   return conn;
}

void connection::initAddress(char *ip, int port) {
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
}

int connection::buildServer(char *ip, int port) {
    int ret = 0;
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    
    initAddress(ip, port);
  
    ret = bind( sockfd, ( struct sockaddr * )&address, sizeof( address ) );
    assert( ret >= 0 );

    ret = listen( sockfd, 5 );
    assert( ret >= 0 );
  
    return sockfd;
}