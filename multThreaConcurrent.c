/* ************************************************************************
> File Name:     multThreaConcurrent.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年10月07日 星期六 20时59分27秒
> Description:   
 ************************************************************************/
#include<wrap.h>
#define PORT 2116

void* do_work(void *arg)
{
    int n,i;
    struct s_info *ts=(struct s_info*)arg;
    char buf[1024];
    char str[16];
    while(1)
    {
        n=Read(ts->fd,buf,1024);
        if(n==0){printf("the client %d closed",ts->fd);break;}
        for(i=0;i<n;i++)
        {
            buf[i]=toupper(buf[i]);
        }
        Write(ts->fd,buf,n);
        Write(STDOUT_FILENO,buf,n);
    }
    close(ts->fd);
    return (void *)0;
}

int main(int argc,char *argv[])
{
    int lfd,sfd;
    pthread pid;
    struct sockaddr_in servaddr,clieaddr;
    struct s_info ts[256];
    int i=0;
    socklen_t clieLen=sizeof(clieLen);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    lfd=Socket(AF_INET,SOCK_STREAM,0);
    Bind(lfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    Listen(lfd,128);
    printf("Accepting client's connection......'");
    while(1)
    {
        sfd=Accept(lfd,(struct sockaddr*)&clieaddr,&clieLen);
        ts[i].fd=sfd;
        ts[i].addr=clieaddr;
        pthread_create(&tid,NULL,do_work,(void*)&ts[i]);
        pthread_detach(tid);
        i++;
    }
    return 0;
}
