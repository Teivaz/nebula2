#include "base/nmaxloghandler.h"

//------------------------------------------------------------------------------
/**
*/
nMaxLogHandler::nMaxLogHandler() :
	log(0), numMsgDismissed(0)
{
	//empty
}

//------------------------------------------------------------------------------
/**
*/
nMaxLogHandler::~nMaxLogHandler()
{
	//empty
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxLogHandler::Print(const char* msg, va_list argList)
{
	this->doLog(LOG_PRINT, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxLogHandler::Message(const char* msg, va_list argList)
{
	this->doLog(LOG_MESSAGE, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxLogHandler::Error(const char* msg, va_list argList)
{
	this->doLog(LOG_ERROR, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxLogHandler::doLog(logType type, const char* msg, va_list argList)
{
	if (this->log)
	{
		//FIXME: unsave!!!
		enum
		{
			SIZE = 2048
		};
		char* buf = n_new char[SIZE];
		int len = vsprintf(buf, msg, argList); 
				
		switch (type)
		{
			case LOG_PRINT:
				this->log->LogEntry(SYSLOG_INFO, NO_DIALOG, NULL, buf );
				break;
			case LOG_MESSAGE:
				this->log->LogEntry(SYSLOG_WARN, DISPLAY_DIALOG, "Nebula2 - Message", buf);
				break;
			case LOG_ERROR:
				this->log->LogEntry(SYSLOG_ERROR, DISPLAY_DIALOG, "Nebula2 - Error", buf);
				break;
		}
		n_delete[] buf;
	}
	else
		this->numMsgDismissed++;
	    //TODO: add a fifo for the messeages before we have a ptr to the max syslog 
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxLogHandler::SetLogSys(LogSys* logSys)
{
	if (logSys)
		this->log = logSys;
	n_printf("Nebula2: maxplugin loaded. %i messages lost because of missing buffer.\n", this->numMsgDismissed );
}
