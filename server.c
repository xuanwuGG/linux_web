/* ************************************************************************
> File Name:     sever.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年09月23日 星期六 18时00分01秒
> Description:   
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>
#define SPORT 2116

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc,char *argv[])
{
    char buffer[BUFSIZ],clientIP[BUFSIZ];
    int server_fd,sfd,ret;
    struct sockaddr_in server_addr,client_addr;
    socklen_t clientLenth=sizeof(client_addr);
    server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1)
    {
        sys_err("socket error!");
    }
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SPORT);//此处要转成网络字节序！！
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ret=bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret==-1)
    {
        sys_err("bind error!");
    }
    listen(server_fd,128);
    sfd=accept(server_fd,(struct sockaddr*)&client_addr,&clientLenth);
    if(sfd==-1)
    {
        sys_err("accept error!");
    }
    printf("client's' ip:%s,port:%d\n",
            inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,clientIP,sizeof(clientIP)),
            ntohs(client_addr.sin_port));
    while(1)
    {
    int reSize=read(sfd,buffer,sizeof(buffer));
    write(STDOUT_FILENO,buffer,reSize);
    for(int i=0;i<reSize;i++)
    {
        buffer[i]=toupper(buffer[i]);
    }
    write(sfd,buffer,reSize);
    }
    return 0;
}
