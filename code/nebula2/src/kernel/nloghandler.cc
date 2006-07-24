//------------------------------------------------------------------------------
//  nloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nloghandler.h"

//------------------------------------------------------------------------------
/**
*/
nLogHandler::nLogHandler() :
    isOpen(false)
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
bool
nLogHandler::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Print(const char* /* str*/, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Message(const char* /* str */, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Error(const char* /* str */, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    - 26-Mar-05    kims    created
*/
void
nLogHandler::OutputDebug(const char* /* str */, va_list /* argList */)
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
