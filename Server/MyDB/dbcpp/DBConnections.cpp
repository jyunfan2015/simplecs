#include <mysql.h> 
#include <string> 
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "DBErr.h"
#include "../../include/Func.h"
#include "../../include/struct/lanuch.h"
#include "../../include/struct/result.h"
#include "../../include/struct/resultAll.h"
#include "../../include/LogInterface.h"
extern "C"{
#include "DBConstant.h"
}

using namespace std;


enum TYPE{REGIT,LANUCH,SECURE,RESULT,GETRESULT};
//simple instances
class DBConnections
{
protected:
	MMYSQL *regit[REGITMAX];
	MMYSQL *lanuch[LANUCHMAX];
	MMYSQL *secure[SECUREMAX];
	MMYSQL *result[RESULTMAX];
	MMYSQL *getResult[GETRESULTMAX];


 	void initDB(){ 
		if(mysql_server_init(sizeof(server_args)/sizeof(char*),server_args,server_groups)){ 
			DBLog(FATAL,"database init failed!");
			exit(1);
		};

		//init MMYSQL []
		for(int i=0;i<REGITMAX;i++){
			regit[i]=new MMYSQL;
			regit[i]->conn=nullptr;
			regit[i]->flag=0;
			regit[i]->conn=mysql_init(NULL);
			//connect to database.c_str()
			if(!mysql_real_connect(regit[i]->conn,server.c_str(),regitUser.c_str(),regitPasswd.c_str(),database.c_str(),0,NULL,0)){
				DBLog(FATAL,"regitUser  connec failed!");
				DBLog(FATAL,mysql_error(regit[i]->conn));
				exit(1);
			}
		}
		for(int i=0;i<LANUCHMAX;i++){
			lanuch[i]=new MMYSQL;
			lanuch[i]->conn=nullptr;
			lanuch[i]->flag=0;
			lanuch[i]->conn=mysql_init(NULL);
			//connect to database.c_str()
			if(!mysql_real_connect(lanuch[i]->conn,server.c_str(),lanuchUser.c_str(),lanuchPasswd.c_str(),database.c_str(),0,NULL,0)){
				DBLog(FATAL,"lanuchUser connec failed!");
				DBLog(FATAL,mysql_error(lanuch[i]->conn));
				exit(1);
			}
		}

		for(int i=0;i<SECUREMAX;i++){ 
			secure[i]=new MMYSQL; 
			secure[i]->conn=nullptr;
			secure[i]->flag=0;
			secure[i]->conn=mysql_init(NULL);
			//connect to database.c_str()
			if(!mysql_real_connect(secure[i]->conn,server.c_str(),secureUser.c_str(),securePasswd.c_str(),database.c_str(),0,NULL,0)){
				DBLog(FATAL,"secureUser connec failed!");
				DBLog(FATAL,mysql_error(secure[i]->conn));
				exit(1);
			}
		}
		
		for(int i=0;i<RESULTMAX;i++){
			result[i]=new MMYSQL;
			result[i]->conn=nullptr;
			result[i]->flag=0;
			result[i]->conn=mysql_init(NULL);
			//connect to database.c_str()
			if(!mysql_real_connect(result[i]->conn,server.c_str(),resultUser.c_str(),resultPasswd.c_str(),database.c_str(),0,NULL,0)){
				DBLog(FATAL,"resultUser connec failed!");
				DBLog(FATAL,mysql_error(result[i]->conn));
				exit(1);
			}
		}

		for(int i=0;i<GETRESULTMAX;i++){
			getResult[i]=new MMYSQL;
			getResult[i]->conn=nullptr;
			getResult[i]->flag=0;
			getResult[i]->conn=mysql_init(NULL);
			//connect to database.c_str()
			if(!mysql_real_connect(getResult[i]->conn,server.c_str(),getResultUser.c_str(),getResultPasswd.c_str(),database.c_str(),0,NULL,0)){
				DBLog(FATAL,"getReusltUser connec failed!");
				DBLog(FATAL,mysql_error(getReuslt[i]->conn));
				exit(1);
			}
		}
	};

