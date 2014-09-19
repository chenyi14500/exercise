#define SEND_ALL_TASK 1
#define SEND_MSG_TASK 2
class task
{
public:
	task(fdset *s, std::vector<string> *msg_vec, int msg_index, threadpool<task> *task_pool);
	task(int sendfd, string send_data);
	void process();
	
private:
	int type;
	fdset *set;
	std::vector<string> *data_vec;
	int data_index;
	threadpool<task> *pool;
	int fd;
	string data;
}
task::task(fdset *s, std::vector<string> *msg_vec, int msg_index, threadpool<task> *task_pool)
	: type(task_type), set(s), data_vec(msg_vec), data_index(msg_index), pool(task_pool) {
	type = SEND_ALL_TASK;
};
task::task(int sendfd, string send_data) : fd(sendfd), data(send_data) {
	type = SEND_MSG_TASK;
};
void task::process() {
	switch( type ) {
		case SEND_ALL_TASK: 
			for(int i = 0; i < set.size(); i++) {
				int f = set.getFdByIndex(i);
				task *new_task = new task(f, set, data_vec, data_index, p);
				new_task->pool->append(new_task);
			}
			break;
		case SEND_MSG_TASK:
			int rc = write(fd, data.data(), data.length());
			if(rc < str.length()) {
			};
			break;
		default:
			break;
	}
}

	