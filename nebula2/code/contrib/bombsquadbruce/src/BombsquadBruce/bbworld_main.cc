//------------------------------------------------------------------------------
//  (c) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbworld.h"
#include "map/nmap.h"
#include "BombsquadBruce/bbmobileactor.h"
#include "scene/nshapenode.h"
#include "BombsquadBruce/general.h"
#include "kernel/nfileserver2.h"
#include "mathlib/rectangle.h"
#include "BombsquadBruce/BBCullingMgr.h"

nNebulaScriptClass(BBWorld, "nroot");

//------------------------------------------------------------------------------
BBWorld::BBWorld() :
    m_GameActive( false ),
    m_InStartGameObj( NEW ),
    m_rCullingMgr( "/sys/servers/culling" )
{
}

BBWorld::~BBWorld()
{
}

///////////////////////////////////////////
// Script Methods
///////////////////////////////////////////

void BBWorld::StartGameObjs()
{ // add the basic game objects first to prevent collisions with them
    n_assert2( m_InStartGameObj == NEW, "StartGameObjs may only be called once!" );
    m_InStartGameObj = STARTED;
    m_rCullingMgr->StartElements();
}

bool BBWorld::AddGameObject( const char* path )
{
    n_assert2( STARTED == m_InStartGameObj, "Must call StartGameObjs before AddGameObject!" );

    bool retVal = false;
    BBRoot * pRoot = dynamic_cast<BBRoot*>( kernelServer->Lookup( path ) );
    if( pRoot )
    {
        SetOnGround( *pRoot );
        retVal = DoAddGameObject( *pRoot );
    }
    return retVal;
}

void BBWorld::EndGameObjs()
{
    n_assert2( m_InStartGameObj == STARTED, "EndGameObjs may only be called after StartGameObjs, and only once!" );
    m_Collision.ClearDynamicObjects(); // remove game-specific temporary objects
    m_rCullingMgr->EndElements();
    m_InStartGameObj = ENDED;
}

///////////////////////////////////////////
// Public Nonscript Methods
///////////////////////////////////////////

void BBWorld::Start()
{
    m_GameActive = true;
}

///////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////

bool BBWorld::DoAddGameObject( BBRoot& root )
{
    n_assert2( STARTED == m_InStartGameObj, "Must call StartGameObjs before DoAddGameObject!" );
    bool retVal = false;
    if( m_Collision.AddStaticObject( root ) )
    {
        // insert object into the global spatial database
        // from which the renderer will retrieve the objects
        // it needs in any given frame
        retVal = m_rCullingMgr->AddElement( root );
    }
    else
    {
        n_printf( "Unable to load '%s' -- position (%.0f,%.0f) is blocked.\n", root.GetName(), root.GetPosition().x, root.GetPosition().z );
    }
    return retVal;
}
///////////////////////////////////////////
// Private Methods
///////////////////////////////////////////

void BBWorld::SetOnGround( BBRoot& gameObj, float distFromGround, bool tiltToGround ) 
{ // distFromGround is used for objects whose position vector doesn't correspond to their base or feet (but rather e.g. to their midpoint)
    vector3 pos = gameObj.GetPosition();
    pos.y = GetHeight( pos.x, pos.z ) + distFromGround; // all game objects are assumed to rest on the map's surface (could parameterize)
    matrix44 m;
    if( tiltToGround )
        m = CalculateTiltToGround( pos ); // tilt to lie parallel to the map surface
    m.rotate_y( gameObj.GetRotation() );
    m.set_translation( pos );
    gameObj.SetTransformMatrix( m );
}
