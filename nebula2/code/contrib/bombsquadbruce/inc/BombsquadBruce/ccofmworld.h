#ifndef N_COFMWORLD_H
#define N_COFMWORLD_H
//------------------------------------------------------------------------------
/**
    @class CCOFMWorld

    @brief The Bombsquad Bruce game world

    (C)	2004 Rafael Van Daele-Hunt
*/

#include "BombsquadBruce/ccworld.h"
#include "kernel/ndynautoref.h"
#include <string>
class CCWater;
class nMap;
class CCMobileActor;

class CCOFMWorld : public CCWorld
{
public:
    CCOFMWorld();
    virtual bool SaveCmds(nPersistServer* ps);

    virtual void Trigger(float deltaTime);

    // script commands:
    virtual void SetMap(const char* path);
    virtual float GetHeight(float x, float z) /*const*/ ;
    virtual void StartGameObjs();
    virtual bool AddGameObject( const char* path );
    virtual void EndGameObjs();
    void SetPlayer(const char * playerPath);
    void SetWater( const char* path );
    void SetMinCollisionHeight( float minHeight ) { n_assert( minHeight >= 0.0f ); m_MinCollisionHeight = minHeight; }
    void SetOnCollisionScript(const char* path) { m_CollisionScript = path; }

    // nonscript commands:
    virtual void Start(); // must be called before world is used

private:
    virtual matrix44 CalculateTiltToGround( const vector3& midpoint ) /*const*/;

    void DoPlayer(float dt);
    nMap& GetMap() { return *(m_rMap); }
    void HandleCollision( CCMobileActor& player, const vector3& curPos );

    nDynAutoRef<CCMobileActor> m_rPlayer;
    nDynAutoRef<CCWater> m_rWater;
    nDynAutoRef<nMap> m_rMap;

    // scriptable constants
    float m_MinCollisionHeight; // objects smaller than this aren't included in the collision list

    const char* m_CollisionScript;
};

//---------------------------------------------------------------------------

#endif
