//////////////////////////////////////////////////////////////////
  
//  Description: 使用命名管道FIFO，实现两个进程间通信

//////////////////////////////////////////////////////////////////

#include "head4fifo.h"

int main(int argc, char **argv)
{
	if(access(FIFO, F_OK))
	{
		mkfifo(FIFO, 0644);
	}

	int fifo = open(FIFO, O_RDONLY);
	
	char msg[20];
	while(1)
	{
		bzero(msg, 20);

		read(fifo, msg, 20);
		printf("from FIFO: %s", msg);
		
		if(!strncmp(msg, "quit", 4))
			break;
	}

	close(fifo);
	return 0;
}

