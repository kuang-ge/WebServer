#include <sys/socket.h>
#include <arpa/inet.h>//该头文件已经包含<netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "util.h"//封装的错误处理函数的头文件
#include <unistd.h>//使用其中的read和write进行数据读写
int main(){

	int sockfd=socket(AF_INET,SOCK_STREAM,0);//建立一个socket套接字，对外提供一个通信接口
	errif(sockfd==-1,"socket creat error");
	//创建一个结构体 要将套接字绑定到IP地址和端口
	struct sockaddr_in serv_addr;
	bzero(&serv_addr,sizeof(serv_addr));//初始化

	//设置地址组、IP地址、端口
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv_addr.sin_port=htons(8888);

	//将socket地址与文件描述符绑定
	errif(bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr))==-1,"socket bind error");

	//监听这个socket端口
	errif(listen(sockfd,SOMAXCONN)==-1,"socket listen error");

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_len = sizeof(clnt_addr);
	bzero(&clnt_addr,sizeof(clnt_addr));
	int clnt_sockfd=accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_len);
	errif(clnt_sockfd==-1,"socket accept error");


	//读写
	while(true){
		char buf[1024];//定义缓冲区
		bzero(&buf,sizeof(buf));//清空缓冲区
		ssize_t read_bytes=read(clnt_sockfd,buf,sizeof(buf));//从客户端socket读取到缓冲区，返回已读大小
		if(read_bytes>0){
			printf("message from client fd %d:%s\n",clnt_sockfd,buf);
			write(clnt_sockfd,buf,sizeof(buf));//将相同数据写到客户端
		}
		else if(read_bytes==0){
			printf("client fd %d disconnected\n",clnt_sockfd);
			close(clnt_sockfd);
			break;
		}
		else if(read_bytes==-1){
			close(clnt_sockfd);
			errif(true,"socket read error");
		}
	}
	close(sockfd);
	return 0;

}















