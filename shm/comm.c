
/*************************************************************************
    > File Name: comm.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Sat 17 Oct 2015 11:01:11 PM CST
 ************************************************************************/

#include"comm.h"
static int shm_create_and_get(int flag)
{
    key_t key=ftok(_PATH_,_PROJ_ID_);
      if(key < 0)
	  {
		  perror("ftok");
		  exit(1);
	  }
   return  shmget(key,1024,flag);
}

int shm_create()
{
return shm_create_and_get(IPC_CREAT | IPC_EXCL | 0666);
}
int shm_get()
{
return shm_create_and_get(IPC_CREAT);
}
void* shm_attach(int shmid)
{
return shmat(shmid,NULL,0);
}
int shm_detach(const void*shmaddr)
{
	return shmdt(shmaddr);
}
int shm_destory(int shmid)
{
	return shmctl(shmid,IPC_RMID,0);
}

