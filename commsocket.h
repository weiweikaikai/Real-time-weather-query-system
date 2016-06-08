#ifndef _COMMSOCKET_H_
#define _COMMSOCKET_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<signal.h>
#include<sys/syscall.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/sendfile.h>
#ifdef __cplusplus
extern 'C'
{
#endif

//�����붨��  

#define Sck_Ok             	0
#define Sck_BaseErr   		3000

#define Sck_ErrParam                	(Sck_BaseErr+1)
#define Sck_ErrTimeOut                	(Sck_BaseErr+2)
#define Sck_ErrPeerClosed               (Sck_BaseErr+3)
#define Sck_ErrMalloc			   		(Sck_BaseErr+4)

#define BACK_LOG 5
#define BUF_SIZE 1024
#define MAIN_PAGE "index.html"
#define SERVICE_VERSION "HTTP/1.1"


//��������
//�������˳�ʼ��
int Server_init(const char* ip,unsigned short port);
//��������������
int Server_accept(int listenfd,int timeout);
//�������˷��ͱ���
int Server_send(int connfd,  unsigned char *data, int datalen, int timeout);
//�������˶˽��ܱ���
int Server_rev(int  connfd, unsigned char *out, int *outlen,  int timeout); 

 int activate_nonblock(int fd);


#ifdef __cpluspluse
}
#endif


#endif
