#ifndef N_CWATER_H
#define N_CWATER_H
//------------------------------------------------------------------------------
/**
    @class CCWater
    @ingroup BombsquadBruceContribModule
    @brief The water for Bombsquad Bruce

    (C)	2004 Rafael Van Daele-Hunt
*/

#include "kernel/ndynautoref.h"
#include "BombsquadBruce/ccroot.h"
class nVariableServer;

class CCWater : public CCRoot
{
public:
    CCWater();
    virtual ~CCWater();
    virtual bool SaveCmds(nPersistServer* persistServer);

    // script commands:
    void SetBaseWaterLevel( float level );
    void SetWaveProperties( float height, float frequency );
    void SetLayerNode( const char* shapeNodePath );
    void AddLayer( float depth );

    // nonscript commands:
    virtual void Trigger( float time ); // not dt, but time
    virtual void Attach(nSceneServer& sceneServer, uint& frameId);
    float GetWaterLevel() const;

private:
    void SetHeight(float);
    nDynAutoRef<nSceneNode> m_rLayerNode; // the definition used for the underwater depth planes
    nArray<nRenderContext> m_LayerContexts; // water planes layered under the main water surface to add depth-based opacity and additional movement
    float m_WaveHeight;
    float m_WaveFrequency;
    float m_BaseLevel;
    float m_CurLevel;
};


//---------------------------------------------------------------------------

#endif
