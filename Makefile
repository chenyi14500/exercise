server : server.cpp
	g++ -Wall -o server server.cpp util.h request.h locker.h
client : client.cpp
	g++ -Wall -o client client.cpp
clear:
	rm -r client server
