class epoll
{
public:
	int wait();
	int add(int fd);
	int remove(int fd);
	struct epoll_event * getEvent(int index);
	int modify(int fd, int flags);
	void setTimeout(int time);
public:
	int epollfd;
	struct epoll_event *events;
	int timeout;
};

class fdset
{
public:
	void add(int fd);
	void remove(int fd);
	int size();
	int getfd(int index);
private:
	std::vector<int> set;
	locker setlocker;
};
		
