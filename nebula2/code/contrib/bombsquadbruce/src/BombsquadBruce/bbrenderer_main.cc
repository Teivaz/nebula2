//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbrenderer.h"
nNebulaScriptClass(BBRenderer, "nroot");

#include "scene/nsceneserver.h"
#include "misc/nconserver.h"
#include "variable/nvariableserver.h"
#include "scene/nshapenode.h"
#include "mathlib/matrix.h"
#include <utility>
#include <algorithm>
#include "BombsquadBruce/bbroot.h"
#include "BombsquadBruce/general.h"
#include "gui/nguiserver.h"
#include "BombsquadBruce/BBCullingMgr.h"
#include "BombsquadBruce/BBCamera.h"
#include "spatialdb/nspatialelements.h"

//------------------------------------------------------------------------------

BBRenderer::BBRenderer() :
	m_FrameId(0),
	m_SceneServer( "/sys/servers/scene"),
	m_ConsoleServer( "/sys/servers/console"),
    m_rCullingMgr( "/sys/servers/culling" )
{
}

BBRenderer::~BBRenderer()
{
}

//------------------------------------------------------------------------------
/*
    @param  viewTransform         the current POV
    @param  cullingCamera         the camera that will handle culling
    @param  extraObjectsToRender  any visible objects not part of the static scene and not in the culling manager
    @param  debugVisualization    whether or not to show the culling frustum and object bounding boxes

    Usually, viewTransform is just cullingCamera.GetTransform(), but for debugging purposes it can be useful to observe
    the culling behaviour from the outside.
*/

void BBRenderer::Render(const matrix44& viewTransform, BBCamera& cullingCamera, const nArray< BBRef<BBRoot> >& extraObjectsToRender, bool debugVisualization )
{
    if(    !nGfxServer2::Instance()->InDialogBoxMode() // not that I ever use DialogBoxMode, but let's be on the safe side
        && m_SceneServer->BeginScene(viewTransform) )
    {
        nSceneServer& sceneServer = *m_SceneServer;
        const nVisibilityVisitor::VisibleElements& objectsToRender = m_rCullingMgr->GetVisibleElements( cullingCamera );
        BBRoot* pRoot;
        int i; // VC++ 6 compatibility
        for( i = objectsToRender.Size(); i > 0; --i )
        {
            pRoot = (BBRoot*)(objectsToRender[i-1]->GetPtr());
            pRoot->Attach( sceneServer, m_FrameId );
        }
        for( i = extraObjectsToRender.Size(); i > 0; --i )
        {
            extraObjectsToRender[i-1]->Attach( sceneServer, m_FrameId );
        }

        if( debugVisualization )
            cullingCamera.Attach( sceneServer, m_FrameId );

        m_StaticSceneContext.SetFrameId( m_FrameId++ );
        m_SceneServer->Attach( &m_StaticSceneContext );
        m_SceneServer->EndScene();
        m_ConsoleServer->Render();
        m_SceneServer->RenderScene();
         nGuiServer::Instance()->Render();

        if( debugVisualization )
        {
            m_rCullingMgr->Visualize();
        }

        m_SceneServer->PresentScene();
    }
}

////////////////////////////////////////////////////
void BBRenderer::SetScene(const char* scene_path)
{
	n_assert(scene_path);
	m_StaticSceneContext.SetRootNode( static_cast<nSceneNode*>( kernelServer->Lookup( scene_path ) ) );

}