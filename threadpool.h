#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"

template <typename T>
class Threadpool
{
public:
	Threadpool(int thread_number = 8, int max_request = 10000);
	~Threadpool();
	bool append(T* request);

private:
	static void * worker(void * arg);
	void run();

private:
	int m_thread_number;
	int m_max_requests;
	pthread_t *m_threads;
	std::list<T*> m_workquque;
	locker m_queuelocker;
	sem m_queuestat;
	bool m_stop;
};

template< typename T>
Threadpool<T> :: Threadpool(int thread_number, int max_request):
			m_thread_number(thread_number), m_max_requests(max_request),
			m_stop(false), m_threads(NULL)
{
	if((m_thread_number <= 0) || (m_max_requests <= 0))
	{
		throw std::exception();
	}

	m_threads = new pthread_t[m_thread_number];

	if(!m_threads)
	{
		throw std::exception();
	}

	for(int i = 0; i < m_thread_number; ++i) 
	{
		printf("create the %dth thread\n", i);
		if(pthread_create(m_threads + i, NULL, worker, this) != 0)
		{
			delete [] m_threads;
			throw std::exception();
		}
		if(pthread_detach(m_threads[i]))
		{
			delete [] m_threads;
			throw std::exception();
		}
	}
}

template<typename T>
Threadpool< T > :: ~Threadpool()
{
	delete [] m_threads;
	m_stop = true;
}

template< typename T >
bool Threadpool< T >:: append(T * request)
{
	m_queuelocker.lock();
	if(m_workquque.size() > m_max_requests )
	{
		m_queuelocker.unlock();
		return false;
	}

	m_workquque.push_back(request);
	m_queuelocker.unlock();
	m_queuestat.post();
	return true;
}

template < typename T >
void * Threadpool< T >::worker(void * arg)
{
	Threadpool *pool = (Threadpool *)arg;
	pool->run();
	return pool;
}

template< typename T >
void Threadpool< T > ::run()
{
	while( !m_stop )
	{
		m_queuestat.wait();
		m_queuelocker.lock();
		if(m_workquque.empty()) {
			m_queuelocker.unlock();
			continue;
		}

		T * request = m_workquque.front();
		m_workquque.pop_front();
		m_queuelocker.unlock();
		if(request == NULL) 
		{
			continue;
		}
		request->process();
	}
}