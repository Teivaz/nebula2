#ifndef N_LW_VIEWER_REMOTE_H
#define N_LW_VIEWER_REMOTE_H
//----------------------------------------------------------------------------
#include "util/nstring.h"
#include "gfx2/nshaderstate.h"
#include "gfx2/nshaderarg.h"

class nIpcClient;

//----------------------------------------------------------------------------
/**
    @class nLWViewerRemote
    @brief Sends messages to the Nebula 2 nViewer application.

    Currently this remote is used for real-time tweaking of shader
    parameters, any changes made to shader parameters by the user in 
    Lightwave are sent through the remote to nViewer.
*/
class nLWViewerRemote
{
public:
    /// constructor
    nLWViewerRemote();
    /// destructor
    ~nLWViewerRemote();
    /// return pointer to instance
    static nLWViewerRemote* Instance();

    /// open the connection to nViewer
    bool Open();
    /// close the connection to nViewer
    void Close();
    /// return true if the connection to nViewer is open
    bool IsOpen() const;

    bool ChangeShaderParam(const nString& shaderName, 
                           const nString& shaderHandler,
                           nShaderState::Param shaderParamId,
                           const nShaderArg& shaderParamValue);

private:
    static nString ShaderArgToString(const nShaderArg&);
    bool Send(const nString&);

    static nLWViewerRemote* Singleton;
    bool isOpen;
    nIpcClient* client;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWViewerRemote*
nLWViewerRemote::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool
nLWViewerRemote::IsOpen() const
{
    return this->isOpen;
}

//----------------------------------------------------------------------------
#endif // N_LW_VIEWER_REMOTE_H
