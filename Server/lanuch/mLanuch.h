#ifndef MLANUCH_H_
#define MLANUCH_H_

#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> //for read write
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include "../proto/Proto.h"
#include "../myDB/dbcpp/DBInterface.h"
#include "../myDB/dbcpp/DBErr.h"
#include "../include/Rdwr.h"
#include "../struct/ShmServer.h"
#include "../include/Func.h"
#include "../include/InitFirst.h"
#include "../include/BroadcastInterface.h"
#include "../publicRoom/UpdateParty.h"

using namespace std;

/**
 *	before do others ,it must lanuch the account 
 *
 *
 *
 */


extern struct shmList *listptr;
extern struct shmNum *numptr;
extern sem_t *listmutex;
extern sem_t *nummutex;


static void mLanuchGame(int connfd,string ip){

	//check the server user counter,if it bigger than MAX_USER,reject the new connections

	
	sem_wait(nummutex);
	//check the user counter  in the server
	if(numptr->counter>=MAX_USER){
			cout<<"come to server full!"<<endl;
			Err_toc *temp=new Err_toc(SERVER_FULL);
			writen(connfd,&temp->id,sizeof(Err_toc));
			sem_post(nummutex);
			DelayTime(5);
			exit(1);
	}
	sem_post(nummutex);
	
	
	while(true){
			int id=0;
			int nread=read(connfd,&id,4);

			if(nread<0){
				if(errno!=EINTR&&errno==EPIPE){
						cout<<"socket disconnections...."<<endl;
						close(connfd);
						exit(1);
				}else
					 continue;			
			}else if(0==nread)	/*EOF of the stream */
					 continue;

			if(pLanuchID==id){
						Lanuch_tos *ptr=new Lanuch_tos();
						memset(ptr,'\0',sizeof(Lanuch_tos));
						readn(connfd,&ptr->error_code,sizeof(Lanuch_tos)-4);

						cout<<"try lanuch:accounts="<<ptr->account<<"  passwd="<<ptr->passwd<<endl;
						string account=ptr->account;
						string passwd=ptr->passwd;
						struct Lanuch lanResult;
						int rNum=LanuchAccount(account,passwd,ip,lanResult);

						cout<<"rNum="<<rNum<<endl;
						
						//lanuch success
						if(0==rNum){
							//save this to the shmList				

							int partyTemp=0;//use to remember this account's party
							sem_wait(listmutex);

							//for(int i=0;i<MAX_USER;i++){
							for(int i=0;i<3;i++){
								cout<<"in lanuch:flag["<<i<<"]="<<listptr->flag[i]<<"  pid="<<listptr->pid[i]<<" id="<<listptr->id[i]<<endl;
								if(0==listptr->flag[i]){

										//server counter ++ and decide it's party and first to avoid when the process exit cause the exception
										//cout<<"now server counter is "<<numptr->counter<<endl;
										sem_wait(nummutex);
							
										if(numptr->blueCounter<=numptr->redCounter&&numptr->blueCounter<(MAX_USER/2)){
												listptr->party[i]=BLUE;
												numptr->counter++;
												numptr->blueCounter++;
												partyTemp=BLUE;
										}else if(numptr->blueCounter>numptr->redCounter&&numptr->redCounter<(MAX_USER/2)){
												listptr->party[i]=RED;
												numptr->counter++;
												numptr->redCounter++;
												partyTemp=RED;
										}else{ //if the server is full
												Err_toc *err=new Err_toc(SERVER_FULL);
												writen(connfd,&err->id,sizeof(Err_toc));
												
												sem_post(nummutex);
												sem_post(listmutex);	
											
												close(connfd);
												exit(1);

										}

										sem_post(nummutex);

										//if decide the party success,take it to list
										listptr->flag[i]=1;//flag this is used
										listptr->id[i]=lanResult.id;
										//listptr->pid[i]=(int)pid;
										pid_t pid=getpid();
										listptr->pid[i]=(int)pid;
										listptr->conn[i]=connfd;

										cout<<"id="<<listptr->id[i]<<" in the listptr,the party is "<<listptr->party[i]<<endl;

										break;
								}
							}					

							//for(int i=0;i<MAX_USER;i++)
							for(int i=0;i<3;i++)
								cout<<"after lanuch:flag["<<i<<"]="<<listptr->flag[i]<<"  id="<<listptr->id[i]<<" pid="<<listptr->pid[i]<<endl;

							sem_post(listmutex);
		

							LanuchResult_toc *result=new LanuchResult_toc(lanResult.name,lanResult.lastlanuch,lanResult.lastIP,lanResult.setting,lanResult.id);
							cout<<"name="<<result->name<<"  lastlanuch="<<result->lastLanuch<<"  lastip="<<result->lastIP<<"  setting="<<result->setting<<endl<<endl;
							
							result->party=partyTemp;
							result->error_code=0;
							writen(connfd,&result->id,sizeof(LanuchResult_toc));

							//if it lanuch right,break this loop and come to the public room loop
							//send the Party_toc
							
							//Party_toc *ptemp=new Party_toc(1000,1001,1002,1003,1004,1005,1006,1007,1008,1009);	
							//writen(connfd,&ptemp->id,sizeof(Party_toc));
							//boardcast the updateparty to all
						
							updateParty();							

							break;
						
						}else{
							Err_toc *err=new Err_toc(rNum);
							writen(connfd,&err->id,sizeof(Err_toc));
						}
					}
				

			}
}

#endif
