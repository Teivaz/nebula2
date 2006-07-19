//------------------------------------------------------------------------------
//  graphics/server.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "graphics/server.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "variable/nvariableserver.h"
#include "resource/nresourceserver.h"
#include "misc/nconserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nscriptserver.h"
#include "graphics/cell.h"
#include "graphics/entity.h"
#include "graphics/resource.h"
#include "graphics/level.h"
#include "graphics/cameraentity.h"
#include "video/nvideoserver.h"
#include "util/celltreebuilder.h"
#include "misc/ncaptureserver.h"
#include "db/server.h"
#include "attr/attributes.h"
#include "foundation/factory.h"

namespace Graphics
{
ImplementRtti(Graphics::Server, Foundation::RefCounted);
ImplementFactory(Graphics::Server);

Server* Server::Singleton = 0;
const float Server::maxLodDistThreshold = 10000.0f;
const float Server::minLodSizeThreshold = 0.0f;

//------------------------------------------------------------------------------
/**
*/
Server::Server() :
    #if __NEBULA_STATS__    
    numShapesVisible("mangaShapesVisible", nArg::Int),
    numLightsVisible("mangaLightVisible", nArg::Int),
    numShapesLit("mangaShapesLit", nArg::Int),
    numCellsVisitedCamera("mangaCellsVisitedCamera", nArg::Int),
    numCellsVisitedLight("mangaCellsVisitedLight", nArg::Int),
    numCellsOutsideCamera("mangaCellsOutsideCamera", nArg::Int),
    numCellsOutsideLight("mangaCellsOutsideLight", nArg::Int),
    numCellsVisibleCamera("mangaCellsVisibleCamera", nArg::Int),
    numCellsVisibleLight("mangaCellsVisibleLight", nArg::Int),
    #endif
    isOpen(false),
    frameId(0),
    time(0.0),
    frameTime(0.0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
Server::~Server()
{
    n_assert(!this->isOpen);
    n_assert(this->curLevel == 0);
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Set the current level object. This will decrement the refcount of
    the previous current level object (if one is set), and increments
    the refcount of the new level object. A 0 pointer argument is allowed,
    this will just release the previously set level.

    @param  level   pointer to a Level object
*/
void
Server::SetLevel(Level* level)
{
    n_assert(this->isOpen);
    if (this->curLevel != 0)
    {
        this->curLevel = 0;
    }
    if (level)
    {
        this->curLevel = level;
    }
}

//------------------------------------------------------------------------------
/**
    Get pointer to current level object. This will not change the refcount
    of the returned object.

    @return     pointer to the current level object (can be 0)
*/
Level*
Server::GetLevel() const
{
    return this->curLevel.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    Initialize the graphics subsystem and open the display.
*/
bool
Server::Open()
{
    n_assert(!this->isOpen);
    nFileServer2* fileServer = nFileServer2::Instance();
    nScriptServer* scriptServer = Foundation::Server::Instance()->GetScriptServer();
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nSceneServer* sceneServer = nSceneServer::Instance();

    this->isOpen = true;

    // initialize the gfx server
    if (this->featureSet.IsValid())
    {
        gfxServer->SetFeatureSetOverride(nGfxServer2::StringToFeatureSet(this->featureSet.Get()));
    }
    gfxServer->SetDisplayMode(this->displayMode);

    // run graphics startup script function (before opening the display!)
    nString result;
    scriptServer->Run("OnGraphicsStartup", result);
    if (this->renderPath.IsValid())
    {
        sceneServer->SetRenderPathFilename(this->renderPath);
    }

    // open the scene server (will also open the display server)
    sceneServer->SetObeyLightLinks(true);
    if (!sceneServer->Open())
    {
        n_error("Graphics::Server::Startup: Failed to open nSceneServer!");
        return false;
    }
    nVideoServer::Instance()->Open();

    // create the anim table object
    if (nFileServer2::Instance()->FileExists("data:tables/anims.xml"))
    {
        this->animTable = AnimTable::Create();
        this->animTable->SetFilename("data:tables/anims.xml");
        this->animTable->Open();
    }
    else
    {
        n_printf("Graphics::Server::Open(): Warning, data:tables/anims.xml doesn't exist!\n");
    }

    // blank screen
    gfxServer->BeginFrame();
    gfxServer->BeginScene();
    gfxServer->Clear(nGfxServer2::AllBuffers, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0);
    gfxServer->EndScene();
    gfxServer->PresentScene();
    gfxServer->EndFrame();

    return true;
}

//------------------------------------------------------------------------------
/**
    Shutdown the graphics subsystem.
*/
void
Server::Close()
{
    n_assert(this->isOpen);
    nScriptServer* scriptServer = Foundation::Server::Instance()->GetScriptServer();

    // run graphics startup script function
    nString result;
    scriptServer->Run("OnGraphicsShutdown", result);

    // close the anim table object
    if (nFileServer2::Instance()->FileExists("data:tables/anims.xml"))
    {
        this->animTable->Close();
    }

    // close the Nebula2 servers
    nVideoServer::Instance()->Close();
    nSceneServer::Instance()->Close();

    // close the display and release Nebula servers
    this->SetLevel(0);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Trigger the graphics subsystem. This will not perform any rendering,
    instead, the Windows message pump will be "pumped" in order to
    process any outstanding window system messages. The method returns
    false if the window system asks the application to quit (either because
    the user has pressed the Close button, or hits Alt-F4).

    @return     true as long as application should continue to run
*/
bool
Server::Trigger()
{
    return nGfxServer2::Instance()->Trigger();
}

//------------------------------------------------------------------------------
/**
    Begin rendering the scene. You should only call Render() and EndScene()
    when BeginRender() returns true.
*/
bool
Server::BeginRender()
{
    n_assert(this->isOpen);
    n_assert(this->curLevel != 0);

    // don't render if currently playing video or display GDI dialog boxes
    if (nGfxServer2::Instance()->InDialogBoxMode())
    {
        return false;
    }

    // reset debug watchers
    #if __NEBULA_STATS__
    this->numShapesVisible->SetI(0);
    this->numLightsVisible->SetI(0);
    this->numShapesLit->SetI(0);
    this->numCellsVisitedCamera->SetI(0);
    this->numCellsVisitedLight->SetI(0);
    this->numCellsOutsideCamera->SetI(0);
    this->numCellsOutsideLight->SetI(0);
    this->numCellsVisibleCamera->SetI(0);
    this->numCellsVisibleLight->SetI(0);
    #endif

    // update frame id
    this->frameId++;

    if (!this->curLevel->BeginRender())
    {
        // clear links
        this->curLevel->EndRender();
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Render the current frame. This doesn't make the frame visible.
    You can do additional rendering after Render() returns. Finalize
    rendering by calling EndRender(), this will also make the frame
    visible.
*/
void
Server::Render()
{
    // ask current level object to perform rendering
    this->curLevel->Render();
}

//------------------------------------------------------------------------------
/**
    Render the debug visualization of the graphics subsystem.
*/
void
Server::RenderDebug()
{
    this->curLevel->RenderDebug();
}

//------------------------------------------------------------------------------
/**
    Finish rendering and present the scene.
*/
void
Server::EndRender()
{
    this->curLevel->EndRender();    
    nCaptureServer::Instance()->Trigger();
    nSceneServer::Instance()->PresentScene();
}

} // namespace Graphics
