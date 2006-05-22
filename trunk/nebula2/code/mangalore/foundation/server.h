#ifndef FOUNDATION_SERVER_H
#define FOUNDATION_SERVER_H
//------------------------------------------------------------------------------
/**
    @class Foundation::Server

    The Foundation subsystem establishes a basic runtime environment,
    and provides pointers to low level objects (mainly Nebula
    servers) to whomever wants them.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nref.h"
#include "util/nlist.h"
#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#else
#include "kernel/ndefaultloghandler.h"
#endif

class nKernelServer;
class nTimeServer;
class nFileServer2;
class nRemoteServer;
class nScriptServer;
class nConServer;
class nGfxServer2;
class nSceneServer;
class nVariableServer;
class nResourceServer;
class nInputServer;
class nAnimationServer;
class nAudioServer3;
class nParticleServer;
class nParticleServer2;
class nGuiServer;
class nShadowServer2;
class nVideoServer;
class nLocaleServer;
class nCaptureServer;
class nPrefServer;
class nSqlServer;
class nRoot;

//------------------------------------------------------------------------------
namespace Foundation
{
class Server
{
public:
    /// resource pool types
    enum ResourcePoolType
    {
        GraphicsPool = 0,
        GuiPool,

        NumResourcePoolTypes,
    };

    /// constructor
    Server(const nString& vendor, const nString& app, const nString& logName);
    /// destructor
    ~Server();
    /// get instance pointer
    static Server* Instance();
    /// set the project directory
    void SetProjectDir(const nString& path);
    /// get the project directory
    const nString& GetProjectDir() const;
    /// get application name
    const nString& GetAppName() const;
    /// get vendor name
    const nString& GetVendorName() const;
    /// startup
    bool Open();
    /// shutdown
    void Close();
    /// get a pointer to a resource pool
    nRoot* GetResourcePool(ResourcePoolType type) const;
    /// get pointer to Nebula2 script server
    nScriptServer* GetScriptServer() const;

private:
    friend class RefCounted;

    bool isOpen;
    nString projDir;
    nString appName;
    nString vendorName;

    nKernelServer* kernelServer;
#ifdef __WIN32__
    nWin32LogHandler logHandler;
#else
    nDefaultLogHandler logHandler;
#endif
    nRef<nScriptServer>     scriptServer;
    nRef<nConServer>        consoleServer;
    nRef<nGfxServer2>       gfxServer;
    nRef<nSceneServer>      sceneServer;
    nRef<nVariableServer>   variableServer;
    nRef<nResourceServer>   resourceServer;
    nRef<nInputServer>      inputServer;
    nRef<nAnimationServer>  animationServer;
    nRef<nAudioServer3>     audioServer;
    nRef<nParticleServer>   particleServer;
    nRef<nParticleServer2>  particleServer2;
    nRef<nGuiServer>        guiServer;
    nRef<nShadowServer2>    shadowServer;
    nRef<nVideoServer>      videoServer;
    nRef<nLocaleServer>     localeServer;
    nRef<nCaptureServer>    captureServer;
    nRef<nPrefServer>       prefServer;
    nRef<nSqlServer>        sqlServer;
    nRef<nRoot>             resourcePools[NumResourcePoolTypes];

    static nList refCountedList;
    static Server* Singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    n_assert(0 != Server::Singleton);
    return Server::Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
Server::GetResourcePool(ResourcePoolType type) const
{
    n_assert((type >= 0) && (type < NumResourcePoolTypes));
    return this->resourcePools[type].get();
}

//------------------------------------------------------------------------------
/**
*/
inline
nScriptServer*
Server::GetScriptServer() const
{
    return this->scriptServer;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetAppName() const
{
    return this->appName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Server::GetVendorName() const
{
    return this->vendorName;
}

}; // namespace Foundation
//------------------------------------------------------------------------------
#endif
