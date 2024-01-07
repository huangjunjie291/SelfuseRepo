/* 
* 实现一个简单的echo 服务器，编译后使用 nc 命令链接 
*    nc -v 127.0.0.1 6789
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "eventloop.h" /*接口*/

typedef struct ClientContext
{
    int  fd;
    char buf[1024];
}ClientContext;

static inline void setfdisblock(int fd, bool isblock)
{
    int flags = fcntl(fd, F_GETFL);
    if(flags < 0)
        return;
    if(isblock) // 阻塞
    {
        flags &= ~O_NONBLOCK;
    }
    else // 非阻塞
    {
        flags |= O_NONBLOCK;
    }    
    
    if(fcntl(fd, F_SETFL, flags)<0)
        perror("fcntl set");
}

static void ClientReadWriteProc(int fd, void *clientData, int mask)
{
    ClientContext* pClient = (ClientContext*)clientData;
    if(mask & AE_READABLE)
    {
        int ret = read(fd, pClient->buf, sizeof(pClient->buf));
        if(ret == -1)
        {
            perror("read faild");
            return;
        }
        write(fd, pClient->buf, sizeof(pClient->buf));
        printf("receive client message, conn:%d content:%s\n", pClient->fd, pClient->buf);
    }
    if(mask & AE_WRITABLE)
    {
        if(pClient->buf[0] != '\0')
        {
            write(fd, pClient->buf, sizeof(pClient->buf));
            printf("send client message, conn:%d content:%s\n", pClient->fd, pClient->buf);
            memset(pClient->buf, 0, sizeof(pClient->buf));
        }
    }
}

static void ClientWriteProc(int fd, void *clientData, int mask)
{
    ClientContext* pClient = (ClientContext*)clientData;
    if(mask & AE_READABLE)
    {
        int ret = read(fd, pClient->buf, sizeof(pClient->buf));
        if(ret == -1)
        {
            perror("read faild");
            return;
        }
        write(fd, pClient->buf, sizeof(pClient->buf));
        printf("receive client message, conn:%d content:%s\n", pClient->fd, pClient->buf);
    }
    else if(mask & AE_WRITABLE)
    {
        if(pClient->buf[0] != '\0')
        {
            write(fd, pClient->buf, sizeof(pClient->buf));
            printf("send client message, conn:%d content:%s\n", pClient->fd, pClient->buf);
            memset(pClient->buf, 0, sizeof(pClient->buf));
        }
    }
}


static void ServerLinkProc(int fd, void *clientData, int mask)
{
    struct sockaddr_in connaddr;
    socklen_t len = sizeof(connaddr);

    int conn = accept(fd, (struct sockaddr*)&connaddr, &len);
    char strip[64] = {0};
    char *ip = inet_ntoa(connaddr.sin_addr);
    strcpy(strip, ip);
    printf("client connect, conn:%d,ip:%s, port:%d\n", conn, strip,ntohs(connaddr.sin_port));

    setfdisblock(conn, false);

    ClientContext* pClient = (ClientContext*)malloc(sizeof(ClientContext));
    pClient->fd = conn;
    memset(pClient->buf, 0, 1024);

    EventLoop* evloop = (EventLoop*)clientData;
    evloop->CreateFileEvent(conn, AE_READABLE|AE_WRITABLE, ClientReadWriteProc, (void*)pClient);
}

int TestTimeProc(long long id, void *clientData)
{
    printf("timer %lld called\n", id);
    return 2000;
}

int main()
{
    int opt = 1;
    int server_fd;
    int server_port = 6789;
    struct sockaddr_in address;

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置套接字选项
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    // 绑定套接字到IP地址和端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 20) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

// 使用Reactor
    EventLoop evloop;

    evloop.CreateFileEvent(server_fd, AE_READABLE, ServerLinkProc, (void*)&evloop );
    evloop.CreateTimeEvent(2000, TestTimeProc, NULL, NULL);
    evloop.RunLoop();
    return 0;
}

