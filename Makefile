server : server.cpp
	g++ -Wall -o server server.cpp util.h request.h locker.h
client : client.cpp
	g++ -Wall -o client client.cpp 
clear:
	rm -r client server
server1: server1.cpp
	g++ -Wall -o server server1.cpp epoll_util.h
taskserver: taskserver.cpp
	g++ -Wall -o server taskserver.cpp task.h locker.h threadpool.h epollutil.h -lpthread
taskclient : taskclient.cpp
	g++ -Wall -o client taskclient.cpp task.h locker.h epollutil.h threadpool.h -lpthread
