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

static void free_resource(char *shmaddr, int shmid, sem_t *sem1, sem_t *sem2)
{
	int ret;

	ret = shmdt(shmaddr);
	if(ret < 0)
	{
		perror("shm detach fail");
	}
	
	if(shmctl(shmid, IPC_RMID, NULL)<0)
	{
		perror("remove a shm fail");
	}
	
	if(sem_close(sem1) < 0)
	{
		perror("close sem1 fail");
	}

	if(sem_close(sem2) < 0)
	{
		perror("close sem2 fail");
	}

	if(sem_unlink("sem1") < 0)
	{
		perror("remove sem1 fail");
	}

	if(sem_unlink("sem2") < 0)
	{
		perror("remove sem2 fail");
	}
}

int main(int argc, char **argv)
{
	//key_t key;
	int shmid;
	char *shmaddr = NULL;
	sem_t *sem1 = NULL;
	sem_t *sem2 = NULL;
	int ret;

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
		sem_wait(sem1);

		if(fgets(shmaddr, 11, stdin) != shmaddr)
		{
			perror("get char from stdin fail");
			return -2;
		}

		//V操作，释放信号量
		sem_post(sem2);

		if(strncmp("quit", shmaddr, 4) == 0)
		{
			printf("p1 ready toquit\n");
			break;
		}
	}
	
	//释放资源
	free_resource(shmaddr, shmid, sem1, sem2);
	
	return 0;
}
