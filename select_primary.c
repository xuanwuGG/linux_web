/* ************************************************************************
> File Name:     select_primary.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年10月09日 星期一 10时37分05秒
> Description:   
 ************************************************************************/
#include<wrap.h>
#define SERV_PORT 2116
int main(int argc,char *argv[])
{
    int i,j,n;
    int maxfd;
    int listenfd,connfd;
    char buf[BUFSIZ];
    struct sockaddr_in clie_addr,serv_addr;
    socklen_t clielen=sizeof(clie_addr);
    listenfd=Socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERV_PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    Bind(listenfd,(struct sockaddr* )&serv_addr,sizeof(serv_addr));
    Listen(listenfd,128);

    maxfd=listenfd;
    fd_set rset,allset;
    int ret;
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);
    while(1)
    {
        rset=allset;
        ret=select(maxfd+1,&rset,NULL,NULL,NULL);
        if(ret<0)
        {
            perr_exit("select error");
        }
        if(FD_ISSET(listenfd,&rset))
        {
            connfd=Accept(listenfd,(struct sockaddr*)&clie_addr,&clielen);
            FD_SET(connfd,&allset);
            if(maxfd<connfd){maxfd=connfd;}
            if(ret==1)//说明只有一个listenfd　
            {
                continue;
            }
        }
        for(i=listenfd+1;i<=maxfd;i++)
        {
            if(FD_ISSET(i,&rset))
            {
                n=Read(i,buf,sizeof(buf));
                if(n==-1)
                {
                    perr_exit("read error");
                }
                else if(n==0)
                {
                    close(i);
                    FD_CLR(i,&allset);
                }
                for(j=0;j<n;j++)
                {
                    buf[j]=toupper(buf[j]);
                }
                Write(i,buf,n);
                Write(STDOUT_FILENO,buf,n);
            }
        }
    }
    close(listenfd);
    return 0;
}
