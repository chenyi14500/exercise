#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include "locker.h"


#define MAX_LEN 1024

class datastore : public locker
{
public:
	bool add(std::string str);
public:
	std::vector<std::string> m_store;
};

bool datastore::add(std::string data)
{
	std::string str(data);
	lock();
	m_store.push_back(str);
	unlock();
	return true;
};


class request
{
public:
	request(int fd, datastore *store);
	bool read();
	bool write();
	void process();
private:
	int m_fd;
	int m_index;
	datastore *m_datastore;
	std::string m_data;
};

request::request(int fd, datastore *store):
		m_fd(fd), m_datastore(NULL)
{
	m_datastore = store;
	if( m_datastore != NULL )
	{
		m_datastore->lock();
		m_index = m_datastore->m_store.size() - 1;
		m_datastore->unlock();
	}
}

bool request::read()
{
	char data[MAX_LEN];
	memset(data, 0, sizeof(MAX_LEN));

	//int ret = recv(m_fd, data, MAX_LEN-1, 0 );
	int ret = read(m_fd, data, 0);
	if(ret > 0)
	{
		printf("fd=%d, recv: %s\n", m_fd, data);
		std::string str(data);
		m_data = str;
		return m_datastore->add(str);
	}
	return false;
}

bool request::write()
{
	std::string str;
	m_datastore->lock();
	
	int i = m_index;
	//printf("fd( %d ): write.... index = %d, store.size=%d\n", m_fd, m_index, m_datastore->m_store.size());
	while( i < m_datastore->m_store.size())
	{
		str = m_datastore->m_store.at(i);
		//int ret = send(m_fd, (char *)str.data(), str.length(), 0);
		int ret = write(m_fd, (char *)str.data(), 0);
		if(ret < 0 )
		{
			m_datastore->unlock();
			printf( "fd(%d) send %ith data failed !\n", m_fd, i );
			m_index = i;
			return false;
		}
		printf("fd=%d, send: %s\n", m_fd, (char *)str.data());
		i++;	
	}
	m_index = m_datastore->m_store.size();
	m_datastore->unlock();
	return true;

}

void request::process()
{

	printf("%s\n", (char *)m_data.data());
}