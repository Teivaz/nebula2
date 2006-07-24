//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwviewerremote.h"
#include "kernel/nipcclient.h"

nLWViewerRemote* nLWViewerRemote::Singleton = 0;

//----------------------------------------------------------------------------
/**
*/
nLWViewerRemote::nLWViewerRemote() :
    isOpen(false),
    client(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//----------------------------------------------------------------------------
/**
*/
nLWViewerRemote::~nLWViewerRemote()
{
    n_assert(Singleton);
    Singleton = 0;

    if (this->isOpen)
    {
        this->Close();
    }
    if (this->client)
    {
        n_delete(this->client);
        this->client = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWViewerRemote::Open()
{
    n_assert(!this->isOpen);

    if (!this->client)
    {
        this->client = n_new(nIpcClient);
    }
    if (this->client)
    {
        n_assert(!this->client->IsConnected());

        nIpcAddress viewerAddress("localhost", "nviewer");
        this->client->SetBlocking(true);
        if (this->client->Connect(viewerAddress))
        {
            this->isOpen = true;
        }
    }

    return this->isOpen;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWViewerRemote::Close()
{
    n_assert(this->isOpen);

    if (this->client)
    {
        if (this->client->IsConnected())
        {
            this->client->Disconnect();
        }
    }
    this->isOpen = false;
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWViewerRemote::ChangeShaderParam(const nString& shaderName,
                                   const nString& shaderHandler,
                                   nShaderState::Param shaderParamId,
                                   const nShaderArg& shaderParamValue)
{
    nString msg;
    msg.Format("/sys/servers/toolkit.changeshaderparameter \"%s\" \"%s\" \"%s\" \"%s\"",
               shaderName.Get(), shaderHandler.Get(),
               nShaderState::ParamToString(shaderParamId),
               this->ShaderArgToString(shaderParamValue).Get());
    return this->Send(msg);
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWViewerRemote::Send(const nString& msg)
{
    static nIpcBuffer buffer(4096);

    if (this->client)
    {
        buffer.SetString(msg.Get());
        return this->client->Send(buffer);
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
nString
nLWViewerRemote::ShaderArgToString(const nShaderArg& arg)
{
    switch (arg.GetType())
    {
        case nShaderState::Bool:
            return nString::FromBool(arg.GetBool());

        case nShaderState::Int:
            return nString::FromInt(arg.GetInt());

        case nShaderState::Float:
            return nString::FromFloat(arg.GetFloat());

        case nShaderState::Float4:
        {
            nFloat4 f4 = arg.GetFloat4();
            nString temp;
            temp.Format("%.6f %.6f %.6f %.6f", f4.x, f4.y, f4.z, f4.w);
            return temp;
        }

        default:
            // should never get here
            n_assert(false);
    }

    return nString();
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
