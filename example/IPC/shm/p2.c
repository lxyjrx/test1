#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>






int main(int argc, char **argv)
{
	//key_t key;
	int shmid;
	char *shmaddr = NULL;
	sem_t *sem1 = NULL;
	sem_t *sem2 = NULL;
	char read_buf[100] = {0};

#if 0
	//首先通过ftok申请一个IPC的key
	key = ftok("./1", 123);
	if((int)key < 0)
	{
		perror("creat a ipc key fail");
		return -1;
	}
#endif
	
	//申请一块共享内存
	shmid = shmget((key_t)1234, 100, IPC_CREAT|0666);	
	if(shmid < 0)
	{
		perror("creat a shm fail");
		return -1;
	}

	//映射到本用户进程
	shmaddr = shmat(shmid, NULL, 0);
	if((long)shmaddr < 0)
	{
		perror("shm attach fail");
		return -1;
	}

	//创建一个posix有名信号量
	sem1 = sem_open("sem1", O_CREAT|O_RDWR, 0666, 1);
	if(sem1 == SEM_FAILED)
	{
		perror("sem1 open fail");
		return -1;
	}
	
	//创建一个posix有名信号量
	sem2 = sem_open("sem2", O_CREAT|O_RDWR, 0666, 0);
	if(sem2 == SEM_FAILED)
	{
		perror("sem2 open fail");
		return -1;
	}
	
	while(1)
	{
		//P操作，等待信号量有效
		sem_wait(sem2);

		strncpy(read_buf, shmaddr, 10);
		printf("read info from shm:%s\n", read_buf);
		
		//V操作，释放信号量
		sem_post(sem1);

		if(strncmp("quit", read_buf, 4) == 0)
		{
			printf("p2 ready toquit\n");
			break;
		}
	}
	
	//释放资源
	
	return 0;
}
