#include "BombsquadBruce/bbengine.h"
#include "BombsquadBruce/bbgame.h"

#include <ctime>
#include "kernel/ntimeserver.h"
#include "gfx2/ngfxserver2.h"
#include "misc/nconserver.h"
#include "resource/nresourceserver.h"
#include "kernel/nscriptserver.h"
#include "gui/nguiserver.h"
#include "input/ninputserver.h"
#include "audio3/naudioserver3.h"
#include "gfx2/nd3d9server.h"
#include "variable/nvariableserver.h"
#include "kernel/nfileserver2.h"
#include "scene/nsceneserver.h"

#include "gui/nguiwindow.h"
#include "gui/nguilabel.h"

nNebulaScriptClass( BBEngine, "nroot" );

BBEngine::BBEngine() :
	m_IsRunning(false),
	m_Game(0)
{
    srand((uint)time(0));
}

BBEngine::~BBEngine()
{
	// empty
}

void BBEngine::StartEngine()
{
	Init();

	while ( m_IsRunning ) {
		bool keepRunning = true;
		
		kernelServer->Trigger();
		kernelServer->GetTimeServer()->Trigger();
        const nTime time = kernelServer->GetTimeServer()->GetFrameTime();
        nGuiServer::Instance()->SetTime(time);

		keepRunning &= nGfxServer2::Instance()->Trigger();
        keepRunning &= m_ScriptServer->Trigger();

        // handle input
        nInputServer::Instance()->Trigger(time);
        nGuiServer::Instance()->Trigger();
    
		keepRunning &= m_Game->Trigger(static_cast<float>(time));

        nInputServer::Instance()->FlushEvents();

		m_IsRunning &= keepRunning;
	}

	UnInit();
}

void BBEngine::StopEngine()
{
	m_IsRunning = false;
}

void BBEngine::Init()
{
    kernelServer->GetTimeServer()->EnableFrameTime();
	n_assert( !m_IsRunning );
	m_IsRunning = true;
    StartServers();
    // after you get rid of BBGui, you can probably move the following into SetGame:
    m_Game->Start();   
}

void BBEngine::UnInit()
{
    n_assert( !m_IsRunning );
	m_Game->Stop();
	m_Game.invalidate();
    StopServers();
}

void BBEngine::StopServers()
{   // order may be important!
    nGuiServer::Instance()->Release();
    nAudioServer3::Instance()->Close();
    nAudioServer3::Instance()->Release();
	nGfxServer2::Instance()->CloseDisplay();
    nGfxServer2::Instance()->Release();
    m_ScriptServer->Release();
    kernelServer->Lookup( "/sys/servers/anim" )->Release();
    nVariableServer::Instance()->Release();
    nResourceServer::Instance()->Release();
}

void BBEngine::StartServers()
{   // order may be important!
    n_verify( kernelServer->New("nresourceserver", "/sys/servers/resource") );
    n_verify( kernelServer->New("nvariableserver", "/sys/servers/variable") );
    n_verify( kernelServer->New("nanimationserver", "/sys/servers/anim") );
    StartScriptServer();
    StartGuiServer();
    StartSoundServer();
    // Open the scene server.  We want to create it in script, so we can be flexible
    // about which subclass to use, but since nSceneServer itself isn't scriptable,
    // we must open it here (which is fine, since this will need to be done
    // whichever subclass is used).
    nSceneServer* pSceneServer = (nSceneServer*)kernelServer->Lookup( "/sys/servers/scene" );
    n_assert2( pSceneServer, "No scene server in BBEngine::StartServers!" );
    pSceneServer->Open();
}

void BBEngine::StartScriptServer()
{
    m_ScriptServer = (nScriptServer*)kernelServer->New("nluaserver", "/sys/servers/script");
    nString result;
    nString startupScript( "home:data/scripts/universal/startup.lua" );
    n_verify( m_ScriptServer->RunScript(startupScript.Get(), result) ); // since "result" is a string dump of the lua stack (as opposed to a return code from RunScript), and since I know startup.lua doesn't return anything, I can ignore that value.
}

void BBEngine::StartGuiServer()
{
    // We do this here, rather than in the startup.lua with the rest of the 
    // servers, because the class is designed that way.  
    // That isn't so bad, though, because there's no good reason to initialize
    // the servers in script anyway.
    n_verify( kernelServer->New("nguiserver", "/sys/servers/gui") );
    nGuiServer::Instance()->Open();
}

void BBEngine::StartSoundServer()
{
    n_verify( kernelServer->New("ndsoundserver3", "/sys/servers/audio") );
    nAudioServer3::Instance()->Open();
}

void BBEngine::SetGame( const char * gamePath )
{
    n_assert( gamePath );
    const char* result = 0;
    if( m_Game.isvalid() ) // deactivate old game
        m_Game->Stop(); 
    m_Game = gamePath;
}

void BBEngine::LoadObject( const char* filename )
{
    n_verify( nKernelServer::Instance()->Load( nFileServer2::Instance()->ManglePath(filename).Get() ) );
}
