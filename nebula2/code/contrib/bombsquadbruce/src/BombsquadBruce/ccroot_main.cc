//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccroot.h"
#include "BombsquadBruce/profile.h"
#include "BombsquadBruce/general.h"
#include "scene/nshapenode.h"
#include "BombsquadBruce/ccsceneserver.h"

nNebulaScriptClass(CCRoot, "nroot");

//------------------------------------------------------------------------------
CCRoot::CCRoot() :
    m_CollisionRadius(0),
    m_Rotation(0),
    m_Show( true )
{
}

CCRoot::~CCRoot()
{
    m_RenderContext.GetRootNode()->RenderContextDestroyed( &m_RenderContext );
}

//////////////////////////////////////////////
// public methods
//////////////////////////////////////////////

void CCRoot::Attach( nSceneServer& sceneServer, uint& frameId )
{
    if( m_Show )
    {
        m_RenderContext.SetFrameId( frameId++ );
        m_RenderContext.SetTransform( m_TransformMatrix );
        sceneServer.Attach( &m_RenderContext );
    }
}

/**
@brief Set the associated visual node for presentation.
*/
void CCRoot::SetShapeNode(const char* shapeNodePath)
{
	n_assert(shapeNodePath);
    nShapeNode* shapeNode = static_cast<nShapeNode*>( kernelServer->Lookup( shapeNodePath ) );
    m_RenderContext.SetRootNode( shapeNode );
    shapeNode->RenderContextCreated( &m_RenderContext );
    shapeNode->LoadResources(); // do this now so that we can e.g. check bounding box later without screwing up constness
}

const nShapeNode* CCRoot::GetShapeNode() const
{
    const nShapeNode* retVal = 0;
    const nSceneNode* sceneNode = m_RenderContext.GetRootNode();
    if( sceneNode )
    {
        retVal = static_cast<const nShapeNode*>( sceneNode );
    }
    return retVal;
}

/**
@brief Sets the position of the entity on the map.
*/
void CCRoot::SetPosition( const vector3 & pos )
{
	m_TransformMatrix.set_translation( pos ); // note that usually the world will set the height later, so you can just supply 0.0f for the y coordinate
}

/**
@brief Scales the entity.
Note that multiple calls are cumulative!
*/
void CCRoot::SetScale( const vector3 & scale )
{
    const vector3& oldPos = m_TransformMatrix.pos_component();
    m_TransformMatrix.set_translation( vector3() );
	m_TransformMatrix.scale( scale );
    m_TransformMatrix.set_translation( oldPos );
}

/**
@brief Points an render context object variable at the given path.
The path must already be valid when SetPathVariable is called.
*/
void CCRoot::SetPathVariable( nVariable::Handle handle, const char* NOH_Path )
{
     if( m_RenderContext.GetVariable( handle ) )
    {
        n_assert( "CCRoot::SetVariable -- var already exists!" );
        n_printf( "CCRoot::SetVariable -- var %d already exists!", handle );
    }
    else
    {
        nAutoRef<CCSceneServer> rScene = "/sys/servers/scene";
        nRoot* val = kernelServer->Lookup( NOH_Path );
        const nVariable var( handle, val );
        m_RenderContext.AddVariable( var );
    }
}

/**
@brief Sets the entity's y-axis rotation.
Assumes that the matrix isn't rotated along any other axis,
which is a very bad assumption indeed.
*/
void CCRoot::SetRotation( float angleInRad )
{    
    vector3 pos = m_TransformMatrix.pos_component();
    m_TransformMatrix.set_translation( vector3( ) );
    m_TransformMatrix.rotate_y( angleInRad );
    m_TransformMatrix.set_translation( pos );
    m_Rotation = angleInRad;
}

void CCRoot::SetCollisionRadius(float rad)
{
    n_assert( -1.0f == rad || 0.0f <= rad );
    m_CollisionRadius = rad;
}

vector3 CCRoot::GetPosition() const
{
	return m_TransformMatrix.pos_component();
}

float CCRoot::GetRotation() const
{
	return m_Rotation;
}

void CCRoot::SetTransformMatrix(const matrix44 & m)
{
	m_TransformMatrix = m;
}

float CCRoot::GetCollisionRadius() const
{
    if( -1.0f == m_CollisionRadius )
    {
        bbox3 box( GetShapeNode()->GetLocalBox() );
        const vector3& extents = box.extents();
        m_CollisionRadius = n_max( extents.x, extents.z );
    }
    n_assert( m_CollisionRadius >= 0.0f );
    return m_CollisionRadius;
}