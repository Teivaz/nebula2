//------------------------------------------------------------------------------
//  nscriptserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nscriptserver.h"

nNebulaClass(nScriptServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nScriptServer::nScriptServer() :
    quitRequested(false),
    failOnError(true),
    selMethod(SELCOMMAND)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer::~nScriptServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Run a script statement.

    @param  cmdStr      the statement to execute
    @param  result      [out] will be filled with pointer to result
    @return             false if an error occured during execution
*/
bool 
nScriptServer::Run(const char* /* cmdStr */, const char*& /* result */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Run a script file.

    @param  filename    the script filename (ABSOLUTE PATH!)
    @param  result      [out] will be filled with pointer to result
    @return             false if an error occured during execution
*/
bool
nScriptServer::RunScript(const char* /* filename */, const char*& /* result */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
#ifndef __XBxX__
char*
nScriptServer::Prompt(char* buf, int bufSize)
{
    buf[0] = 0;
    return buf;
}
#endif

//------------------------------------------------------------------------------
/**
*/
nFile*
nScriptServer::BeginWrite(const char* /* filename */, nRoot* /* obj */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteBeginNewObject(nFile*, nRoot *, nRoot *)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteBeginNewObjectCmd(nFile*, nRoot *, nRoot *, nCmd *)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteBeginSelObject(nFile*, nRoot *, nRoot *)
{
   // overwrite in subclass
   return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteCmd(nFile*, nCmd *)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteEndObject(nFile*, nRoot *, nRoot *)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nScriptServer::EndWrite(nFile* /* file */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    This method should be called frequently (normally once per frame) when
    the script server is not run in interactive mode.

     - 21-Dec-98   floh    created
     - 31-Aug-99   floh    'quit_requested' wird zurueckgesetzt, wenn
                           es einmal true war
*/
bool 
nScriptServer::Trigger()
{
    bool retval = !(this->quitRequested);
    this->quitRequested = false;
    return retval;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------


