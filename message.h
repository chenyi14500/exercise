#include <iostream>
#include "util.h"

#define CLIENT_IP_REQUEST 1
#define CLIENT_IP_RESPONSE 2
#define SERVER_IP_REQUEST 3
#define SERVER_IP_RESPONSE 4
#define CHAT_CONTENT_REQUEST 5

struct ip_port
{
	char * ip[32];
	int port;
	int clients_num;
};

int get_request_type( char * request_buffer );
struct ip_port get_ip_port_from_content( char *request_content );
int create_send_buffer( int type, char *content, int len );
int fill_ip_response( char *response_buffer, struct ip_port ipp );
int server_ip_response( char *response_buffer );
struct ip_port get_ip_port_from_server_vector( std::vector<struct ip_port> *s_vec );
