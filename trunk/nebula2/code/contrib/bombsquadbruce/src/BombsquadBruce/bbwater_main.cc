//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbwater.h"
#include "variable/nvariableserver.h"
#include "scene/nsceneserver.h"

nNebulaScriptClass(BBWater, "bbroot");

//------------------------------------------------------------------------------
BBWater::BBWater() :
    m_WaveHeight( 0.0f ),
    m_WaveFrequency( 0.0f ),
    m_BaseLevel( 0.0f ),
    m_CurLevel( 0.0f )
{
}

BBWater::~BBWater() 
{
}
/////////////////////////////////////
// Script Methods
/////////////////////////////////////

void BBWater::SetBaseWaterLevel( float level )
{ 
    m_BaseLevel = level; 
    SetHeight( level );
}

void BBWater::SetWaveProperties( float height, float frequency )
{
    m_WaveHeight = height;
    m_WaveFrequency = frequency;
    n_assert( 0.0f <= m_WaveFrequency );
}

void BBWater::SetLayerNode( const char* path )
{
    m_rLayerNode = path;
}

void BBWater::AddLayer( float depth )
{ // SetWaterLevelVarName must already have been called
    nRenderContext newLayer;
    newLayer.SetRootNode( m_rLayerNode );
    matrix44 m = m_RenderContext.GetTransform();
    m.translate( vector3( 0.0f, m_BaseLevel - depth, 0.0f ) );
    newLayer.SetTransform(m);
    m_LayerContexts.PushBack( newLayer );
}
/////////////////////////////////////
// Public Methods
/////////////////////////////////////

void BBWater::Trigger( float dt )
{
    float newHeight = m_BaseLevel + n_sin( dt * m_WaveFrequency ) * m_WaveHeight;
    SetHeight( newHeight );
}

void BBWater::Attach( nSceneServer& sceneServer, uint& frameId )
{
    for( int i = 0; i < m_LayerContexts.Size(); ++i )
    {
        m_LayerContexts[i].SetFrameId( frameId++ );
        sceneServer.Attach( &m_LayerContexts[i] );
    }
    BBRoot::Attach( sceneServer, frameId );
}

float BBWater::GetWaterLevel() const
{
    return m_CurLevel;
}
/////////////////////////////////////
// Private Methods
/////////////////////////////////////


void BBWater::SetHeight(float newHeight) 
{
    vector3 pos = GetPosition();
    pos.y = newHeight;
    SetPosition( pos );
    m_CurLevel = newHeight;
}