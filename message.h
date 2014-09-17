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

// server ip, server port, client number 
struct s_ipp
{
	struct ip_port ipp;
	int client_num;
};

int get_request_type( char * request_buffer ) {
	if( request_buffer == NULL ) return -1;
	return (int)request_buffer[0];
}

struct ip_port get_ip_port_from_content( char *request_content ) {
	struct ip_port ipp;
	sscanf(requst_buffer, "%d %s", &ipp.port, ipp.ip);
	std::cout << "ip: " << ipp.ip << ", port : " << ipp.port << endl;
	return ipp;
}

int fill_client_ip_response( char *response_buffer, struct ip_port ipp ) {
	memset(response_buffer, 0, MAX_BUFFER_LEN);
	char *response_content = response_buffer + 1;
	int len = sprintf(response_content, "%d %s", ipp.port, ipp.ip);
	response_buffer[0] = (char)CLIENT_IP_RESPONSE;
	return len + 1;
}
	
int fill_server_ip_response( char *response_buffer, std::vector<struct ip_port> *s_vec) {
	memset(response_buffer, 0, MAX_BUFFER_LEN);
	char *response_content = response_buffer + 1;
	
	
struct ip_port get_ip_port_from_server_vector( std::vector<struct ip_port> *s_vec );
