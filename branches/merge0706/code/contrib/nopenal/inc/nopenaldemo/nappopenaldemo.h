//-----------------------------------------------------------------------------
//  nappopenaldemo.h
//
//  (C)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_APPOPENALDEMO_H
#define N_APPOPENALDEMO_H
//-----------------------------------------------------------------------------
/**
    @class nAppOpenALDemo
    @ingroup NOpenALDemoContribModule
    
    @brief nopenal demo application
    
*/
#include "application/napplication.h"

class nAppOpenALState;

//-----------------------------------------------------------------------------
class nAppOpenALDemo : public nApplication
{
public:
    nAppOpenALDemo();
    virtual ~nAppOpenALDemo();

    // @name nApplication override functions
    // @{
    virtual bool Open();
    virtual void Close();    
    // @}

protected:
    /// create script server object
    virtual nScriptServer* CreateScriptServer();
    /// create audio server object
    virtual nAudioServer3* CreateAudioServer();

protected:
    /// app state
    nRef<nAppOpenALState>    nappopenalstate;

};
//------------------------------------------------------------------------------
#endif // N_APPOPENALDEMO_H 
