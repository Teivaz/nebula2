//------------------------------------------------------------------------------
//  ncommand.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/ncommand.h"


 int nCommand::CommandCounter = 0;


//------------------------------------------------------------------------------
/**
*/
nCommand::nCommand():
    curStatus(COMMAND_DISABLED)
{
    if (CommandCounter > 2000000000) CommandCounter = 0;
    uniqueID = ++CommandCounter;
}

//------------------------------------------------------------------------------
/**
*/
nCommand::~nCommand()
{
    // emtpy
}


void nCommand::EvaluateResult(nStream& result)
{
     if (result.HasAttr("type"))
     {
         nString type = result.GetString("type");

         if (type=="result")
         {
             this->curStatus = COMMAMD_RESULT;
         }
         else if(type=="error")
         {
             this->curStatus = COMMAMD_ERROR;
         }
     }
}