//------------------------------------------------------------------------------
//  foundation/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "foundation/server.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nremoteserver.h"
#include "kernel/nscriptserver.h"
#include "misc/nconserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "variable/nvariableserver.h"
#include "input/ninputserver.h"
#include "resource/nresourceserver.h"
#include "anim2/nanimationserver.h"
#include "audio3/naudioserver3.h"
#include "particle/nparticleserver.h"
#include "particle/nparticleserver2.h"
#include "gui/nguiserver.h"
#include "shadow2/nshadowserver2.h"
#include "video/nvideoserver.h"
#include "locale/nlocaleserver.h"
#include "foundation/refcounted.h"
#include "file/nnpkfileserver.h"
#include "misc/ncaptureserver.h"
#include "misc/nprefserver.h"
#include "sql/nsqlserver.h"

using namespace Foundation;

nList Server::refCountedList;
Server* Server::Singleton = 0;

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndsound);
nNebulaUsePackage(ngui);
nNebulaUsePackage(ndshow);

//------------------------------------------------------------------------------
/**
*/
Server::Server(const nString& vendor, const nString& app, const nString& logName) :
    isOpen(false),
    vendorName(vendor),
    appName(app),
    kernelServer(0),
    logHandler(logName.Get())
{
    n_assert(0 == Server::Singleton);
    Server::Singleton = this;

    this->kernelServer = n_new(nKernelServer);
    this->kernelServer->AddPackage(nnebula);
    this->kernelServer->AddPackage(ndinput8);
    this->kernelServer->AddPackage(ndirect3d9);
    this->kernelServer->AddPackage(ndsound);
    this->kernelServer->AddPackage(ngui);
    this->kernelServer->AddPackage(ndshow);

    this->kernelServer->SetLogHandler(&(this->logHandler));
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);

    // check if the refcounted list is empty
    RefCounted* curObj;
    int numRefLeaks = 0;
    while (curObj = (RefCounted*) refCountedList.RemHead())
    {
        n_printf("Object at address '%lx' still referenced (refcount = %d), class '%s'\n",
            curObj, curObj->GetRefCount(), curObj->GetClassName());
        numRefLeaks++;
    }
    /*
    if (numRefLeaks > 0)
    {
        n_message("There were %d objects still referenced, check log for details!", numRefLeaks);
    }
    */

    this->kernelServer->SetLogHandler(0);
    n_delete(this->kernelServer);
    this->kernelServer = 0;

    n_assert(0 != Server::Singleton);
    Server::Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Set the project directory. The default project directory is identical
    with the home: assign.

    @param  path    project directory, must end with a /
*/
void
Server::SetProjectDir(const nString& path)
{
    this->projDir = path;
}

//------------------------------------------------------------------------------
/**
    Return the current project directory.

    @return     the project directory, can be 0
*/
const nString&
Server::GetProjectDir() const
{
    return this->projDir;
}

