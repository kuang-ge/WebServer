#include "Epoll.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

Epoll::Epoll():epfd(-1),events(nullptr){
	epfd=epoll_create1(0);//创建描述符 标识内核表
	errif(epfd==-1,"epoll create error");
	
	events = new epoll_event[MAX_EVENTS];//动态申请内存存放epoll_event指针
	bzero(events,sizeof(*events)* MAX_EVENTS);
}

Epoll::~Epoll(){
	if(epfd!=-1){
		close(epfd);
		epfd=-1;
	}
	delete[] events;
}

//void Epoll::addFd(int fd,uint32_t op){
//	struct epoll_event ev;//内核事件
//	bzero(&ev,sizeof(ev));
//	ev.data.fd=fd;//指定事件所从属的目标文件描述符
//	ev.events=op;//操作类型
//	errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll and event add error");//epoll_ctl()是用来操作内核事件表的 这里是增加操作
//}

//std::vector<epoll_event> Epoll::poll(int timeout){
//	std::vector<epoll_event> activeEvents;
//	int nfds=epoll_wait(epfd,events,MAX_EVENTS,timeout);//检测等待事件，从内核表中放到events
//	errif(nfds==-1,"epoll wait error");
//	for(int i=0;i<nfds;i++){
//		activeEvents.push_back(events[i]);
//	}
//	return activeEvents;
//}

//从内核表中得到事件
std::vector<Channel*> Epoll::poll(int timeout){
	std::vector<Channel*> activeChannels;
	int nfds=epoll_wait(epfd,events,MAX_EVENTS,timeout);//检测等待事件，从内核表中放到events
        errif(nfds==-1,"epoll wait error");
        for(int i=0;i<nfds;i++){
		Channel* ch=(Channel*)events[i].data.ptr;
                ch->setRevents(events[i].events);
	 	activeChannels.push_back(ch);
        }
        return activeChannels;

}

//更新 其实就是更新内核表的事件
void Epoll::updateChannel(Channel* ch){
	int fd=ch->getFd();
	struct epoll_event ev;
	bzero(&ev,sizeof(ev));
	ev.data.ptr=ch;
	ev.events=ch->getEvents();
	if(!ch->getInEpoll()){
		errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll add error");
		ch->setInEpoll();
	}
	else {
		errif(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)==-1,"epoll modify error");
	}
}
















