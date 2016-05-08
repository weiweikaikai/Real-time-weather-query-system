/*************************************************************************
    > File Name: shm_cli.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Sat 17 Oct 2015 11:04:47 PM CST
 ************************************************************************/

#include"comm.h"

int main(int argc,char* argv[])
{
printf(" cli\n");
int shmid=shm_get();

while(1)
{
char* shm_ptr=(char*)shm_attach(shmid);
printf("cli= %s\n",shm_ptr);

}

return  0;
}

