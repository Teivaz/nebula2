#ifndef CC_SHAPENODE_H
#define CC_SHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class CCShapeNode
    @ingroup BombsquadBruceContribModule

    Adds various Crazy Chipmunk functionality to nShapeNode.
    At the moment, only allows SetLockViewerXZ, which
    tells the node not to move relative to the camera's X, Z position.
    This is useful for e.g. creating an "infinite" ground plane.

    (C) 2004 Rafael Van Daele-Hunt
*/
#include "scene/nshapenode.h"

//------------------------------------------------------------------------------
class CCShapeNode : public nShapeNode
{
public:
    CCShapeNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    // script methods:
    /// Set to true if this node's (x,z) position is locked to the viewer.
    void SetLockViewerXZ( bool val ) { m_LockViewerXZ = val; }
    /// Tells us if this node's (x,z) position is locked to the viewer.
    bool GetLockViewerXZ() const { return m_LockViewerXZ; }
    /// The factor by which to multiply texture (u,v) to get world coordinates.  Only used in conjunction with SetLockViewerXZ
    void SetTexCoordScale( float scale ) { m_TexCoordScale = scale; } // e.g. if your mesh is a 100x100 plane, and your texture repeats 10 times along each side, then scale = 100/10 = 10
    /// Set to tell us (a) to do a fisheye transform, and (b) what position variable to base it on
    //void SetFisheyeCentreVarName( const char* varName );

    // nonscript public methods:
    /// update transform and render into scene server
    virtual bool RenderTransform(nSceneServer*, nRenderContext* , const matrix44& parentMatrix);
private:
    /// Tells us if this node is subject to the fisheye transform (objects father away from a particular game object become smaller)
    //bool GetDoFisheye() const { return nVariable::InvalidHandle != m_FisheyeCentreHandle; }

    //nVariable::Handle m_FisheyeCentreHandle;
    bool m_LockViewerXZ;
    float m_TexCoordScale; 
};

#endif

