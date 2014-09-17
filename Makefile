master : master.cpp
  g++ -Wall -o master master.cpp util.h
server: server.cpp
	g++ -Wall -o server server.cpp task.h locker.h threadpool.h util.h -lpthread
client : client.cpp
	g++ -Wall -o client client.cpp util.h
clean : 
  rm -rf server client
