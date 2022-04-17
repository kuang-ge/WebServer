#pragma once

#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll{
private:
	int epfd;//epoll描述符
	struct epoll_event *events;
public:
	Epoll();
	~Epoll();
	
	void addFd(int fd,uint32_t op);
	//std::vector<epoll_event> poll(int timeout=-1);
	void updateChannel(Channel*);
	std::vector<Channel*> poll(int timeout=-1);





};



