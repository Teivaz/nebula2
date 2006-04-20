#ifndef VIEWER_VIEWERAPP_H
#define VIEWER_VIEWERAPP_H
//------------------------------------------------------------------------------
/**
    @class Viewer::ViewerApp

    A generic Mangalore viewer application, similar to the Nebula2 viewer,
    but which also supports Physics.

    (C) 2005 RadonLabs GmbH
*/
#include "application/app.h"
#include "physics/boxshape.h"
//#include "application/dsaappmsghandler.h"
//------------------------------------------------------------------------------
namespace Viewer
{
class ViewerApp : public Application::App
{
public:
    /// constructor
    ViewerApp();
    /// destructor
    virtual ~ViewerApp();
	/// get instance pointer
	static ViewerApp* Instance();
    /// returns the application name (override in subclass)
    virtual nString GetAppName() const;
    /// returns the version string (override in subclass)
    virtual nString GetAppVersion() const;
    /// returns vendor name (override in subclass)
    virtual nString GetVendorName() const;
    /// open application
    virtual bool Open();
    /// close the application
    virtual void Close();

private:
    /// setup application defaults, override in subclass
    virtual void SetupFromDefaults();
	/// parse command line args, override in subclass if new args are desired
	virtual void SetupFromCmdLineArgs();
    /// called per frame by Run(), override in subclass as needed
    virtual void OnFrame();
    /// setup the gui system
    virtual void SetupGui();
    /// setup application state handlers
    virtual void SetupStateHandlers();
	/// read the project registry key
    nString ReadProjRegistryKey();
	/// set resource name of display object
	void SetObjectResourceName(const nString& p);
    /// get resource name of display object
    const nString& GetObjectResourceName() const;
    /// set lights in scene
    void SetupLightsInScene();
    /// set camera in scene
    void SetupCamera();
    /// set initial camera pos
    void SetCameraPos(const vector3& v);
    /// get initial camera pos
    const vector3& GetCameraPos() const;
    /// set initial camera center of interest
    void SetCameraCoi(const vector3& v);
    /// get initial camera center of interest
    const vector3& GetCameraCoi() const;
    /// set initial camera up vector
    void SetCameraUpVec(const vector3& v);
    /// get initial camera up vector
    const vector3& GetCameraUpVec() const;
    /// force relevant resource assigns to temp:
    void SetForceTempAssigns(bool);
    /// get force temp assign status
    bool GetForceTempAssigns() const;
    /// set physics in scene
    void SetupGround();
    /// redirect resources, needed when acting as previewer
    void RedirectResourceForPreview();
    /// creates the Entities to view
    bool CreateViewedEntity();

    static ViewerApp* Singleton;
	nString objectResourceName;
    vector3 cameraPos;
    vector3 cameraCoi;
    vector3 cameraUpVec;
    Game::Entity* loadedEntity;
    bool forceTempAssigns;
};
//------------------------------------------------------------------------------
/**
*/
inline
void
ViewerApp::SetObjectResourceName(const nString& p)
{
    this->objectResourceName = p;
}


//------------------------------------------------------------------------------
/**
*/
inline
const nString&
ViewerApp::GetObjectResourceName() const
{
    return this->objectResourceName;
}
//------------------------------------------------------------------------------
/**
*/
inline
ViewerApp*
ViewerApp::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ViewerApp::SetCameraPos(const vector3& v)
{
    this->cameraPos = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ViewerApp::GetCameraPos() const
{
    return this->cameraPos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ViewerApp::SetCameraCoi(const vector3& v)
{
    this->cameraCoi = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ViewerApp::GetCameraCoi() const
{
    return this->cameraCoi;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ViewerApp::SetCameraUpVec(const vector3& v)
{
    this->cameraUpVec = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ViewerApp::GetCameraUpVec() const
{
    return this->cameraUpVec;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
ViewerApp::SetForceTempAssigns(bool b)
{
    this->forceTempAssigns = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ViewerApp::GetForceTempAssigns() const
{
    return this->forceTempAssigns;
}
};
//------------------------------------------------------------------------------
#endif

