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
itn events;
void *args;
void (*call_back)(int fd, int events,void *arg);
int status;//表示该文件描述符是否在红黑树上
char buf[BUFLEN];
int len;
long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENT+1];

void eventset(struct myevent_s *ev,int fd, void (*call_back)(int,int ,void,void *),void *arg)
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
    }
    int nfd=epoll_wait(g_efd,events,MAX_EVENT,events,MAX_EVENT+1,1000);
    if(nfd<0){printf("epoll_wait error,exit\n");break;}
    for(i=0;i<nfd;i++)
    {
        struct myevent_s *ev=(struct myevent_s*)events[i].data.ptr;
        if((events[i].events & EPOLLIN)&&(ev->events & EPOLLIN))
        {
            ev->call_back(ev->fd,events[i].events,ev->arg);
        }
        if((events[i].events & EPOLLIN)&&(ev->events & EPOLLIN))
        {
            ev->call_back(ev->fd,events[i].events,ev->arg);
        }
    }
    return 0;
}
