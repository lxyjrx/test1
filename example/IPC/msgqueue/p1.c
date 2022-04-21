#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>


#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

struct msgbuf 
{
	long mtype;       /* message type, must be > 0 */
	char mtext[100];    /* message data */
};


int main(void)
{
	int msqid;	//保存消息队列的id（标识符）
	sem_t *sem1 = NULL;	//保存信号量地址
	sem_t *sem2 = NULL;	//保存信号量地址

	//首先申请一个消息队列
	msqid = msgget((key_t)123, IPC_CREAT|0666);	
	if(msqid < 0)
	{
		perror("creat a msg queue fail");
		return -1;
	}

	//创建或打开一个POSIX有名信号量
	sem1 = sem_open("sem1", O_CREAT|O_RDWR, 0666, 1);
	if(sem1 == SEM_FAILED)
	{
		perror("open sem1 fail");
		return -1;
	}
	
	//创建或打开一个POSIX有名信号量
	sem2 = sem_open("sem2", O_CREAT|O_RDWR, 0666, 0);
	if(sem2 == SEM_FAILED)
	{
		perror("open sem1 fail");
		return -1;
	}
	
	struct msgbuf msg_to_send;
	msg_to_send.mtype = 1;
	
	while(1)
	{
		//等待信号量有效，P操作
		sem_wait(sem1);

		if(fgets(msg_to_send.mtext, 11, stdin) != msg_to_send.mtext)
		{
			perror("get char from stdin fail");
			return -2;
		}
		
		if(msgsnd(msqid, &msg_to_send, 100, 0) < 0)
		{
			perror("send msg to queue fail");
			return -3;
		}
		
		//释放信号量，V操作
		sem_post(sem2);

		if(strncmp("quit", msg_to_send.mtext, 4)==0)
		{
			printf("p1 readys to quit\n");
			break;
		}
	}
	
	sleep(1);
	//释放各种资源
	sem_close(sem1);
	sem_close(sem2);

	if(msgctl(msqid, IPC_RMID, NULL) < 0)
	{
		perror("remove msg queue fail");
	}

	return 0;
}