//------------------------------------------------------------------------------
/**
    Initializes the Foundation subsystem.
*/
bool
Server::Open()
{
    n_assert(this->kernelServer);
    n_assert(!this->isOpen);

    // replace nFileServer2 with nNpkFileServer
    kernelServer->ReplaceFileServer("nnpkfileserver");
    nNpkFileServer* npkFileServer = (nNpkFileServer*) nFileServer2::Instance();
    npkFileServer->ParseDirectory("home:", "npk");
    npkFileServer->ParseDirectory("home:export", "npk");
    npkFileServer->ParseDirectory("home:data", "npk");

    // create servers
    this->scriptServer    = (nScriptServer*)     this->kernelServer->New("ntclserver",        "/sys/servers/script");
    this->consoleServer   = (nConServer*)        this->kernelServer->New("nconserver",        "/sys/servers/console");
    this->gfxServer       = (nGfxServer2*)       this->kernelServer->New("nd3d9server",       "/sys/servers/gfx");
    this->sceneServer     = (nSceneServer*)      this->kernelServer->New("nsceneserver",      "/sys/servers/scene");
    this->variableServer  = (nVariableServer*)   this->kernelServer->New("nvariableserver",   "/sys/servers/variable");
    this->resourceServer  = (nResourceServer*)   this->kernelServer->New("nresourceserver",   "/sys/servers/resource");
    this->inputServer     = (nInputServer*)      this->kernelServer->New("ndi8server",        "/sys/servers/input");
    this->animationServer = (nAnimationServer*)  this->kernelServer->New("nanimationserver",  "/sys/servers/anim");
    this->audioServer     = (nAudioServer3*)     this->kernelServer->New("ndsoundserver3",    "/sys/servers/audio");
    this->particleServer  = (nParticleServer*)   this->kernelServer->New("nparticleserver",   "/sys/servers/particle");
    this->particleServer2 = (nParticleServer2*)  this->kernelServer->New("nparticleserver2",  "/sys/servers/particle2");
    this->guiServer       = (nGuiServer*)        this->kernelServer->New("nguiserver",        "/sys/servers/gui");
    this->shadowServer    = (nShadowServer2*)    this->kernelServer->New("nshadowserver2",    "/sys/servers/shadow");
    this->videoServer     = (nVideoServer*)      this->kernelServer->New("ndshowserver",      "/sys/servers/video");
    this->localeServer    = (nLocaleServer*)     this->kernelServer->New("nlocaleserver",     "/sys/servers/locale");
    this->captureServer   = (nCaptureServer*)    this->kernelServer->New("ncaptureserver",    "/sys/servers/capture");
    this->prefServer      = (nPrefServer*)       this->kernelServer->New("nwin32prefserver",  "/sys/servers/pref");
    this->sqlServer       = (nSqlServer*)        this->kernelServer->New("nsqlite3server",    "/sys/servers/sql");

    // set app and vendor name in preference server
    this->prefServer->SetCompanyName(this->vendorName.Get());
    this->prefServer->SetApplicationName(this->appName.Get());

    // setup "appdata:" assign
    nFileServer2* fileServer = this->kernelServer->GetFileServer();
    nString appdata;
    appdata.Format("user:%s/%s", this->vendorName.Get(), this->appName.Get());
    fileServer->SetAssign("appdata", appdata);

    // setup capture server
    nString captureDir;
    captureDir.Format("appdata:capture", this->vendorName.Get(), this->appName.Get());
    this->captureServer->SetBaseDirectory(captureDir);

    // create resource pools
    this->resourcePools[GraphicsPool] = this->kernelServer->New("nroot", "/res/gfx");
    this->resourcePools[GuiPool]      = this->kernelServer->New("nroot", "/res/gui");

    // setup the "proj:" assign
    if (this->GetProjectDir().IsValid())
    {
        fileServer->SetAssign("proj", this->GetProjectDir());
    }
    else
    {
        fileServer->SetAssign("proj", fileServer->GetAssign("home"));
    }

    // open locale servers
    this->localeServer->Open();

    // run startup script
    nString result;
    if (this->GetProjectDir().IsValid())
    {
        this->scriptServer->RunScript("proj:data/scripts/startup.tcl", result);
    }
    else
    {
        this->scriptServer->RunScript("home:data/scripts/startup.tcl", result);
    }

    // call OnStartup script function
    this->scriptServer->Run("OnStartup", result);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Shutdown the application server. This method must be called AFTER
    all other subsystems are shutdown.
*/
void
Server::Close()
{
    n_assert(this->isOpen);
    n_assert(this->kernelServer);

    // call OnShutdown script function
    nString result;
    this->scriptServer->Run("OnShutdown", result);

    // release resource pools
    int i;
    for (i = 0; i < NumResourcePoolTypes; i++)
    {
        this->resourcePools[i]->Release();
        n_assert(!this->resourcePools[i].isvalid());
    }

    // release servers
    this->sqlServer->Release();
    this->captureServer->Release();
    this->shadowServer->Release();
    this->videoServer->Release();
    this->guiServer->Release();
    this->particleServer->Release();
    this->particleServer2->Release();
    this->audioServer->Release();
    this->animationServer->Release();
    this->inputServer->Release();
    this->variableServer->Release();
    this->sceneServer->Release();
    this->gfxServer->Release();
    this->resourceServer->Release();
    this->consoleServer->Release();
    this->scriptServer->Release();

    this->isOpen = false;
}
