//------------------------------------------------------------------------------
//  nscriptserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nscriptserver.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nScriptServer, "kernel::nroot");

nObject* nScriptServer::currentTargetObject = 0;

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
    @param  result      [out] will be filled with the result
    @return             false if an error occured during execution
*/
bool
nScriptServer::Run(const char* /* cmdStr */, nString& /* result */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Run a script function with the specified name without any args

    @param functionName     the function to invoke
    @param result           [out] will be filled with the result
    @return                 false if an error occured during execution
*/
bool
nScriptServer::RunFunction(const char* /*functionName*/, nString& /*result*/)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Run a script file.

    @param  filename    the script filename (ABSOLUTE PATH!)
    @param  result      [out] will be filled with the result
    @return             false if an error occured during execution
*/
bool
nScriptServer::RunScript(const char* /* filename */, nString& /* result */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
#ifndef __XBxX__
nString
nScriptServer::Prompt()
{
    nString emptyString;
    return emptyString;
}
#endif

//------------------------------------------------------------------------------
/**
*/
nFile*
nScriptServer::BeginWrite(const char* /* filename */, nObject* /* obj */)
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


