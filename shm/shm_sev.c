/*************************************************************************
    > File Name: shm_sev.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Sat 17 Oct 2015 11:05:26 PM CST
 ************************************************************************/
#include"comm.h"

int main(int argc,char* argv[])
{
printf("sev\n");
int shmid =shm_create();

char * shmptr=(char*) shm_attach(shmid);

char arr[1024];
while(1)
{
scanf("%s ",arr);
memcpy(shmptr,arr,sizeof(arr));
}
//sleep(100);


printf("sev is shmptr = %s\n",shmptr);
shm_detach(shmptr);
shm_destory(shmid);


return  0;
}

