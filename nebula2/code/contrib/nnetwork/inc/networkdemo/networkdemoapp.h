/**
   @file networkdemoapp.h
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Declaration of NetworkDemoApp class.
   @brief $Id$ 

   This file is licensed under the terms of the Nebula License.
*/
#ifndef N_NETWORKDEMOAPP_H
#define N_NETWORKDEMOAPP_H
//------------------------------------------------------------------------------
#include "gfx2/ndisplaymode2.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "resource/nresourceserver.h"
#include "misc/nconserver.h"
#include "misc/nwatched.h"
#include "scene/nsceneserver.h"
#include "scene/nrendercontext.h"
#include "scene/ntransformnode.h"
#include "network/nsessionserver.h"
#include "network/nsessionclient.h"
#include "networkdemo/netserverdemo.h"
#include "networkdemo/netclientdemo.h"
#include "variable/nvariableserver.h"
//------------------------------------------------------------------------------
/**
    @class NetworkDemoApp
    @ingroup NetworkDemoContribModule

    @brief A simple viewer app class modified for NetworkDemo use.
*/
class NetworkDemoApp
{
public:
    /// constructor
    NetworkDemoApp(nKernelServer* ks);
    /// destructor
    virtual ~NetworkDemoApp();
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// open the viewer
    bool Open();
    /// close the viewer
    void Close();
    /// run the viewer, returns when app should exit
    void Run();

protected:
    /// handle general input
    void HandleInput(float frameTime);

private:
    nKernelServer*      kernelServer;
    nRef<nScriptServer> refScriptServer;
    nRef<nGfxServer2>   refGfxServer;
    nRef<nResourceServer> refResourceServer;
    nRef<nInputServer>  refInputServer;
    nRef<nConServer>    refConServer;
    nRef<nSceneServer>  refSceneServer;
    nRef<nVariableServer> refVarServer;
    nRef<nSessionServer>  refSessionServer;
    nRef<nSessionClient>  refSessionClient;
    nRef<NetClientDemo> refNetClient;
    nRef<NetServerDemo> refNetServer;

    nRef<nTransformNode> refRootNode;

    int index;

    bool isOpen;
    bool running;
    nDisplayMode2 displayMode;
    nCamera2 camera;

    nRenderContext renderContext;
    matrix44 viewMatrix;

    nGfxServer2::FeatureSet featureSetOverride;

    // debug variables to show in the app
    nWatched netserverSessionOpen;
    nWatched netserverSessionNumClients;
    nWatched netclientSessionOpen;
    nWatched netclientSessionNumServers;
    nWatched netclientSessionName;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
NetworkDemoApp::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

#endif//N_NETWORKDEMOAPP_H
//------------------------------------------------------------------------------
//   EOF 
//------------------------------------------------------------------------------
