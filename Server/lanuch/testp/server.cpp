#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "../../include/LogInterface.h"
#include "../../include/Func.h"
#include "../NetConstant.h"
#include "../../proto/proto.h"
#include "../../proto/protoID.h"
#include "../../MyDB/dbcpp/DBInterface.h"
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
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
			cout<<"child "<<pid<<"  terminated!"<<endl;
	return ;

}
//read the stream after get the proto id
static ssize_t readn(int connfd,void *vptr,size_t len){

	size_t nleft; //the left character in stream
	ssize_t nread;	//had read character in stream
	
	char *ptr;

	ptr=(char*)vptr;

	nleft=len;
	
	while(nleft>0){
		if((nread=read(connfd,ptr,nleft))<0){
				if(errno==EINTR)
					nread=0;
				else
					return -1;
		}else if(0==nread){
				break;
		}
	
		nleft=nleft-nread;
		ptr=ptr+nread;
	}
	return (len-nleft);

};

//write the stream into the connfd
static ssize_t writen(int connfd,void *vptr,size_t len){

	size_t nleft;
	ssize_t nwritten;
	
	const char *ptr;
	ptr=(char*)vptr;
	nleft=len;

	while(nleft>0){
		if((nwritten=write(connfd,ptr,nleft))<=0){
				if(nwritten<0&&errno==EINTR){
						cout<<"nwritten<0"<<endl;
						nwritten=0;
				}else{
						cout<<"write to stream error"<<endl;
						exit(0);
				}

		}
		
		nleft=nleft-nwritten;
		ptr=ptr+nwritten;


	}
	return (len-nwritten);

}

//read the id to decide the proto

static void startProc(int connfd){
	
	while(true){
		char buf[256];
		memset(buf,'\0',256);
		int nread=readn(connfd,buf,256);
		
		int k;
		


	}

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
	
		
		socklen_t clilen=sizeof(cliaddr);

		connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
		
		startProc(connfd);

}

