#define N_IMPLEMENTS nLogHandler
//------------------------------------------------------------------------------
//  nloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nloghandler.h"

//------------------------------------------------------------------------------
/**
*/
nLogHandler::nLogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLogHandler::~nLogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Print(const char* str, va_list argList)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Message(const char* str, va_list argList)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Error(const char* str, va_list argList)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Subclasses may log all messages in an internal line buffer. If they
    chose to do so, this must be a nLineBuffer class and override the
    GetLineBuffer() method.
*/
nLineBuffer*
nLogHandler::GetLineBuffer()
{
    return 0;
}
