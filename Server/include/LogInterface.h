#ifndef LOG_INTERFACE_H_
#define LOG_INTERFACE_H_
#include "Logger.h"
#include "LogConstant.h"
using namespace std;

void MLog(LogType type,Level level,string msg){
	//cout<<"come to log"<<" "<<msg<<endl;
	Logger::GetInstance()->MLog(type,level,msg);
}


#endif 
