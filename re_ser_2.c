/* ************************************************************************
> File Name:     re_ser_2.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年09月26日 星期二 22时06分58秒
> Description:   
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<ctype.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define PORT 2116
void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc,char *argv[])
{
    int listfd,servfd;
    struct sockaddr_in serv_add,cli_add;
    char clientIP[1024];
    listfd=socket(AF_INET,SOCK_STREAM,0);
    serv_add.sin_family=AF_INET;
    serv_add.sin_port=htons(PORT);
    serv_add.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(listfd,(struct sockaddr *)&serv_add,sizeof(serv_add));
    listen(listfd,128);
    servfd=accept(listfd,(struct sockaddr *)&cli_add,(socklen_t *)sizeof(cli_add));
    printf("client's IP:%s,PORT:%d",inet_ntop(AF_INET,&cli_add.sin_addr.s_addr,clientIP,sizeof(clientIP)),ntohs(cli_add.sin_port));
    close(listfd);
    close(servfd);
}
