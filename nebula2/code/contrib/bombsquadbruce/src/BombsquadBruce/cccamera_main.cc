//------------------------------------------------------------------------------
//  (C) 2003 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/cccamera.h"
#include "BombsquadBruce/ccroot.h"
#include "input/ninputserver.h"
#include "BombsquadBruce/general.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nscenenode.h"
#include "scene/nsceneserver.h"
#include "BombsquadBruce/CCCullingMgr.h"
#include "spatialdb/ngenarrayvisitors.h"

nNebulaScriptClass(CCCamera, "nroot");

//------------------------------------------------------------------------------
CCCamera::CCCamera() :
	m_ZMove(0.0f),
	m_VerticalSpin(0.0f),
	m_HorizontalSpin(0.0f),
	m_FwdBackMove(0.0f),
	m_LeftRightMove(0.0f),
    m_CullMode(FRUSTUM),
    m_rCullingMgr( "/sys/servers/culling" ),
    m_ManipulateCmdProto(0)
{
}

CCCamera::~CCCamera()
{
    if( m_RenderContext.IsValid() )
        m_RenderContext.GetRootNode()->RenderContextDestroyed( &m_RenderContext ); // TODO Make CCCamera child of a simpler CCRoot
}

void CCCamera::Trigger(double)
{ 
    m_ZMove = nInputServer::Instance()->GetSlider(m_ZoomInChannel.c_str()) - nInputServer::Instance()->GetSlider(m_ZoomOutChannel.c_str());
    m_VerticalSpin = nInputServer::Instance()->GetSlider(m_SpinUpChannel.c_str()) - nInputServer::Instance()->GetSlider(m_SpinDownChannel.c_str());
    m_HorizontalSpin = nInputServer::Instance()->GetSlider(m_SpinLeftChannel.c_str()) - nInputServer::Instance()->GetSlider(m_SpinRightChannel.c_str());
    m_FwdBackMove = nInputServer::Instance()->GetSlider(m_MoveFwdChannel.c_str()) - nInputServer::Instance()->GetSlider(m_MoveBackChannel.c_str());
    m_LeftRightMove = nInputServer::Instance()->GetSlider(m_MoveLeftChannel.c_str()) - nInputServer::Instance()->GetSlider(m_MoveRightChannel.c_str());
}

void CCCamera::SetControl( const std::string & control, const std::string & channel )
{
	if( control == "SPIN_LEFT" )
		m_SpinLeftChannel = channel;
	else if( control == "SPIN_RIGHT" )
		m_SpinRightChannel = channel;
	else if( control == "SPIN_UP" )
		m_SpinUpChannel = channel;
	else if( control == "SPIN_DOWN" )
		m_SpinDownChannel = channel;
	else if( control == "MOVE_LEFT" )
		m_MoveLeftChannel = channel;
	else if( control == "MOVE_RIGHT" )
		m_MoveRightChannel = channel;
	else if( control == "MOVE_FWD" )
		m_MoveFwdChannel = channel;
	else if( control == "MOVE_BACK" )
		m_MoveBackChannel = channel;
	else if( control == "ZOOM_IN" )
		m_ZoomInChannel = channel;
	else if( control == "ZOOM_OUT" )
		m_ZoomOutChannel = channel;

}

/**
    @brief Get camera position (in worldspace).
*/
vector3 CCCamera::GetPosition( ) const 
{
    return m_TransformMatrix.pos_component();
}

/**
    @brief Gets camera orientation as a quaternion
*/
quaternion CCCamera::GetOrientation() const
{
    return m_TransformMatrix.get_quaternion();
}

void CCCamera::SetDebugShape( const char* shapeNodePath )
{
	n_assert(shapeNodePath);
    nSceneNode* shapeNode = static_cast<nSceneNode*>( kernelServer->Lookup( shapeNodePath ) );
    m_RenderContext.SetRootNode( shapeNode );
    shapeNode->RenderContextCreated( &m_RenderContext );
}

void CCCamera::SetSphereCuller( const sphere& cullSphere )
{
    m_CullMode = SPHERE;
    m_CullSphereRadius = cullSphere.r;
    m_CullSphereOffset = cullSphere.p;
}

void CCCamera::SetFrustumCuller()
{
    m_CullMode = FRUSTUM;
}

/**
    Uses CCCullingMgr to perform a cull of game objects, and 
    calls Manipulate, on every object not culled.
    Note that culls aren't perfect!  They may include
    objects outside the cull zone.
*/

void CCCamera::ManipulateVisibleElements()
{
    const nVisibilityVisitor::VisibleElements& elements = m_rCullingMgr->GetVisibleElements(*this);
    int i; // VC6 compatibility
    for( i = elements.Size(); i > 0; --i )
        Manipulate( *((CCRoot*)(elements[i-1]->GetPtr() )) );
}

//-------------------------------------------------------------------
// nonscript public methods
//-------------------------------------------------------------------

void CCCamera::Attach( nSceneServer& sceneServer, uint& frameId )
{
    if( m_RenderContext.IsValid() )
    {
        m_RenderContext.SetFrameId( frameId++ );
        m_RenderContext.SetTransform( m_TransformMatrix );
        sceneServer.Attach( &m_RenderContext );
    }
}

nVisibilityVisitor* CCCamera::GetVisibilityVisitor( nVisibilityVisitor::VisibleElements& m_Elements ) const
{
    switch( m_CullMode )
    {
    case SPHERE:
        {
            // create sphere that has the desired offset relative to the current pos,
            // where that offset is aligned to the camera's orientation.
            matrix44 sphereTransform;
            sphereTransform.set_translation( m_CullSphereOffset );
            sphereTransform.mult_simple(GetTransform());
            sphere cullSphere(sphereTransform.pos_component(), m_CullSphereRadius);
            return n_new( nVisibleSphereGenArray( cullSphere, m_Elements ) );
        }
    case FRUSTUM:
        return n_new( nVisibleFrustumGenArray(*this, this->GetTransform(), m_Elements) );
    default:
        return 0;
    }
}

/**
    Calls the script command "DoManipulate" on the object.
*/
void CCCamera::Manipulate(nRoot& obj) 
{
    if( !m_ManipulateCmdProto )
        m_ManipulateCmdProto = GetClass()->FindScriptCmdByName( "DoManipulate" );
    n_assert( m_ManipulateCmdProto );
    nCmd* pCmd = m_ManipulateCmdProto->NewCmd();
    n_assert( pCmd );
    n_assert( 1 == pCmd->GetNumInArgs() );
    n_assert( obj.IsA(kernelServer->FindClass( "ccroot" ) ) );
    pCmd->In()->SetO( &obj );
    n_verify( Dispatch( pCmd ) );
    m_ManipulateCmdProto->RelCmd( pCmd );
}
