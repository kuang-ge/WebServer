#pragma once
#include <sys/epoll.h>

//将一个文件描述符封装成一个Channel类，在类中对不同类型的描述符进行不同的处理
class Epoll;
class Channel{
private:
	Epoll *ep;//Epoll类指针
	int fd;//文件描述符
	uint32_t events;//事件类型
	uint32_t revents;//表示epoll返回该Channel时文件描述符正在发生的事件
	bool inEpoll;//表示当前Channel是否在eopll红黑树中
public:
	Channel(Epoll* ep,int _fd);
	~Channel();
	
	void enableReading();//希望监听该Channel发生的读事件
	int getFd();
	uint32_t getEvents();
	uint32_t getRevents();
	bool getInEpoll();
	void setInEpoll();
	
	void setRevents(uint32_t);

};







