#ifndef N_MAXLOGHANDLER_H
#define N_MAXLOGHANDLER_H

#include <Max.h>

#include "kernel/nloghandler.h"

class nMaxLogHandler : public nLogHandler
{
public:
	nMaxLogHandler();
	~nMaxLogHandler();
	 
	/// print a message to the max.log dump
	virtual void Print(const char* msg, va_list argList);
	/// show an important message (may block the program until the user acks)
	virtual void Message(const char* msg, va_list argList);
	/// show an error message (may block the program until the user acks)
	virtual void Error(const char* msg, va_list argList);
	
	///Setup the reference to Max Interface Log
	void SetLogSys(LogSys* log);
private:
	enum logType
	{
		LOG_PRINT = 1,
		LOG_MESSAGE,
		LOG_ERROR	
	};
	int numMsgDismissed;
	
	void doLog(logType type, const char* msg, va_list argList);
	LogSys* log;
};

#endif