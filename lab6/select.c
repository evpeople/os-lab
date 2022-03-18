#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
static void Usage(const char *proc)
{
	printf("%s [local_ip] [local_port]\n", proc);
}
int array[4096];
static int start_up(const char *_ip, int _port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket");
		exit(1);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(_port);
	local.sin_addr.s_addr = inet_addr(_ip);
	if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(2);
	}
	if (listen(sock, 10) < 0)
	{
		perror("listen");
		exit(3);
	}
	return sock;
}
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		Usage(argv[0]);
		return -1;
	}
	int listensock = start_up(argv[1], atoi(argv[2]));
	int maxfd = 0;
	fd_set rfds;
	fd_set wfds;
	array[0] = listensock;
	int i = 1;
	int array_size = sizeof(array) / sizeof(array[0]);
	for (; i < array_size; i++)
	{
		array[i] = -1;
	}
	while (1)
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		for (i = 0; i < array_size; ++i)
		{
			if (array[i] > 0)
			{
				FD_SET(array[i], &rfds);
				FD_SET(array[i], &wfds);
				if (array[i] > maxfd)
				{
					maxfd = array[i];
				}
			}
		}
		// printf("test before select");
		switch (select(maxfd + 1, &rfds, &wfds, NULL, NULL))
		{
		case 0:
		{
			printf("timeout\n");
			break;
		}
		case -1:
		{
			perror("select");
			break;
		}
		default:
		{
			int j = 0;
			for (; j < array_size; ++j)
			{
				if (j == 0 && FD_ISSET(array[j], &rfds))
				{
					printf("test \n");
					//listensock happened read events
					struct sockaddr_in client;
					socklen_t len = sizeof(client);
					int new_sock = accept(listensock, (struct sockaddr *)&client, &len);
					if (new_sock < 0) //accept failed
					{
						perror("accept");
						continue;
					}
					else //accept success
					{
						printf("get a new client%s\n", inet_ntoa(client.sin_addr));
						fflush(stdout);
						int k = 1;
						for (; k < array_size; ++k)
						{
							if (array[k] < 0)
							{
								array[k] = new_sock;
								if (new_sock > maxfd)
									maxfd = new_sock;
								break;
							}
						}
						if (k == array_size)
						{
							close(new_sock);
						}
					}
				} //j == 0
				else if (j != 0 && FD_ISSET(array[j], &rfds))
				{
					//new_sock happend read events
					char buf[1024];
					ssize_t s = read(array[j], buf, sizeof(buf) - 1);
					if (s > 0) //read success
					{
						buf[s] = 0;
						printf("clientsay#%s\n", buf);
						if (FD_ISSET(array[j], &wfds))
						{
							char *msg = "HTTP/1.0 200 OK <\r\n\r\n<html><h1>yingying beautiful</h1></html>\r\n";
							write(array[j], msg, strlen(msg));
						}
					}
					else if (0 == s)
					{
						printf("client quit!\n");
						close(array[j]);
						array[j] = -1;
					}
					else
					{
						perror("read");
						close(array[j]);
						array[j] = -1;
					}
				}
				else if (FD_ISSET(array[j], &wfds))
				{
					char *msg = "HTTP/1.0 200 OK <\r\n\r\n<html><h1>yingying beautiful</h1></html>\r\n";
					write(array[j], msg, strlen(msg));
					close(array[j]);
					array[j] = -1;
				}
			}
			break;
		}
		}
	}
	return 0;
}
