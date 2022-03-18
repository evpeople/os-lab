#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
static Usage(const char *proc)
{
	printf("%s [local_ip] [local_port]\n", proc);
}
int start_up(const char *_ip, int _port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket");
		exit(2);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(_port);
	local.sin_addr.s_addr = inet_addr(_ip);
	if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}
	if (listen(sock, 10) < 0)
	{
		perror("listen");
		exit(4);
	}
	return sock;
}
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		Usage(argv[0]);
		return 1;
	}
	int sock = start_up(argv[1], atoi(argv[2]));
	int epollfd = epoll_create(256);
	if (epollfd < 0)
	{
		perror("epoll_create");
		return 5;
	}
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) < 0)
	{
		perror("epoll_ctl");
		return 6;
	}
	int evnums = 0; //epoll_wait return val
	struct epoll_event evs[64];
	int timeout = -1;
	while (1)
	{
		switch (evnums = epoll_wait(epollfd, evs, 64, timeout))
		{
		case 0:
			printf("timeout...\n");
			break;
		case -1:
			perror("epoll_wait");
			break;
		default:
		{
			int i = 0;
			for (; i < evnums; ++i)
			{
				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				if (evs[i].data.fd == sock && evs[i].events & EPOLLIN)
				{
					int new_sock = accept(sock,
							      (struct sockaddr *)&client, &len);
					if (new_sock < 0)
					{
						perror("accept");
						continue;
					} //if accept failed
					else
					{
						printf("Get a new client[%s]\n",
						       inet_ntoa(client.sin_addr));
						ev.data.fd = new_sock;
						ev.events = EPOLLIN;
						epoll_ctl(epollfd, EPOLL_CTL_ADD,
							  new_sock, &ev);
					} //accept success

				} //if fd == sock
				else if (evs[i].data.fd != sock &&
					 evs[i].events & EPOLLIN)
				{
					char buf[1024];
					ssize_t s = read(evs[i].data.fd, buf, sizeof(buf) - 1);
					if (s > 0)
					{
						buf[s] = 0;
						printf("client say#%s", buf);
						ev.data.fd = evs[i].data.fd;
						ev.events = EPOLLOUT;
						epoll_ctl(epollfd, EPOLL_CTL_MOD,
							  evs[i].data.fd, &ev);
					} //s > 0
					else
					{
						close(evs[i].data.fd);
						epoll_ctl(epollfd, EPOLL_CTL_DEL,
							  evs[i].data.fd, NULL);
					}
				} //fd != sock
				else if (evs[i].data.fd != sock && evs[i].events & EPOLLOUT)
				{
					char *msg = "HTTP/1.0 200 OK <\r\n\r\n<html><h1>yingying beautiful </h1></html>\r\n";
					write(evs[i].data.fd, msg, strlen(msg));
					close(evs[i].data.fd);
					epoll_ctl(epollfd, EPOLL_CTL_DEL,
						  evs[i].data.fd, NULL);
				} //EPOLLOUT
				else
				{
				}
			} //for
		}	  //default
		break;
		} //switch
	}	  //while
	return 0;
}
