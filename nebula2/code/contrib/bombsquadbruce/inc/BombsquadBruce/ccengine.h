#ifndef N_ENGINE_H
#define N_ENGINE_H
//------------------------------------------------------------------------------
/**
    @class CCEngine
    @ingroup BombsquadBruceContribModule
    @brief The global game application.

    The engine starts and triggers the essential servers, as well as an 
    instance of CCGame -- which instance is script-settable and defines 
    the current game mode (see CCGame).  The engine is the first class
    to be instantiated, and when it is stopped, the app will close.

    (C) 2004 Rafael Van Daele-Hunt
*/

#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"

class nConServer;
class nScriptServer;
//class CCProfilerDisplay;
class CCGame;

class CCEngine : public nRoot
{
public:
    CCEngine();
    virtual ~CCEngine();

    // script commands
    void StopEngine();
    void SetGame( const char * gamePath );
    /// Load a persistent object from an .n2 file
    void LoadObject( const char* path );

    // nonscript commands
    void StartEngine();

protected:
    virtual void Init();
    virtual void UnInit();
private:
    void StartServers();
    void StopServers();
    void StartScriptServer();
    void StartGuiServer();
    void StartSoundServer();
    void StartGame( const char * gamePath );

    bool m_IsRunning;

    nDynAutoRef<CCGame>         m_Game;

    nAutoRef<nConServer>        m_ConsoleServer;
    nRef<nScriptServer>         m_ScriptServer;

    //nAutoRef<CCProfilerDisplay> m_Profiler;
};

#endif