	MMYSQL *GetFree(TYPE type){
		switch(type){
			case REGIT:
				for(int i=0;i<REGITMAX;i++){
					if(0==regit[i]->flag){//free
						//lock();
						//cout<<"regit "<<i<<"is use"<<endl; 
					 	regit[i]->flag=1;//set flag become busy	
						return regit[i];
					}
				};
				break;
			case LANUCH:
				for(int i=0;i<LANUCHMAX;i++){
					if(0==lanuch[i]->flag){//free
						//lock();
					 	lanuch[i]->flag=1;//set flag become busy	
						return lanuch[i];
					}
				};
				break;
			case SECURE:
				 for(int i=0;i<SECUREMAX;i++){
					if(0==secure[i]->flag){//free
						//lock();
					 	secure[i]->flag=1;//set flag become busy	
						return secure[i];
					}
				};
				break;
			case RESULT:
				 for(int i=0;i<RESULTMAX;i++){
					if(0==result[i]->flag){//free
						//lock();
					 	result[i]->flag=1;//set flag become busy	
						return result[i];
					}
				};
				break;
			case GETRESULT:
				 for(int i=0;i<GETRESULTMAX;i++){
					if(0==getResult[i]->flag){//free
						//lock();
					 	getResult[i]->flag=1;//set flag become busy	
						return getResult[i];
					}
				};
				break;
			} 
			//if no conn,return nullptr
			DBLog(DEBUG,"no free connects for sql");
			return nullptr;

	}

