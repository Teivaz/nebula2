//------------------------------------------------------------------------------
//  ndefaultloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ndefaultloghandler.h"
#include <string.h>

//------------------------------------------------------------------------------
/**
*/
nDefaultLogHandler::nDefaultLogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDefaultLogHandler::~nDefaultLogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nDefaultLogHandler::Print(const char* msg, va_list argList)
{
    vprintf(msg, argList);
    this->PutLineBuffer(msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nDefaultLogHandler::Message(const char* msg, va_list argList)
{
    vprintf(msg, argList);
    this->PutLineBuffer(msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nDefaultLogHandler::Error(const char* msg, va_list argList)
{
    vprintf(msg, argList);
    this->PutLineBuffer(msg, argList);
    fflush(stdout);
}

//------------------------------------------------------------------------------
/**
    Send a message to debugger window.

    - 26-Mar-05    kims    created
*/
void
nDefaultLogHandler::OutputDebug(const char* msg, va_list argList)
{
    vprintf(msg, argList);
    this->PutLineBuffer(msg, argList);
    fflush(stdout);
}

//------------------------------------------------------------------------------
/**
    Return a pointer to the internal line buffer.
*/
nLineBuffer*
nDefaultLogHandler::GetLineBuffer()
{
    return &(this->lineBuffer);
}

//------------------------------------------------------------------------------
/**
    Put a message into the argbuffer.

    @param  msg     the actual message
    @param  argList a var args list
*/
void
nDefaultLogHandler::PutLineBuffer(const char* msg, va_list argList)
{
    // FIXME: argh, this is not save, and also slow!
    n_assert(msg);
    n_assert(strlen(msg) < 1024);

    char charBuffer[2 * 1024];
    vsnprintf(charBuffer, sizeof(charBuffer), msg, argList);
    this->lineBuffer.Put(charBuffer);
}

