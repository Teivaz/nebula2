//------------------------------------------------------------------------------
//  (c) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccofmworld.h"
#include "map/nmap.h"
#include "BombsquadBruce/ccwater.h"
#include "BombsquadBruce/ccmobileactor.h"

nNebulaScriptClass(CCOFMWorld, "ccworld");


///////////////////////////////////////////
// Basic Methods
///////////////////////////////////////////
CCOFMWorld::CCOFMWorld() :
    m_MinCollisionHeight( 100.0f ),
    m_CollisionScript( 0 )
{
}

/**
@brief Run world for time interval dt.

*/
void CCOFMWorld::Trigger( float dt )
{
    if( IsGameActive() )
    {
        m_rWater->Trigger( dt );
    }
    DoPlayer( dt );
}

///////////////////////////////////////////
// Script Methods
///////////////////////////////////////////

void CCOFMWorld::SetMap(const char* path)
{
    n_assert(path);
    m_rMap = path;
    m_rMap->LoadMap();
}

float CCOFMWorld::GetHeight(float x, float z) /*const*/
{
    return m_rMap->GetHeight(x, z);
}

void CCOFMWorld::StartGameObjs()
{ // add the basic game objects first to prevent collisions with them
    CCWorld::StartGameObjs();
    m_AlwaysVisibleGameObjs.Append( CCRef<CCRoot>( m_rPlayer ) );
    m_Collision.AddDynamicObject( *m_rPlayer ); // to prevent randomly created scenery objects from appearing atop the player
    m_AlwaysVisibleGameObjs.Append( CCRef<CCRoot>( m_rWater ) );
}

bool CCOFMWorld::AddGameObject( const char* path )
{
    bool retVal = false;
    CCRoot * pRoot = dynamic_cast<CCRoot*>( kernelServer->Lookup( path ) );
    const float height = GetHeight( pRoot->GetPosition().x, pRoot->GetPosition().z );
    if( height <= m_rWater->GetWaterLevel() )
    {
        n_printf( "Unable to load '%s' -- position (%.0f,%.0f) is underwater (height %.0f).\n", pRoot->GetName(), pRoot->GetPosition().x, pRoot->GetPosition().z, height );
    }
    else
    {
        retVal = CCWorld::AddGameObject( path );
    }
    return retVal;
}

void CCOFMWorld::EndGameObjs()
{
    m_Collision.CullShortObjects( m_MinCollisionHeight ); // don't want to trip over a shrub
    CCWorld::EndGameObjs();
}

void CCOFMWorld::SetPlayer( const char* playerPath )
{
    m_rPlayer = playerPath;
}

void CCOFMWorld::SetWater( const char* path )
{
    m_rWater = path;
}

///////////////////////////////////////////
// Public Nonscript Methods
///////////////////////////////////////////

void CCOFMWorld::Start()
{
    CCWorld::Start();
}

///////////////////////////////////////////
// Private Methods
///////////////////////////////////////////

void CCOFMWorld::DoPlayer( float dt )
{
    m_rPlayer->Trigger(dt);
    if( IsGameActive() )
    {
        const vector3& curPos = m_rPlayer->GetPosition(); // we may change this in the course of this method
        HandleCollision( *m_rPlayer, curPos );
    }
}

void CCOFMWorld::HandleCollision( CCMobileActor& player, const vector3& curPos )
{
    const CCCollision::CollVolT* collVol = m_Collision.GetCollision( player );
    if( collVol ) 
    {
        const vector3& prevPos = player.GetPreviousPosition();
        player.SetPosition( vector3( prevPos.x, curPos.y, prevPos.z ) );
        if( m_Collision.GetCollision( player ) ) // then we're screwed--probably entered collision from above
        {
            CCUtil::RunScriptFunction(m_CollisionScript);
        }
    }
}

void SetLocalUpTowards( matrix44 & m, const vector3 & v )
{
    vector3 from(m.M41,m.M42,m.M43);
    vector3 y(v - from);
    if( y.x == 0.0f && y.y == 0.0f && y.z == 0.0f )
    { // from == v
        n_printf( "Tried to look at self!\n" );
    }
    else
    {
        y.norm();
        vector3 z(CCUtil::UP_VECTOR);
        vector3 x(z * y);   // x = y cross z
        if( x.x == 0.0f && x.y == 0.0f && x.z == 0.0f )
        { // then z == (+/-)up 
            x.x = z.y;
            x.y = -z.x;
            n_assert( x % z == 0.0f );
        }
        z = x * y;      // y = z cross x
        x.norm();
        z.norm();
        m.M11=x.x;  m.M12=x.y;  m.M13=x.z;  m.M14=0.0f;
        m.M21=y.x;  m.M22=y.y;  m.M23=y.z;  m.M24=0.0f;
        m.M31=z.x;  m.M32=z.y;  m.M33=z.z;  m.M34=0.0f;
    }
}

matrix44 CCOFMWorld::CalculateTiltToGround( const vector3 & midpoint ) /*const*/
{ // determines how high the midpoint of the square must be, and how the whole must be rotated, so that the object lies on the ground
    // to do this correctly requires physics.  I will start with a cheapo version that just relies on the terrain normal (so won't work with bumpy terrain)
    vector3 OUT_normal;
    m_rMap->GetNormal(midpoint.x, midpoint.z, OUT_normal);
    matrix44 m;
    SetLocalUpTowards( m, OUT_normal );
    return m;
}