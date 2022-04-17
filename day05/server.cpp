#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024


void handleReadEvent(int);

int main(){

	Socket *serv_sock=new Socket();
	InetAddress *serv_addr=new InetAddress("127.0.0.1",8888);
	
	serv_sock->bind(serv_addr);
	serv_sock->listen();
	
	Epoll *ep=new Epoll();//IO复用 实现并发
	serv_sock->setnonblocking();//非阻塞socket
	//ep->addFd(serv_sock->getFd(),EPOLLIN|EPOLLET);
	Channel* servChannel=new Channel(ep,serv_sock->getFd());
	servChannel->enableReading();
	while(true){
		std::vector<Channel*> activeChannels=ep->poll();
		int nfds=activeChannels.size();
		for(int i=0;i<nfds;++i){
			int chfd=activeChannels[i]->getFd();
			if(chfd==serv_sock->getFd()){//事件所从属的是服务器socket文件描述符
				InetAddress *clnt_addr=new InetAddress();
				Socket* clnt_sock=new Socket(serv_sock->accept(clnt_addr));
				printf("new client fd %d IP:%s Port:%d\n",clnt_sock->getFd(),inet_ntoa(clnt_addr->addr.sin_addr),ntohs(clnt_addr->addr.sin_port));
				clnt_sock->setnonblocking();
				Channel *clntChannel = new Channel(ep,clnt_sock->getFd());
				clntChannel->enableReading();
			}
			else if(activeChannels[i]->getRevents()&EPOLLIN){
				handleReadEvent(activeChannels[i]->getFd());
			}
			else{
				printf("something else happened\n");
			}
		}
	}
	delete serv_sock;
	delete serv_addr;	
	return 0;
}

void handleReadEvent(int sockfd){
	char buf[READ_BUFFER];//定义缓冲区
	//读写
	while(true){
		bzero(&buf,sizeof(buf));//清空缓冲区
		ssize_t read_bytes=read(sockfd,buf,sizeof(buf));//从客户端socket读取到缓冲区，返回已读大小
		if(read_bytes>0){
			printf("message from client fd %d:%s\n",sockfd,buf);
			write(sockfd,buf,sizeof(buf));//将相同数据写到客户端
		}
		else if(read_bytes==0){//客户端断开连接 EOF
			printf("EOF client fd %d disconnected",sockfd);
			close(sockfd);
			break;
		}
		else if(read_bytes==-1&& errno == EINTR){//客户端正常中断、继续读取
			printf("continue reading");
			continue;
		}
		else if(read_bytes==-1&& ((errno == EAGAIN)||(errno==EWOULDBLOCK))){//非阻塞IO 表示数据读取完毕
			printf("finish reading once erro: %d\n",errno);
			break;
		}
	}
}







































