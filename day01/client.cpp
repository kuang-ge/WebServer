#include <sys/socket.h>
#include <arpa/inet.h>//该头文件已经包含<netinet/in.h>
#include <stdio.h>
#include <string.h>
int main(){
	int sockfd=socket(AF_INET,SOCK_STREAM,0);//建立一个socket套接字，对外提供一个通信接口

	//创建一个结构体 要将套接字绑定到IP地址和端口
	struct sockaddr_in serv_addr;
	bzero(&serv_addr,sizeof(serv_addr));//初始化

	//设置地址组、IP地址、端口
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv_addr.sin_port=htons(8888);

	connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));
	return 0;
}















