/* ************************************************************************
> File Name:     epollReactor.c
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年10月16日 星期一 20时29分33秒
> Description:   
 ************************************************************************/
#include<wrap.h>
#define MAX_EVENT 1024 //监听上限
#define BUFLEN 4096
#define SERV_PORT 2116

void recvData(int fd, int events,void *arg);
void sendData(int fd, int events,void *arg);

struct myevent_s{
int fd;
int events;
void *arg;
void (*call_back)(int fd, int events,void *arg);
int status;//表示该文件描述符是否在红黑树上
char buf[BUFLEN];
int len;
long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENT+1];




void eventset(struct myevent_s *ev,int fd, void (*call_back)(int,int,void *),void *arg)
{
    ev->fd=fd;
    ev->call_back=call_back;
    ev->events=0;
    ev->arg=arg;
    ev->status=0;
    memset(ev->buf,0,sizeof(ev->buf));
    ev->len=0;
    ev->last_active=time(NULL);//用于踢出不活跃的连接
    return;
}
void eventadd(int efd,int events,struct myevent_s *ev)
{
    struct epoll_event epv={0,{0}};
    int op;
    epv.data.ptr=ev;
    epv.events=ev->events=events;
    if(ev->status==0)
    {
        op=EPOLL_CTL_ADD;
        ev->status=1;
    }
    if(epoll_ctl(efd,op,ev->fd,&epv)<0){printf("event add failed [fd=%d],[events=%d]\n",ev->fd,events);}
    else{printf("event add successfully,[fd=%d],[op=%d],[event=%d]\n",ev->fd,op,events);}
    return;
}

void acceptconn(int lfd, int events,void *arg)
{
    struct sockaddr_in cin;
    socklen_t len =sizeof(cin);
    int cfd,i;
    if((cfd=accept(lfd,(struct sockaddr *)&cin,&len))==-1)
    {
        if(errno!=EAGAIN&&errno!=EINTR){printf("something wrong\n");exit(1);}
        printf("%s: accept error:%d,%s\n",__func__,errno,strerror(errno));
        printf("the server fd :%d\n",cfd);
    }
    if(cfd<0){printf("%s\n",strerror(errno));}
    do{
        for(i=0;i<MAX_EVENT;i++)
        {
            if(g_events[i].status==0){break;}
        }
        if(i==MAX_EVENT){printf("%s:max connect limit\n",__func__);break;}
        int flag=0;
        if((flag=fcntl(cfd,F_SETFL,O_NONBLOCK))<0){printf("%s:fcntl setting nonblock error %s\n",__func__,strerror(errno));break;}
        eventset(&g_events[i],cfd,recvData,&g_events[i]);
        eventadd(g_efd,EPOLLIN,&g_events[i]);
    }while(0);
    printf("new connect[%s:%d][time:%ld][position:%d]\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),g_events[i].last_active,i);
    return ;
}
void eventdel(int efd,struct myevent_s *ev)
{
    struct epoll_event epv={0,{0}};
    int op;
    epv.data.ptr=ev;
    if(ev->status==1)
    {
        op=EPOLL_CTL_DEL;
        ev->status=0;
    }
    if(epoll_ctl(efd,op,ev->fd,&epv)<0){printf("event delete failed [fd=%d]\n",ev->fd);}
    else{printf("event delete successfully,[fd=%d],[op=%d]\n",ev->fd,op);}
    return;
}

void sendData(int fd, int events,void *arg)
{
    struct myevent_s *ev=(struct myevent_s*)arg;
    int len;
    len =send(fd,ev->buf,ev->len,0);
    eventdel(g_efd,ev);
    if(len>0)
    {
        printf("send successfully[fd=%d],%s\n",fd,ev->buf);
        eventset(ev,fd,recvData,ev);
        eventadd(g_efd,EPOLLIN,ev);
    }
    else{
        close(ev->fd);
        printf("send error[fd=%d],%s\n",fd,strerror(errno));
    }
    return;
}
void recvData(int fd,int events,void *arg)
{
    struct myevent_s *ev=(struct myevent_s*)arg;
    int len;
    len=recv(fd,ev->buf,sizeof(ev->buf),0);
    eventdel(g_efd,ev);
    if(len>0)
    {
        ev->len=len;
        ev->buf[len]='\0';
        printf("receive data:%s[fd=%d]\n",ev->buf,fd);
        eventset(ev,fd,sendData,ev);
        eventadd(g_efd,EPOLLOUT,ev);
    }
    else if(len==0)
    {
        close(ev->fd);
        printf("close [fd:%d],[pos:%ld]\n",fd,ev-g_events);
    }
    else
    {
        close(ev->fd);
        printf("recv error[fd=%d],%s\n",fd,strerror(errno));
    }
    return;
}


void initlistensocket(int efd,short port)
{
    struct sockaddr_in sin;
    int lfd=Socket(AF_INET,SOCK_STREAM,0);
    fcntl(lfd,F_SETFL,O_NONBLOCK);
    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    sin.sin_addr.s_addr=htonl(INADDR_ANY);
    Bind(lfd,(struct sockaddr *)&sin,sizeof(sin));
    Listen(lfd,20);
    eventset(&g_events[MAX_EVENT],lfd,acceptconn,&g_events[MAX_EVENT]);
    eventadd(efd,EPOLLIN,&g_events[MAX_EVENT]);
    return;
}

int main(int argc,char *argv[])
{
    unsigned short port=SERV_PORT;
    if(argc==2){port=atoi(argv[1]);}

    g_efd=epoll_create(MAX_EVENT+1);
    if(g_efd<0){printf("create epollfd failed in %s error %s\n",__func__,strerror(errno));}
    initlistensocket(g_efd,port);
    struct epoll_event events[MAX_EVENT+1];
    printf("server running [port:%d]\n",port);

    int checkpos=0,i;
    while(1){
        //验证超时模块
        long now =time(NULL);
        for(i=0;i<100;i++,checkpos++)
        {
            if(checkpos==MAX_EVENT){checkpos=0;}
            if(g_events[checkpos].status!=1){continue;}
            long duration=now-g_events[checkpos].last_active;

            if(duration>=10)
            {
                close(g_events[checkpos].fd);
                printf("timeout,[fd:%d]\n",g_events[checkpos].fd);
                eventdel(g_efd,&g_events[checkpos]);
            }
        }
    int nfd=epoll_wait(g_efd,events,MAX_EVENT+1,1000);
    if(nfd<0){printf("epoll_wait error,exit\n");break;}
    else if(nfd==0){printf("no event happen\n");}
    for(i=0;i<nfd;i++)
    {
        struct myevent_s *ev=(struct myevent_s*)events[i].data.ptr;
        if((events[i].events & EPOLLIN)&&(ev->events & EPOLLIN))
        {
            ev->call_back(ev->fd,events[i].events,ev->arg);
        }
        if((events[i].events & EPOLLOUT)&&(ev->events & EPOLLOUT))
        {
            ev->call_back(ev->fd,events[i].events,ev->arg);
        }
    }
    }
    return 0;
}
