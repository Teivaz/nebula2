#include "BombsquadBruce/bbgame.h"
#include "BombsquadBruce/bbworld.h"
#include "BombsquadBruce/bbrenderer.h"
#include "BombsquadBruce/general.h"
#include "BombsquadBruce/bbcamera.h"
#include "kernel/ntimeserver.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(BBGame, "nroot");

BBGame::BBGame() :
    m_GameTime(-1.0f),
    m_TickSize(0)
{
    // empty
}

BBGame::~BBGame()
{
    // empty
}

void BBGame::CallTriggers(float time)
{
    CallScripts( "fixedTick", time );
    m_World->Trigger( time );
    m_Camera->Trigger(time);
    if( m_DebugCamera.isvalid() )
        m_DebugCamera->Trigger(time);
}

bool BBGame::Trigger(float time)
{   
    const float deltaTime = time - m_GameTime;
    if ( 0 == m_TickSize ) {
        CallTriggers(deltaTime);
        m_GameTime = time;
    } else if( deltaTime > m_TickSize ) {
        for( ; m_GameTime + m_TickSize <= time; m_GameTime += m_TickSize )
            CallTriggers( m_TickSize );
    }
    CallScripts( "variableDelta", deltaTime );

    Render( );
    n_sleep(0); // call the scheduler to be multitask friendly
    return true;
}

void BBGame::Render()
{
    bool debugView = ( m_DebugCamera.isvalid() );
    const BBCamera* const pViewerCamera = (debugView) ? m_DebugCamera : m_Camera;
    m_Renderer->Render( pViewerCamera->GetTransform(), *m_Camera, m_World->GetAlwaysVisibleGameObjs(), debugView );
}

void BBGame::SetWorld( const char* worldPath )
{
    n_assert( worldPath );
    m_World = worldPath;
}

void BBGame::SetRenderer( const char* rendererPath )
{
    n_assert( rendererPath );
    m_Renderer = rendererPath;
}

void BBGame::SetGameTick( float ticksPerSecond )
{
    n_assert( ticksPerSecond >= 0 );
    if( 0 == ticksPerSecond )
        m_TickSize = 0;
    else
        m_TickSize = 1 / ticksPerSecond;   
}

void BBGame::Start()
{
    // Do script-based initialization:
    const char* result = 0;
    nCmdProto* run = GetClass()->FindScriptCmdByName( "Run" );
	if ( run ) {
		nCmd* cmd = run->NewCmd();
		Dispatch( cmd );
		run->RelCmd( cmd );
	}
    else
        n_printf( "No Run() command found for the game!" );

    // do C++ initialization
    m_GameTime = static_cast<float>( kernelServer->GetTimeServer()->GetFrameTime() );
    m_World->Start();
}

void BBGame::Stop()
{
    m_GameTime = -1.0f;
}

/**
    Sets the view of the world.
    Erases any active debug cameras.
*/
void BBGame::SetCamera(const char* camera_path)
{
    n_assert(camera_path);
    n_printf( "BBGame: Set camera to %s\n", camera_path );
    m_Camera = camera_path;
    m_DebugCamera.invalidate();
    nGfxServer2::Instance()->SetCamera( *m_Camera );
}

/**
    Sets the view of the world, but culling still
    occurs from the pov of the active camera (last
    SetCamera call).
*/

void BBGame::SetDebugCamera(const char* camera_path)
{
    n_assert(camera_path);
    n_printf( "BBGame: Set debug camera to %s\n", camera_path );
    m_DebugCamera = (BBCamera*)kernelServer->Lookup(camera_path);
    nGfxServer2::Instance()->SetCamera( *m_DebugCamera );
}

void BBGame::CallScripts( const char* scriptType, float deltaTime )
{
    const char* result = 0;

    nCmdProto* trigger = GetClass()->FindScriptCmdByName( "RunScripts" );
    if ( trigger ) {
        nCmd* cmd = trigger->NewCmd();
        cmd->In()->SetS( scriptType );
        cmd->In()->SetF( deltaTime );
        Dispatch( cmd );
        trigger->RelCmd( cmd );
    }
}