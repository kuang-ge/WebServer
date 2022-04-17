#pragma once
#include <functional>

//接受连接类
//分离连接模块 TCP面向连接的特性

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor
{
private:
    EventLoop *loop;
    Socket *sock;
    InetAddress *addr;
    Channel *acceptChannel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    std::function<void(Socket*)> newConnectionCallback;//函数指针的替代品 常与std::bind搭配
    void setNewConnectionCallback(std::function<void(Socket*)>);
};
