#pragma once
#include <arpa/inet.h>
//IP地址转换类
class InetAddress{
public:
	struct sockaddr_in addr;//专用socket地址
	socklen_t addr_len;

	InetAddress();
	InetAddress(const char* ip,uint16_t port);
	~InetAddress();
};	











