#ifndef N_GAME_H
#define N_GAME_H
//------------------------------------------------------------------------------
/**
    @class CCGame

    @brief Handles a single game mode (e.g. the pregame menus, or game proper).

    All it actually does is call registered scripts and triggers the renderer 
    and world; the scripts and the world define the game mode.

    (C) 2004 Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include <vector>
#include <string>

//------------------------------------------------------------------------------
class CCWorld;
class CCRenderer;
class CCCamera;

class CCGame : public nRoot
{
public:
    CCGame();
    virtual ~CCGame();
    virtual bool SaveCmds( nPersistServer* persistServer );

    // script commands
    /// Set world path
    void SetWorld( const char* worldPath );
    /// Set renderer path
    void SetRenderer( const char* worldPath );
    /// Sets the time between game state updates.  0 == as often as possible.
    void SetGameTick( float ticksPerSecond );
    /// Set the game's viewpoint
    void SetCamera(const char* camera_path);    
    /// Switch to a debug visualization of the active camera
    void SetDebugCamera(const char* camera_path);    

    // nonscript commands
    void Start(); // called to start or restart the game
    void Stop();

    virtual bool Trigger(float time); // make sure to call Start before this
private:
    void CallTriggers(float time);
    void CallScripts( const char* scriptType, float deltaTime );
    void Render( );

    float m_GameTime;
    float m_TickSize;

    nDynAutoRef<CCWorld> m_World;
    nDynAutoRef<CCRenderer> m_Renderer;
    nDynAutoRef<CCCamera> m_Camera;
    nRef<CCCamera> m_DebugCamera;
};

//------------------------------------------------------------------------------

#endif
