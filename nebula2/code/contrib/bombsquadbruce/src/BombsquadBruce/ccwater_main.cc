//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccwater.h"
#include "variable/nvariableserver.h"
#include "scene/nsceneserver.h"

nNebulaScriptClass(CCWater, "ccroot");

//------------------------------------------------------------------------------
CCWater::CCWater() :
    m_WaveHeight( 0.0f ),
    m_WaveFrequency( 0.0f ),
    m_BaseLevel( 0.0f ),
    m_CurLevel( 0.0f )
{
}

CCWater::~CCWater() 
{
}
/////////////////////////////////////
// Script Methods
/////////////////////////////////////

void CCWater::SetBaseWaterLevel( float level )
{ 
    m_BaseLevel = level; 
    SetHeight( level );
}

void CCWater::SetWaveProperties( float height, float frequency )
{
    m_WaveHeight = height;
    m_WaveFrequency = frequency;
    n_assert( 0.0f <= m_WaveFrequency );
}

void CCWater::SetLayerNode( const char* path )
{
    m_rLayerNode = path;
}

void CCWater::AddLayer( float depth )
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

void CCWater::Trigger( float dt )
{
    float newHeight = m_BaseLevel + n_sin( dt * m_WaveFrequency ) * m_WaveHeight;
    SetHeight( newHeight );
}

void CCWater::Attach( nSceneServer& sceneServer, uint& frameId )
{
    for( int i = 0; i < m_LayerContexts.Size(); ++i )
    {
        m_LayerContexts[i].SetFrameId( frameId++ );
        sceneServer.Attach( &m_LayerContexts[i] );
    }
    CCRoot::Attach( sceneServer, frameId );
}

float CCWater::GetWaterLevel() const
{
    return m_CurLevel;
}
/////////////////////////////////////
// Private Methods
/////////////////////////////////////


void CCWater::SetHeight(float newHeight) 
{
    vector3 pos = GetPosition();
    pos.y = newHeight;
    SetPosition( pos );
    m_CurLevel = newHeight;
}