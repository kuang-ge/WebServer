#pragma once
#include <sys/epoll.h>
#include <functional>

//将一个文件描述符封装成一个Channel类，在类中对不同类型的描述符进行不同的处理
class EventLoop;
class Channel{
private:
	EventLoop* loop;

	int fd;//文件描述符
	uint32_t events;//事件类型
	uint32_t revents;//表示epoll返回该Channel时文件描述符正在发生的事件
	bool inEpoll;//表示当前Channel是否在eopll红黑树中
	
	std::function<void()> callback;

public:
	Channel(EventLoop* _loop,int _fd);
	~Channel();

	void handleEvent();	

	void enableReading();//希望监听该Channel发生的读事件
	int getFd();
	uint32_t getEvents();
	uint32_t getRevents();
	bool getInEpoll();
	void setInEpoll();
	
	void setRevents(uint32_t);

	void setCallback(std::function<void()>);

};







