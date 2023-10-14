/* ************************************************************************
> File Name:     epoll_concurrent.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年10月14日 星期六 20时04分31秒
> Description:   
 ************************************************************************/
#include<wrap.h>
#define PORT 2116
#define MAXLINE 8192
#define OPEN_MAX 5000
int main(int argc,char *argv[])
{
    int lfd,cfd,sockfd,num=0;
    struct sockaddr_in serv_addr,clie_addr;
    char str[1024];
    char buf[MAXLINE];
    socklen_t clie_len=sizeof(clie_addr);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    lfd=Socket(AF_INET,SOCK_STREAM,0);
    Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    Listen(lfd,128);

    struct epoll_event tep,ep[OPEN_MAX];
    ssize_t efd,nready;
    int res;
    efd=epoll_create(OPEN_MAX);
    if(efd==-1){perr_exit("epoll_create error");}
    tep.events=EPOLLIN;
    tep.data.fd=lfd;
    res=epoll_ctl(efd,EPOLL_CTL_ADD,lfd,&tep);
    if(res==-1){perr_exit("epoll_ctl error");}
    while(1)
    {
        nready=epoll_wait(efd,ep,OPEN_MAX,-1);
        if(nready==-1){perr_exit("epoll_wait error");}
        for(int i=0;i<nready;i++)
        {
            if(!(ep[i].events & EPOLLIN)){continue;}
            if(ep[i].data.fd==lfd)
            {
                cfd=Accept(lfd,(struct sockaddr *)&clie_addr,&clie_len);
                printf("received form %s at PORT %d\n",
                        inet_ntop(AF_INET,&clie_addr.sin_addr,str,sizeof(str)),
                        ntohs(clie_addr.sin_port));
                printf("cfd %d ___________________client %d\n",cfd, ++num);

                tep.events=EPOLLIN;tep.data.fd=cfd;
                res=epoll_ctl(efd,EPOLL_CTL_ADD,cfd,&tep);
                if(res==-1){perr_exit("epoll_ctl error");}
            }
            else
            {
                sockfd=ep[i].data.fd;
                int n=Read(sockfd,buf,MAXLINE);
                if(n==0)
                {
                    res=epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);
                    if(res==-1){perr_exit("epoll_exit error ");}
                    close(sockfd);
                    printf("client %d closed connection\n",sockfd);
                }
                else if(n<0)
                {
                    perror("read n<0 error: ");
                    res=epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);
                    close(sockfd);
                }
                else
                {
                    for(int i=0;i<n;i++)
                    {
                        buf[i]=toupper(buf[i]);
                    }
                    Write(STDOUT_FILENO,buf,n);
                    Write(sockfd,buf,n);
                }
            }
        }
    }
    close(lfd);
    return 0;
}
