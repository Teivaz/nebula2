//------------------------------------------------------------------------------
//  ccsound_main.cc
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccsound.h"
#include "audio3/nsound3.h"
#include "audio3/naudioserver3.h"

nNebulaScriptClass(CCSound, "nroot");

//------------------------------------------------------------------------------
/**
*/
CCSound::CCSound() :
    m_pResource( 0 )
{
}

//------------------------------------------------------------------------------
/**
*/
CCSound::~CCSound()
{
}

//------------------------------------------------------------------------------
/**
*/
bool CCSound::Init( const char * filename, bool looping, bool ambient )
{
    bool retVal = false;
    m_pResource = (nSound3 *)(nAudioServer3::Instance()->NewSound());
    if( m_pResource )
    {
        m_pResource->SetFilename(filename);
        m_pResource->SetLooping(looping);
        m_pResource->SetAmbient( ambient );
        //SetVolume( 1.0f );
        m_pResource->SetNumTracks(2);
        retVal = m_pResource->Load();
    }
    n_assert( retVal );
    return retVal;
}

//------------------------------------------------------------------------------
/**
*/
void CCSound::Play()
{
    n_assert( m_pResource && m_pResource->IsValid() );
    m_pResource->Start();
}

//------------------------------------------------------------------------------
/**
*/
void CCSound::Stop()
{
    n_assert( m_pResource && m_pResource->IsValid() );
    m_pResource->Stop();
}
//------------------------------------------------------------------------------
/**
*/
void
CCSound::SetPosition( const vector3& position )
{
    n_assert( m_pResource && m_pResource->IsValid() );
    matrix44 m;
    m.set_translation( position );
    m_pResource->SetTransform(m);
    m_pResource->Update();
}

//------------------------------------------------------------------------------
/**
*/
void
CCSound::SetVolume( float volume )
{    
    m_pResource->SetVolume( volume );
    m_pResource->Update();
}

//------------------------------------------------------------------------------
/**
*/
//void
//CCSound::SetVelocity( const vector3& velocity )
//{
//    n_assert( m_pResource && m_pResource->IsValid() );
//    m_pResource->SetVelocity(v);
//    m_pResource->Update();
//}

