server : server.cpp
	g++ -Wall -o server server.cpp util.h request.h locker.h
client : client.cpp
	g++ -Wall -o client client.cpp request.h
clear:
	rm -r client server
server1: server1.cpp
	g++ -Wall -o server server1.cpp epoll_util.h
