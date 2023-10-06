/* ************************************************************************
> File Name:     multProcesConcurrent.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年10月06日 星期五 18时57分18秒
> Description:   
 ************************************************************************/
#include<wrap.h>
#define PORT 1111

void childCatch(int signum)
{
    while(waitpid(signum,0,WNOHANG)>0);
    return;
}
int main(int argc,char *argv[])
{
    int lfd,sfd,ret;
    struct sockaddr_in servaddr,clieaddr;
    pid_t pid=0;
    char buff[BUFSIZ];
    socklen_t len=sizeof(clieaddr);
    //memset(&servaddr,0,sizeof(servaddr));//将地址置空
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    lfd=Socket(AF_INET,SOCK_STREAM,0);
    Bind(lfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    listen(lfd,128);
    while(1)
    {
        sfd=Accept(lfd,(struct sockaddr*)&clieaddr,&len);
        pid=fork();
        if(pid<0){perr_exit("fork error");}
        else if(pid==0){close(lfd);break;}
        else
        {
            struct sigaction act;
            act.sa_handler=childCatch;
            sigemptyset(&act.sa_mask);
            act.sa_flags=0;
            ret=sigaction(SIGCHLD,&act,NULL);
            {
                if(ret!=0){perr_exit("sigaction error");}
            }
            close(sfd);
        }

    }
    if(pid==0)
    {
        while(1){
        ret=Read(sfd,buff,sizeof(buff));
        if(ret==0)
        {
            close(sfd);
            exit(1);
        }    
        for(int i=0;i<ret;i++)
        {
            buff[i]=toupper(buff[i]);
        }
        Write(sfd,buff,ret);
        Write(STDOUT_FILENO,buff,ret);
        }
    }
    return 0;
}
