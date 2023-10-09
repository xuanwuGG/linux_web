/* ************************************************************************
> File Name:     client.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年09月24日 星期日 19时50分32秒
> Description:   
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<sys/socket.h>
#include<ctype.h>
#include<arpa/inet.h>
void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc,char *argv[])
{
   int cfd=socket(AF_INET,SOCK_STREAM,0);
   struct sockaddr_in serv_add;
   serv_add.sin_family=AF_INET;
   serv_add.sin_port=htons(1111);
   inet_pton(AF_INET,"127.0.0.1",&serv_add.sin_addr.s_addr);
   int ret=connect(cfd,(struct sockaddr *)&serv_add,sizeof(serv_add));
   if(ret==-1)
   {
       sys_err("connect errror:");
   }
   char buf[BUFSIZ];
   for(;;)
   {
       write(cfd,"bipaopu\n",8);
       int size=read(cfd,buf,sizeof(buf));
       write(STDOUT_FILENO,buf,size);
       sleep(2);
   } 

}
