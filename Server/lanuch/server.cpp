#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "../include/LogInterface.h"
#include "../include/Func.h"
#include "NetConstant.h"
#include "Connection.h"
#include "openShm.h"
#include "../include/struct/shmServer.h"
/**
 *this is the server main process
 *all user process are fork from this process
 *after fork from this ,child process do it's own things
 *author:coderguagn
 *date:2014/04/08
 */

//the child process exit functions

void sig_chld_exit(int signo){
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0){
	//change the share memory 
			int fd;
			struct shmNum *shmptr;
			sem_t *mutex;

			fd=shm_open("mshmNum",O_RDWR,0644);
			if(fd<0){
					cout<<"can't open mshmNum failed!"<<endl;
			}

			shmptr=(struct shmNum*)mmap(NULL,sizeof(struct shmNum),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		
			close(fd);
			
			mutex=sem_open("msemNum",0);
			sem_wait(mutex);
			shmptr->counter--;
			sem_post(mutex);


			cout<<"child "<<pid<<"  terminated!"<<endl;
	}
	
	
	return ;

}

int main(){
	int listenfd;	//the  listen  fd
	int connfd;	//the client fd;

	struct sockaddr_in cliaddr;	//client addr
	struct sockaddr_in servaddr;	//server addr
	
	//init the listenfd
	if(-1==(listenfd=socket(AF_INET,SOCK_STREAM,0))){
			cout<<"socket init failed!"<<endl;
			exit(0);
	}
	
	bzero(&servaddr,sizeof(servaddr));
	
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	servaddr.sin_addr.s_addr=INADDR_ANY;

	//bind the port
	if(-1==bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))){
			cout<<"bind the port failed!"<<endl;
			exit(0);
	}
	
	//listen
	
	if(-1==listen(listenfd,BACKLOG)){
		cout<<"listen failed"<<endl;
		exit(1);
	}
	//catch the child process exit
	signal(SIGCHLD,sig_chld_exit);

	//open the share memory of mshmNum and mshmList
	
	openShmFunc();//in openShm.h
	
	while(true){
		
		socklen_t clilen=sizeof(cliaddr);

		connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
		
		if(fork()==0){
				//string addr=inet_ntoa(cliaddr.sin_addr);
				//cout<<"client from "<<addr<<" lanuch"<<endl;
				//close(connfd);
				//exit(0);
				//cout<<"connfd="<<connfd<<endl;
				string ip=inet_ntoa(cliaddr.sin_addr);
				startProc(connfd,ip);
				//exit(0);

		}



	}

}

