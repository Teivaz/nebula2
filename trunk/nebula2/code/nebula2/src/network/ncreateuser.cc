//------------------------------------------------------------------------------
//  nCreateUser.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "network/nCreateUser.h"
#include "network/nbuddyclient.h"


//------------------------------------------------------------------------------
/**
*/
nCreateUser::nCreateUser()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCreateUser::~nCreateUser()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool nCreateUser::Execute()
{

    nStream xmlStream;
    xmlStream.SetFilename("MyData");
    xmlStream.OpenString("");
    xmlStream.BeginNode("rlbuddysystem");
    xmlStream.SetInt("ver",1);
    xmlStream.SetInt("id",this->uniqueID);
    xmlStream.SetString("type","set");
    xmlStream.BeginNode("createuser");
    xmlStream.SetString("user",this->username);
    xmlStream.SetString("password",this->password);
    xmlStream.EndNode();
    xmlStream.EndNode();

    xmlStream.GetDocumentAsString(this->xmlRequest);

    return nBuddyClient::Instance()->SendCommand(this);
}