	//interface for query
	/**
	type:who will query the sql
	sql:the sql query 
	res:result
	*/
	int MyQuery(TYPE type,string sql,MYSQL_RES **res){
		if(0==sql.length()){
			DBLog(DEBUG,"the sql.length=0");
			return SQL_NULL;
		}
		//illegal type
		if(type!=REGIT&&type!=LANUCH&&type!=SECURE&&type!=RESULT&&type!=GETRESULT){
			DBLog(DEBUG,"the sql type is error");
			return TYPE_ERROR; 
		}
		MMYSQL *temp=GetFree(type); 
		if(temp==NULL)
			return NO_FREE_CONN;	
		int errNum;
		if(0!=(errNum=mysql_query(temp->conn,sql.c_str()))){
			string errStr="query error!"+mysql_error(temp->conn);
			DBLog(DEBUG,errStr);
			temp->flag=0;
		 	return errNum;	
		 
		}
		if(type==REGIT||type==LANUCH||type==GETRESULT||type==SECURE){ 
			if(nullptr!=res)
			    *res=mysql_store_result(temp->conn);
		}	
		temp->flag=0;
		return OK;
	}
	DBConnections(){
		initDB();
	};
	DBConnections(DBConnections &)=delete;
	static DBConnections *instance;
	
//public
public:
	static DBConnections *GetInstance(){
		if(nullptr==instance) {
			DBLog(INFO,"DBConnections init");
			instance=new DBConnections();
		}
		return instance;
	};
	//regit
	//for check the account when regit
	int CheckAccount(string account){
		MYSQL_RES *res;
		string sql="select account from accounts where account='"+account+"'";
		int returnNum=MyQuery(REGIT,sql,&res);
		if(0==returnNum){
			if((int)mysql_num_rows(res)==0)	{
				return OK;
			}else
				return HAD_REGIT;
		}else
			return returnNum;
	

	} 
	//for regit a accounts
	int RegitAccount(string account,string passwd,string email,string name,int question,string answer){
		string questionStr=IntToStr(question);
		string t=GetTimeNow();
		string sql="insert into accounts values(null,'"+account+"','"+passwd+"','"+email+"','"+name+"','"+questionStr+"','"+answer+"',false,'"+t+"',null,0,1000,0,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000)";
		 int rNum=MyQuery(REGIT,sql,nullptr);
		 return rNum;
			
	}
	
//for forget the passwd	
	int ForgetPasswd(string account,string email,int question,string answer){
		string questionStr=IntToStr(question);
		string sql="select account from accounts where account='"+account+"' AND  email='"+email+"' AND question='"+questionStr+"' AND answer='"+answer+"'";
		MYSQL_RES *res;
		int rNum=MyQuery(SECURE,sql,&res);
		if(0==rNum){
				if(1==(int)mysql_num_rows(res))
							return OK;
				else if(0==(int)mysql_num_rows(res))
							return ERR_IDENTITY; 
				else 
							return (int)mysql_num_rows(res);
			}else
				return rNum;
	
	}

//for reset the passwd
	int ResetPasswd(string account,string newPasswd){
			string sql="update accounts set passwd='"+newPasswd+"' where account='"+account+"'";
			MYSQL_RES *res;
			int rNum=MyQuery(SECURE,sql,&res);
			if(0==rNum){
			//can't use the result?
			//		if(1==(int)mysql_num_rows(res))			
									return OK;
			//		else{
			//					Log("something bad");
			//					return (int)mysql_num_rows(res);
			//		}
			}else{
							Log("reset fail");
							return rNum;
			}
	}
	//for lanuch
	int Lanuch(string account,string passwd,Lanuch &lanResult){
			string sql="select name,lastlanuch,lastIP,setting from accounts where account='"+account+"' AND passwd='"+passwd+"'";
			MYSQL_RES *res;
			int rNum=MyQuery(LANUCH,sql,&res);
			if(0==rNum){
				if(1==(int)mysql_num_rows(res)){
						MYSQL_ROW row;			
						if(NULL!=(row=mysql_fetch_row(res))){
								lanResult.name=row[0];
								lanResult.lastlanuch=row[1];
								lanResult.lastIP=row[2];
								lanResult.setting=StrToInt(row[3]);
								return OK;
						}else
								return ERROR;
				}else	
						return NO_THIS_ACCOUNT;
			}else 
				return rNum;

	}
	//for InsertResult 
	//need  update the accounts flag,result*
	int InsertResult(ResultAll &resultA){
			string sql="insert into results values(DEFAULT,'"+IntToStr(resultA.red1.id)+"','"+IntToStr(resultA.red1.kill)+"','"+IntToStr(resultA.red1.death)+"','"+IntToStr(resultA.red1.assist)+"','"+IntToStr(resultA.red1.harm)+"','"+IntToStr(resultA.red1.getHarm)+"','"+IntToStr(resultA.red1.reHP)+"','"+IntToStr(resultA.red1.nullGun)+"','"+IntToStr(resultA.red2.id)+"','"+IntToStr(resultA.red2.kill)+"','"+IntToStr(resultA.red2.death)+"','"+IntToStr(resultA.red2.assist)+"','"+IntToStr(resultA.red2.harm)+"','"+IntToStr(resultA.red2.getHarm)+"','"+IntToStr(resultA.red2.reHP)+"','"+IntToStr(resultA.red2.nullGun)+"','"+IntToStr(resultA.red3.id)+"','"+IntToStr(resultA.red3.kill)+"','"+IntToStr(resultA.red3.death)+"','"+IntToStr(resultA.red3.assist)+"','"+IntToStr(resultA.red3.harm)+"','"+IntToStr(resultA.red3.getHarm)+"','"+IntToStr(resultA.red3.reHP)+"','"+IntToStr(resultA.red3.nullGun)+"','"+IntToStr(resultA.red4.id)+"','"+IntToStr(resultA.red4.kill)+"','"+IntToStr(resultA.red4.death)+"','"+IntToStr(resultA.red4.assist)+"','"+IntToStr(resultA.red4.harm)+"','"+IntToStr(resultA.red4.getHarm)+"','"+IntToStr(resultA.red4.reHP)+"','"+IntToStr(resultA.red4.nullGun)+"','"+IntToStr(resultA.red5.id)+"','"+IntToStr(resultA.red5.kill)+"','"+IntToStr(resultA.red5.death)+"','"+IntToStr(resultA.red5.assist)+"','"+IntToStr(resultA.red5.harm)+"','"+IntToStr(resultA.red5.getHarm)+"','"+IntToStr(resultA.red5.reHP)+"','"+IntToStr(resultA.red5.nullGun)+"','"+IntToStr(resultA.blue1.id)+"','"+IntToStr(resultA.blue1.kill)+"','"+IntToStr(resultA.blue1.death)+"','"+IntToStr(resultA.blue1.assist)+"','"+IntToStr(resultA.blue1.harm)+"','"+IntToStr(resultA.blue1.getHarm)+"','"+IntToStr(resultA.blue1.reHP)+"','"+IntToStr(resultA.blue1.nullGun)+"','"+IntToStr(resultA.blue2.id)+"','"+IntToStr(resultA.blue2.kill)+"','"+IntToStr(resultA.blue2.death)+"','"+IntToStr(resultA.blue2.assist)+"','"+IntToStr(resultA.blue2.harm)+"','"+IntToStr(resultA.blue2.getHarm)+"','"+IntToStr(resultA.blue2.reHP)+"','"+IntToStr(resultA.blue2.nullGun)+"','"+IntToStr(resultA.blue3.id)+"','"+IntToStr(resultA.blue3.kill)+"','"+IntToStr(resultA.blue3.death)+"','"+IntToStr(resultA.blue3.assist)+"','"+IntToStr(resultA.blue3.harm)+"','"+IntToStr(resultA.blue3.getHarm)+"','"+IntToStr(resultA.blue3.reHP)+"','"+IntToStr(resultA.blue3.nullGun)+"','"+IntToStr(resultA.blue4.id)+"','"+IntToStr(resultA.blue4.kill)+"','"+IntToStr(resultA.blue4.death)+"','"+IntToStr(resultA.blue4.assist)+"','"+IntToStr(resultA.blue4.harm)+"','"+IntToStr(resultA.blue4.getHarm)+"','"+IntToStr(resultA.blue4.reHP)+"','"+IntToStr(resultA.blue4.nullGun)+"','"+IntToStr(resultA.blue5.id)+"','"+IntToStr(resultA.blue5.kill)+"','"+IntToStr(resultA.blue5.death)+"','"+IntToStr(resultA.blue5.assist)+"','"+IntToStr(resultA.blue5.harm)+"','"+IntToStr(resultA.blue5.getHarm)+"','"+IntToStr(resultA.blue5.reHP)+"','"+IntToStr(resultA.blue5.nullGun)+"')";
		int rNum=MyQuery(RESULT,sql,nullptr);				
		if(0==rNum){
			return OK;
		}else{
			return rNum;
		}
	}
	//for getResult
	int GetResult(int resultID,ResultAll &resultA){
		string sql="select * from results where id='"+IntToStr(resultID)+"'";
		MYSQL_RES *res;
		int rNum=MyQuery(GETRESULT,sql,&res);
		if(0==rNum){
			MYSQL_ROW row;
			if(NULL!=(row=mysql_fetch_row(res))){
				//row[0] is the result id
				resultA.red1.id=StrToInt(row[1]);
				resultA.red1.kill=StrToInt(row[2]);
				resultA.red1.death=StrToInt(row[3]);
				resultA.red1.assist=StrToInt(row[4]);
				resultA.red1.harm=StrToInt(row[5]);
				resultA.red1.getHarm=StrToInt(row[6]);
				resultA.red1.reHP=StrToInt(row[7]);
				resultA.red1.nullGun=StrToInt(row[8]);

				resultA.red2.id=StrToInt(row[9]);
				resultA.red2.kill=StrToInt(row[10]);
				resultA.red2.death=StrToInt(row[11]);
				resultA.red2.assist=StrToInt(row[12]);
				resultA.red2.harm=StrToInt(row[13]);
				resultA.red2.getHarm=StrToInt(row[14]);
				resultA.red2.reHP=StrToInt(row[15]);
				resultA.red2.nullGun=StrToInt(row[16]);

				resultA.red3.id=StrToInt(row[17]);
				resultA.red3.kill=StrToInt(row[18]);
				resultA.red3.death=StrToInt(row[19]);
				resultA.red3.assist=StrToInt(row[20]);
				resultA.red3.harm=StrToInt(row[21]);
				resultA.red3.getHarm=StrToInt(row[22]);
				resultA.red3.reHP=StrToInt(row[23]);
				resultA.red3.nullGun=StrToInt(row[24]);

				resultA.red4.id=StrToInt(row[25]);
				resultA.red4.kill=StrToInt(row[26]);
				resultA.red4.death=StrToInt(row[27]);
				resultA.red4.assist=StrToInt(row[28]);
				resultA.red4.harm=StrToInt(row[29]);
				resultA.red4.getHarm=StrToInt(row[30]);
				resultA.red4.reHP=StrToInt(row[31]);
				resultA.red4.nullGun=StrToInt(row[32]);

				resultA.red5.id=StrToInt(row[33]);
				resultA.red5.kill=StrToInt(row[34]);
				resultA.red5.death=StrToInt(row[35]);
				resultA.red5.assist=StrToInt(row[36]);
				resultA.red5.harm=StrToInt(row[37]);
				resultA.red5.getHarm=StrToInt(row[38]);
				resultA.red5.reHP=StrToInt(row[39]);
				resultA.red5.nullGun=StrToInt(row[40]);

				resultA.blue1.id=StrToInt(row[41]);
				resultA.blue1.kill=StrToInt(row[42]);
				resultA.blue1.death=StrToInt(row[43]);
				resultA.blue1.assist=StrToInt(row[44]);
				resultA.blue1.harm=StrToInt(row[45]);
				resultA.blue1.getHarm=StrToInt(row[46]);
				resultA.blue1.reHP=StrToInt(row[47]);
				resultA.blue1.nullGun=StrToInt(row[48]);

				resultA.blue2.id=StrToInt(row[49]);
				resultA.blue2.kill=StrToInt(row[50]);
				resultA.blue2.death=StrToInt(row[51]);
				resultA.blue2.assist=StrToInt(row[52]);
				resultA.blue2.harm=StrToInt(row[53]);
				resultA.blue2.getHarm=StrToInt(row[54]);
				resultA.blue2.reHP=StrToInt(row[55]);
				resultA.blue2.nullGun=StrToInt(row[56]);

				resultA.blue3.id=StrToInt(row[57]);
				resultA.blue3.kill=StrToInt(row[58]);
				resultA.blue3.death=StrToInt(row[59]);
				resultA.blue3.assist=StrToInt(row[60]);
				resultA.blue3.harm=StrToInt(row[61]);
				resultA.blue3.getHarm=StrToInt(row[62]);
				resultA.blue3.reHP=StrToInt(row[63]);
				resultA.blue3.nullGun=StrToInt(row[64]);

				resultA.blue4.id=StrToInt(row[65]);
				resultA.blue4.kill=StrToInt(row[66]);
				resultA.blue4.death=StrToInt(row[67]);
				resultA.blue4.assist=StrToInt(row[68]);
				resultA.blue4.harm=StrToInt(row[69]);
				resultA.blue4.getHarm=StrToInt(row[70]);
				resultA.blue4.reHP=StrToInt(row[71]);
				resultA.blue4.nullGun=StrToInt(row[72]);

				resultA.blue5.id=StrToInt(row[73]);
				resultA.blue5.kill=StrToInt(row[74]);
				resultA.blue5.death=StrToInt(row[75]);
				resultA.blue5.assist=StrToInt(row[76]);
				resultA.blue5.harm=StrToInt(row[77]);
				resultA.blue5.getHarm=StrToInt(row[78]);
				resultA.blue5.reHP=StrToInt(row[79]);
				resultA.blue5.nullGun=StrToInt(row[80]);

				return OK;
			}else{
				return RESULT_NOT_EXIST;
			}
			
		}else{
			return rNum;
		}

	}
	

};
 //must initialize in out of class ,deny would link error!!!
DBConnections *DBConnections::instance=nullptr;
