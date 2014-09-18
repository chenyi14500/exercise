class epoll
{
public:
	epoll(int event_number, int timeout);
	int wait();
	int add(int fd);
	int remove(int fd);
	struct epoll_event * getEventByIndex(int index);
	int modify(int fd, int flags);
public:
	int epollfd;
	struct epoll_event *events;
	int timeout;
	int max_event_number;
	int current_event_number;
};
epoll::epoll(int event_number, int time):
	max_event_number(event_number), timeout(time) {
	epollfd = epoll_create(5);
	events = (struct epoll_event *) malloc( max_event_number * sizeof(struct epoll_event));
	if( events == NULL ) {
	};
};

int epoll::wait() {
	current_event_number = epoll_wait(epollfd, events, max_event_number, timeout);
	return current_event_number;
};

int epoll::add(int fd) {
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.data.ptr = NULL;
	ev.events = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
};

int epoll::remove(int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
};

int epoll::modify(int fd, int flags) {
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.data.ptr = NULL;
	ev.events = flags;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
};

struct epoll_event * epoll:getEventByIndex(int index) {
	if( index >= 0 && index < current_event_number ) {
		return &events[index];
	}
	return NULL;
};

epoll::~epoll() {
	free(events);
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


		
