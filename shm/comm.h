/*************************************************************************
    > File Name: comm.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Sat 17 Oct 2015 11:01:11 PM CST
 ************************************************************************/

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
#include<dirent.h>
#include<sys/ipc.h>
#include<sys/shm.h>


#define _PATH_ "./log"
#define _PROJ_ID_ 0x1

int shm_create();
int shm_get();
void* shm_attach(int shid);
int shm_detach(const void*shmaddr);
int shm_destory(int shmid);

