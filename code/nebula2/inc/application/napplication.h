#ifndef N_APPLICATION_H
#define N_APPLICATION_H
//------------------------------------------------------------------------------
/**
    @class nApplication
    @ingroup Application

    @brief A simple application wrapper class. Derive a subclass for a
    specialized application.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/ncamera2.h"
#include "audio3/nlistener3.h"

class nAppState;
class nScriptServer;
class nGfxServer2;
class nInputServer;
class nConServer;
class nResourceServer;
class nSceneServer;
class nVariableServer;
class nAnimationServer;
class nParticleServer;
class nVideoServer;
class nGuiServer;
class nShadowServer2;
class nAudioServer3;
class nPrefServer;
class nLocaleServer;
class nAppCamera;

//------------------------------------------------------------------------------
class nApplication : public nRoot
{
public:
    /// constructor
    nApplication();
    /// destructor
    virtual ~nApplication();
    /// return instance pointer
    static nApplication* Instance();
    /// set the project directory (if different from home:)
    void SetProjectDirectory(const nString& n);
    /// get project directory
    const nString& GetProjectDirectory() const;
    /// set company name
    void SetCompanyName(const nString& n);
    /// get company name
    const nString& GetCompanyName() const;
    /// set application name
    void SetAppName(const nString& n);
    /// get application name
    const nString& GetAppName() const;
    /// set startup script name
    void SetStartupScript(const nString& s);
    /// get startup script name
    const nString& GetStartupScript() const;
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set optional render path (else autoselect)
    void SetRenderPath(const nString& p);
    /// get optional render path
    const nString& GetRenderPath() const;
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// run the application
    virtual void Run();
    /// set the quit requested flag
    void SetQuitRequested(bool b);
    /// get the quit requested flag
    bool GetQuitRequested() const;
    /// set current application state (will be activated at beginning of next frame)
    void SetState(const nString& n);
    /// get current application state
    nString GetCurrentState() const;
    /// find app state by name
    nAppState* FindState(const nString& name) const;
    /// get current system time
    nTime GetTime() const;
    /// get current frame time
    nTime GetFrameTime() const;
    /// get current state time (time since last state switch)
    nTime GetStateTime() const;
    /// set the locale table filename
    void SetLocaleTable(const nString& s);
    /// get the locale table filename
    const nString& GetLocaleTable() const;
    /// get app camera
    nAppCamera* GetAppCamera() const;

protected:
    /// create a new state object
    nAppState* CreateState(const nString& className, const nString& stateName);
    /// create script server object, override in subclass as needed
    virtual nScriptServer* CreateScriptServer();
    /// create graphics server, override in subclass as needed
    virtual nGfxServer2* CreateGfxServer();
    /// create input server, override in subclass as needed
    virtual nInputServer* CreateInputServer();
    /// create console server, override in subclass as needed
    virtual nConServer* CreateConsoleServer();
    /// create resource server, override in subclass as needed
    virtual nResourceServer* CreateResourceServer();
    /// create scene server, override in subclass as needed
    virtual nSceneServer* CreateSceneServer();
    /// create variable server, override in subclass as needed
    virtual nVariableServer* CreateVariableServer();
    /// create animation server, override in subclass as needed
    virtual nAnimationServer* CreateAnimationServer();
    /// create particle server, override in subclass as needed
    virtual nParticleServer* CreateParticleServer();
    /// create video server, override in subclass as needed
    virtual nVideoServer* CreateVideoServer();
    /// create gui server, override in subclass as needed
    virtual nGuiServer* CreateGuiServer();
    /// create shadow server, override in subclass as needed
    virtual nShadowServer2* CreateShadowServer();
    /// create audio server, override in subclass as needed
    virtual nAudioServer3* CreateAudioServer();
    /// create preferences server, override in subclass as needed
    virtual nPrefServer* CreatePrefServer();
    /// create the locale server, override in subclass as needed
    virtual nLocaleServer* CreateLocaleServer();
    /// create camera object, override in subclass as needed
    virtual nAppCamera* CreateAppCamera();
    /// update time values
    virtual void OnUpdateTime();
    /// perform a state transition
    virtual void DoStateTransition();
    /// do one complete frame
    virtual void DoFrame();
    /// called when 3d rendering should be performed
    virtual void OnRender3D();
    /// called when 2d rendering should be performed
    virtual void OnRender2D();
    /// called before nSceneServer::RenderScene()
    virtual void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    virtual void OnFrameRendered();

private:
    static nApplication* Singleton;

    bool isOpen;
    bool quitRequested;

    nRef<nScriptServer>     refScriptServer;
    nRef<nGfxServer2>       refGfxServer;
    nRef<nInputServer>      refInputServer;
    nRef<nConServer>        refConServer;
    nRef<nResourceServer>   refResourceServer;
    nRef<nSceneServer>      refSceneServer;
    nRef<nVariableServer>   refVariableServer;
    nRef<nAnimationServer>  refAnimationServer;
    nRef<nParticleServer>   refParticleServer;
    nRef<nVideoServer>      refVideoServer;
    nRef<nGuiServer>        refGuiServer;
    nRef<nShadowServer2>    refShadowServer;
    nRef<nAudioServer3>     refAudioServer;
    nRef<nPrefServer>       refPrefServer;
    nRef<nLocaleServer>     refLocaleServer;
    
    nRef<nRoot> refAppStates;
    nRef<nAppState> curState;
    nRef<nAppState> nextState;

    nString projDir;
    nString companyName;
    nString appName;
    nString startupScript;
    nString localeTable;
    nDisplayMode2 displayMode;
    nCamera2 gfxCamera;
    nString renderPath;
    
    nTime stateTransitionTime;
    nTime time;
    nTime stateTime;
    nTime frameTime;

    nListener3 audioListener;
    nAppCamera *appCamera;
};

//------------------------------------------------------------------------------
/**
*/
inline
nApplication*
nApplication::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nApplication::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetProjectDirectory(const nString& n)
{
    this->projDir = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetProjectDirectory() const
{
    return this->projDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetCompanyName(const nString& n)
{
    this->companyName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetCompanyName() const
{
    return this->companyName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetAppName(const nString& n)
{
    this->appName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetAppName() const
{
    return this->appName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetStartupScript(const nString& n)
{
    this->startupScript = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetStartupScript() const
{
    return this->startupScript;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nApplication::GetDisplayMode() const
{
    return this->displayMode;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetRenderPath(const nString& p)
{
    this->renderPath = p;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetRenderPath() const
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nApplication::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nApplication::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nApplication::GetStateTime() const
{
    return this->stateTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetQuitRequested(bool b)
{
    this->quitRequested = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nApplication::GetQuitRequested() const
{
    return this->quitRequested;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nApplication::SetLocaleTable(const nString& s)
{
    this->localeTable = s;
}

//------------------------------------------------------------------------------
inline
const nString& 
nApplication::GetLocaleTable() const
{
    return this->localeTable;
}

//------------------------------------------------------------------------------
/**
    Retrives application camera.

    - 08-Jun-05    kims    Added.
*/
inline
nAppCamera* 
nApplication::GetAppCamera() const
{
    return this->appCamera;
}
//------------------------------------------------------------------------------
#endif
