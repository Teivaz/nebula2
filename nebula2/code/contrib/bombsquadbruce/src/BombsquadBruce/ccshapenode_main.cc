//------------------------------------------------------------------------------
//  ccshapenode_main.cc
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccshapenode.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "variable/nvariableserver.h"

nNebulaScriptClass(CCShapeNode, "nshapenode");

//------------------------------------------------------------------------------
CCShapeNode::CCShapeNode() :
    //m_FisheyeCentreHandle( -1 ),
    m_LockViewerXZ( false ),
    m_TexCoordScale( -1.0f )
{
}

//void CCShapeNode::SetFisheyeCentreVarName( const char* varName )
//{
//    m_FisheyeCentreHandle = nVariableServer::Instance()->FindVariableHandleByName( varName );
//    n_assert( nVariable::InvalidHandle != m_FisheyeCentreHandle );
//}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
CCShapeNode::RenderTransform(nSceneServer* sceneServer, 
                                nRenderContext* renderContext, 
                                const matrix44& parentMatrix)
{
    bool retVal = nShapeNode::RenderTransform( sceneServer, renderContext, parentMatrix ); 
    if( retVal ) 
    {           
        if( this->GetLockViewerXZ() )
        {
            matrix44 m = this->tform.getmatrix();
            // this part ensures that we don't move relative to the camera
            const matrix44& viewMatrix = this->refGfxServer->GetTransform(nGfxServer2::InvView);
            m.M41 = viewMatrix.M41;
            m.M43 = viewMatrix.M43;
            // this part provided data that allows the shader to move the texture around so
            // so that we _appear_ to move relative to the camera
            if( m_TexCoordScale > 0.0f )
            {
                const matrix44& m44 = this->refGfxServer->GetTransform(nGfxServer2::View);
                matrix33 m33( viewMatrix.x_component(), viewMatrix.y_component(), viewMatrix.z_component() );
                const vector3& camPos = m44.pos_component();
                vector3 newCamPos;
                m33.mult( camPos, newCamPos );
                vector4 param( -fmodf( newCamPos.x, m_TexCoordScale ) / m_TexCoordScale, fmodf( newCamPos.z, m_TexCoordScale ) / m_TexCoordScale, 0.0f, 0.0f );
                // z and w are unused, since we're only going to add param to the texture's uv components.
                SetVector( nShaderState::TexGenS, param );
            }
            sceneServer->SetModelTransform(m * parentMatrix);
        }
        //if( this->GetDoFisheye() )
        //{
        //    const nFloat4& centrePos = nVariableServer::Instance()->GetGlobalVariableContext().GetVariable( m_FisheyeCentreHandle )->GetFloat4();
        //    SetVector( nShader2::TexGenT, vector4( centrePos.x, centrePos.y, centrePos.z, centrePos.w ) );
        //}
    }
    return retVal;
}
