# 题目 使用epoll select机制编程
## 实验目的
了解掌握Linux系统提供的select、epoll等I/O机制，编写客户端、服务器端程序，编程验证对比这几种网络I/O机制

## 实验内容
一个基于epoll的dns

一些简单的select，epoll的使用

## 实验设计原理/步骤

## 实验结果及分析、

## 程序代码，
在EasyDns中使用epoll的实例
```c++
    int epollfd = epoll_create(MAX_EPOLL_SIZE);
    struct epoll_event ev;
    struct epoll_event events[MAX_EPOLL_SIZE];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
    while (1)
    {
        int nfds = epoll_wait(epollfd, events, 20, 500);
        for (size_t i = 0; i < nfds; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                int count = recvfrom(listenfd, buf, 1024, 0, (struct sockaddr *)&clent_addr, &len);
                char *rawmsg = malloc(sizeof(char) * ANS_LEN);
                memcpy(rawmsg, buf, ANS_LEN);
                setblocking(listenfd);
                dealWithPacket(rawmsg, (struct sockaddr *)&clent_addr, listenfd, count);
                setnonblocking(listenfd);
            }
        }
    }
```