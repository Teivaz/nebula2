#ifndef N_CWORLD_H
#define N_CWORLD_H
//------------------------------------------------------------------------------
/**
    @class CCWorld
    @ingroup BombsquadBruceContribModule
    @brief Abstract base class for a game world.

    A collection of game objects, some stored in an nSpatialDB,
    some always visible.  Whether CCWorld should do anything
    more is currently under debate.
    Old description:
    Manages generic game objects, player, and map, provides a simple interface
    for CCGame, and handles the interaction between game entities (e.g. player
    & other stuff)

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "util/narray.h"
#include "BombsquadBruce/ccref.h"
#include "BombsquadBruce/ccroot.h"
#include "BombsquadBruce/cccollision.h"
class CCCullingMgr;

class CCWorld : public nRoot
{
public:
    CCWorld();
    virtual ~CCWorld();
    virtual bool SaveCmds(nPersistServer* ps);

    virtual void Trigger(float deltaTime) { n_error( "Pure virtual method CCWorld::Trigger called!"); } 

    // script commands
    /// Sets the map, from which to get height & boundary information
    virtual void SetMap(const char* path) { n_error( "Pure virtual method CCWorld::SetMap called!"); }
    /// Get the height at a specific point using x, y
    virtual float GetHeight(float x, float z) /*const*/ { n_error( "Pure virtual method CCWorld::GetHeight called!"); return 0; }     
    virtual void StartGameObjs();
    virtual bool AddGameObject( const char* path );
    virtual void EndGameObjs();
    bool IsGameActive() const { return m_GameActive; }
    void Pause(bool pause) { m_GameActive = !pause; }

    // nonscript commands
    virtual void Start(); // must be called before world is used
    /// game objects that don't go into the culling manager, for passing to CCRenderer
    const nArray< CCRef<CCRoot> >& GetAlwaysVisibleGameObjs() { return m_AlwaysVisibleGameObjs; }
protected:
    virtual matrix44 CalculateTiltToGround( const vector3& midpoint ) /*const*/ { n_error( "Pure virtual method CCWorld::CalculateTiltToGround called!"); return matrix44(); }
    void SetOnGround( CCRoot& gameObj, float distFromGround = 0, bool tiltToGround = true );
    bool DoAddGameObject( CCRoot& rRoot );
   
    /// when this is false, the game is not active (e.g. player has died or paused), and the world should take appropriate action (or, more likely, inaction)
    bool m_GameActive;		
    /// noncullable game objects that can't be included in the static scene because they are more than just a contextless shapenode.
    nArray< CCRef<CCRoot> > m_AlwaysVisibleGameObjs;       
    CCCollision m_Collision;
private:
    const CCRoot* GetStaticCollision( const CCRoot& gameObj ) const;

    nAutoRef<CCCullingMgr> m_rCullingMgr;

    // other game state
    enum blockState { NEW, STARTED, ENDED };
    blockState m_InStartGameObj;  /// whether we are inside a StartGameObjs/EndGameObjs block
};

//---------------------------------------------------------------------------

#endif
