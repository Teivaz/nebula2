//------------------------------------------------------------------------------
//  nprefserver_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nprefserver.h"

nNebulaScriptClass(nPrefServer, "nroot");
nPrefServer* nPrefServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nPrefServer::nPrefServer() :
    isOpen(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nPrefServer::~nPrefServer()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->GetCompanyName().IsEmpty());
    n_assert(!this->GetApplicationName().IsEmpty());

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nPrefServer::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteString(const nString& key, const nString& value)
{
    return this->WriteGeneric(key, value);
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteInt(const nString& key, int value)
{
    nString s;
    s.SetInt(value);
    return this->WriteGeneric(key, s);
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteFloat(const nString& key, float value)
{
    nString s;
    s.SetFloat(value);
    return this->WriteGeneric(key, s);
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteBool(const nString& key, bool value)
{
    if (value)
    {
        return this->WriteGeneric(key, "true");
    }
    else
    {
        return this->WriteGeneric(key, "false");
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteVector4(const nString& key, const vector4& value)
{
    nString s;
    s.Format("%f,%f,%f,%f", value.x, value.y, value.z, value.w);
    return this->WriteGeneric(key, s);
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteVector3(const nString& key, const vector3& value)
{
    nString s;
    s.Format("%f,%f,%f", value.x, value.y, value.z);
    return this->WriteGeneric(key, s);
}

//------------------------------------------------------------------------------
/**
*/
nString
nPrefServer::ReadString(const nString& key)
{
    return this->ReadGeneric(key);
}

//------------------------------------------------------------------------------
/**
*/
int
nPrefServer::ReadInt(const nString& key)
{
    return this->ReadGeneric(key).AsInt();
}

//------------------------------------------------------------------------------
/**
*/
float
nPrefServer::ReadFloat(const nString& key)
{
    return this->ReadGeneric(key).AsFloat();
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::ReadBool(const nString& key)
{
    return (this->ReadGeneric(key) == "true");
}

//------------------------------------------------------------------------------
/**
*/
vector4
nPrefServer::ReadVector4(const nString& key)
{
    vector4 value;
    nString valString = this->ReadGeneric(key);
    nArray<nString> tokens;
    valString.Tokenize(",", tokens);
    n_assert(tokens.Size() == 4);
    value.x = tokens[0].AsFloat();
    value.y = tokens[1].AsFloat();
    value.z = tokens[2].AsFloat();
    value.w = tokens[3].AsFloat();
    return value;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nPrefServer::ReadVector3(const nString& key)
{
    vector3 value;
    nString valString = this->ReadGeneric(key);
    nArray<nString> tokens;
    valString.Tokenize(",", tokens);
    n_assert(tokens.Size() == 3);
    value.x = tokens[0].AsFloat();
    value.y = tokens[1].AsFloat();
    value.z = tokens[2].AsFloat();
    return value;
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::WriteGeneric(const nString& /*key*/, const nString& /*value*/)
{
    // NOTE: overwrite this method in a subclass!
    return false;
}

//------------------------------------------------------------------------------
/**
*/
nString
nPrefServer::ReadGeneric(const nString& /*key*/)
{
    // NOTE: overwrite this method in a subclass!
    nString emptyString;
    return emptyString;
}

//------------------------------------------------------------------------------
/**
*/
bool
nPrefServer::KeyExists(const nString& /*key*/)
{
    // NOTE: overwrite this method in a subclass!
    return false;
}

