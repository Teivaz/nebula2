#ifndef C_SCENESERVER_H
#define C_SCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class BBStdSceneServer
    @ingroup BombsquadBruceContribModule
    @ingroup NebulaSceneSystemServers

    @brief Default Crazy Chipmunk scene server.
    
    Differs from nMRTSceneServer in that it:
     -# Renders alpha blended objects 
        (those that set the variable at GetAlphaVarHandle() in their
        nRenderContext) back to front, after all other objects.
     -# Handles LOD.

    (C) 2004 Rafael Van Daele-Hunt
*/
#include "scene/nmrtsceneserver.h"
#include "variable/nvariable.h"

//------------------------------------------------------------------------------
class BBSceneServer : public nMRTSceneServer
{
public:
    /// constructor
    BBSceneServer();
    // script methods:
    nVariable::Handle GetAlphaVarHandle() const { return m_AlphaFlagHandle; }
    nVariable::Handle GetLODDistVarHandle() const { return m_LODDistHandle; }
    nVariable::Handle GetLODVarHandle(int index) const;
    // nonscript methods:
protected:
    /// transfer CrazyChipmunk-Standard parameters to shader (wind, etc...)
    virtual void UpdateShader(nShader2* shd, nRenderContext* renderContext);
    /// split scene nodes into light and shape nodes
    virtual void SplitNodes(uint shaderFourCC);
    /// the actual sorting call is virtual, since Compare can't be
    virtual void DoSort( ushort* indexPtr, int numIndices );
    /// static qsort() compare function
    static int __cdecl BBCompare(const ushort* i1, const ushort* i2);

private:
    /// Determines which LOD node should be rendered, or whether the node should be culled after all
    bool SetLOD( Group& );
    void SubInLowPolyModel( Group&, int LODShapeIndex );
    void SubInBillboard( Group&, int LODShapeIndex );

    //static const int m_NumLODShapes = 2;
    enum { m_NumLODShapes = 2 }; // VC++ 6 compatibility hack

    nVariable::Handle m_LODDistHandle;
    nVariable::Handle m_LODShapeHandles[m_NumLODShapes];

    // Used for sorting:
    static nVariable::Handle m_AlphaFlagHandle;
    static BBSceneServer* bbself;

};
//------------------------------------------------------------------------------
#endif


