/* ************************************************************************
> File Name:     wrap.h
> Author:        xuanwuGG
> -----------------------------------------------------------------
> Created Time:  2023年10月05日 星期四 20时11分13秒
> Description:   
 ************************************************************************/
#ifndef _WRAP_H_
#define _WRAP_H_
#include<sys/socket.h>
#include<sys/select.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<signal.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<strings.h>
#include<ctype.h>
#include<sys/wait.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<time.h>
void perr_exit(const char *s);
int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr);
int Bind(int fd,const struct sockaddr *sa,socklen_t salen);
int Connect(int fd,const struct sockaddr *sa,socklen_t salen);
int Listen(int fd, int backlog);
int Socket(int faimly,int type,int protocol);
ssize_t Read(int fd,void *ptr,size_t nbytes);
ssize_t Write(int fd, const void *vptr,size_t nbytes);
int Close(int fd);
ssize_t Readn(int fd,void *vptr,size_t n);
ssize_t Writen(int fd,const void *vptr,size_t n);
ssize_t my_read(int fd,char *ptr);
ssize_t Readline(int fd, void *vptr,size_t maxlen);

struct s_info{
    int fd;
    struct sockaddr_in addr;
};
#endif
