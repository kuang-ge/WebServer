#include <sys/socket.h>
#include <arpa/inet.h>//该头文件已经包含<netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "util.h"//封装了错误检测函数
#include <unistd.h>//包含read和write函数
int main(){
	int sockfd=socket(AF_INET,SOCK_STREAM,0);//建立一个socket套接字，对外提供一个通信接口
	errif(sockfd==-1,"socket create error");
	
	//创建一个结构体 要将套接字绑定到IP地址和端口
	struct sockaddr_in serv_addr;
	bzero(&serv_addr,sizeof(serv_addr));//初始化

	//设置地址组、IP地址、端口
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv_addr.sin_port=htons(8888);

	errif(connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr))==-1,"connect error");
	
	//读写 要包含<unistd.h>头文件
	while(true){
		char buf[1024];//定义缓冲区
		bzero(&buf,sizeof(buf));//清空缓冲区
		scanf("%s",buf);//输入数据
		ssize_t write_bytes=write(sockfd,buf,sizeof(buf));//发送缓冲区的数据给服务器socket，返回发送的大小
		if(write_bytes==-1){
			printf("socket is disconnected,can not write any more!\n");
			break;
		}
		
		bzero(&buf,sizeof(buf));//清空缓冲区
		ssize_t read_bytes=read(sockfd,buf,sizeof(buf));//从服务器读取数据，返回读取的大小
		if(read_bytes>0){
			printf("message from server: %s\n",buf);
		}
		else if(read_bytes==0){//通常是服务器断开连接
			printf("socket is disconnected\n");
			break;
		}
		else if(read_bytes==-1){//表示发生错误
			close(sockfd);
			errif(true,"socket read error!");
		}
	}
	close(sockfd);

	return 0;
}















