#include <sys/socket.h>
#include <arpa/inet.h>//该头文件已经包含<netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "util.h"//封装的错误处理函数的头文件
#include <unistd.h>//使用其中的read和write进行数据读写

#include <fcntl.h>//ET模式 要搭配非阻塞socket
#include <errno.h>
#include <sys/epoll.h>


#define MAX_EVENTS 1024
#define READ_BUFFER 1024

//非阻塞socket设置
void setnonblocking (int fd){
	fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}



int main(){

	int sockfd=socket(AF_INET,SOCK_STREAM,0);//建立一个socket套接字，对外提供一个通信接口
	errif(sockfd==-1,"socket creat error");
	//创建一个结构体 要将套接字绑定到IP地址和端口
	struct sockaddr_in serv_addr;//sockaddr_in为专用socket地址结构体
	bzero(&serv_addr,sizeof(serv_addr));//初始化

	//设置地址组、IP地址、端口
	serv_addr.sin_family=AF_INET;//TCP/IPv4协议族
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");//IP地址转换函数 转换成网络字节序表示的IP地址
	serv_addr.sin_port=htons(8888);//同上

	//将socket地址与文件描述符绑定
	errif(bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr))==-1,"socket bind error");

	//监听这个socket端口
	errif(listen(sockfd,SOMAXCONN)==-1,"socket listen error");
	
	//创建一个epoll描述符 参数默认为0
	int epfd=epoll_create1(0);
	errif(epfd==-1,"epoll create error");
	
	//结构体 定义了一个结构体数组和一个结构体
	struct epoll_event events[MAX_EVENTS], ev;
	bzero(&events,sizeof(events));//初始化
	bzero(&ev,sizeof(ev));
	
	ev.data.fd=sockfd;//服务器IO口
	ev.events=EPOLLIN|EPOLLET;//支持的事件类型 数据可读
	setnonblocking(sockfd);
	epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);//将服务器socket fd添加到epoll

	while(true){
		int nfds = epoll_wait(epfd,events,MAX_EVENTS,-1);//有nfds个fd发生事件
		errif(nfds==-1,"epoll wait error");
		
		//处理
		for(int i=0;i<nfds;++i){//处理这些事件
			if(events[i].data.fd==sockfd){//发生事件的fd是服务器的fd，表示有新客户连接
				struct sockaddr_in clnt_addr;
				bzero(&clnt_addr,sizeof(clnt_addr));
				socklen_t clnt_addr_len=sizeof(clnt_addr);


				int clnt_sockfd=accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_len);//接受连接
				errif(clnt_sockfd==-1,"socket accept error");
				printf("new client fd %d! IP:%s port: %d\n",clnt_sockfd,inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port));

				bzero(&ev,sizeof(ev));
				ev.data.fd=clnt_sockfd;
				ev.events=EPOLLIN|EPOLLET;//对于客户端连接，使用ET模式，可以让epoll更高效，支持更多并发
				setnonblocking(clnt_sockfd);//ET模式搭配非阻塞socket
				epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sockfd,&ev);
			}
			else if(events[i].events & EPOLLIN){//发生事件的是客户端，且是可读事件
				char buf[READ_BUFFER];//定义缓冲区
				//读写
				while(true){
					bzero(&buf,sizeof(buf));//清空缓冲区
					ssize_t read_bytes=read(events[i].data.fd,buf,sizeof(buf));//从客户端socket读取到缓冲区，返回已读大小
					if(read_bytes>0){
						printf("message from client fd %d:%s\n",events[i].data.fd,buf);
						write(events[i].data.fd,buf,sizeof(buf));//将相同数据写到客户端
					}
					else if(read_bytes==0){//客户端断开连接 EOF
						printf("EOF client fd %d disconnected",events[i].data.fd);
						close(events[i].data.fd);
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
			else{
				printf("somthing else happened\n");
			}
		}
	}

	close(sockfd);
	return 0;

}















