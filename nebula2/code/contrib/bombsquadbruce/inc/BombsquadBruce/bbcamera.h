#ifndef BB_CAMERA_H
#define BB_CAMERA_H
//------------------------------------------------------------------------------
/**
    @class BBCamera
    @ingroup BombsquadBruceContribModule
    @brief "Abstract" parent of all camera classes.

    BBCamera translates user input (as provided through
    script-specified channels) into camera movement instructions.
    How this instructions are interpreted is left to BBCamera's
    subclasses, although the internal names (e.g. m_VerticalSpin)
    hint at their intended purposes.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "mathlib/matrix.h"
#include "scene/nrendercontext.h"
#include "gfx2/ncamera2.h"
#include "spatialdb/nvisitorbase.h"
#include <string>
class nSceneServer;
class BBCullingMgr;
class sphere;

// TODO Refactor this into BBInputCamera (an nRoot that only handles input)
// BBCullingCamera (a child of stripped down BBRoot (see below) and nCamera2
// BBCamera will be a child of those two.
// a mixin should handle the Manipulate stuff, perhaps; otherwise, it goes in BBCullingCamera 
//------------------------------------------------------------------------------
class nInputServer;
class nGfxServer2;

class BBCamera : public nRoot, public nCamera2
{
public:
    BBCamera();
    virtual ~BBCamera();
    virtual bool SaveCmds(nPersistServer* ps);

    // script commands:
    /// Update the camera's position and orientation.
    virtual void Trigger(double deltaTime);
    /// Maps names to internal control channels.
    void SetControl( const std::string & control, const std::string & channel ); 
    /// Get camera position (in worldspace).
    vector3 GetPosition() const;
    /// Get camera orientation as a quaternion.
    quaternion GetOrientation() const;
    /// optional debug visualization node
    void SetDebugShape( const char* shapenodePath );
    /// Switch to spherical culling mode
    void SetSphereCuller( const sphere& );
    /// Switch to frustum culling mode
    void SetFrustumCuller();
    /// culls the game objects, calling Manipulate() on each unculled object
    void ManipulateVisibleElements();

    // nonscript commands:
    const matrix44& GetTransform() const { return m_TransformMatrix; }
    void Attach( nSceneServer& sceneServer, uint& frameId ); // TODO: get rid of this -- make it descend from BBRoot
    /// Returns an initialized, camera-dependent subclass of nVisibilityVistor, for use by BBCullingMgr, which owns the returned pointer.
    nVisibilityVisitor* GetVisibilityVisitor( nVisibilityVisitor::VisibleElements& m_Elements ) const; 
protected:
    /// Update the camera's position and orientation.
    float GetZMove() const { return m_ZMove; }
    float GetVerticalSpin() const  { return m_VerticalSpin; }
    float GetHorizontalSpin() const  { return m_HorizontalSpin; }
    float GetFwdBackMove() const  { return m_FwdBackMove; }
    float GetLeftRightMove() const  { return m_LeftRightMove; }

    matrix44 m_TransformMatrix;

private:
    /// Calls the script command "Manipulate"
    void Manipulate(nRoot&);

    // store the user input values for a given tick:
    float m_ZMove;
    float m_VerticalSpin;
    float m_HorizontalSpin;
    float m_FwdBackMove;
    float m_LeftRightMove;

    std::string m_SpinLeftChannel;
    std::string m_SpinRightChannel;
    std::string m_SpinUpChannel;
    std::string m_SpinDownChannel;
    std::string m_MoveLeftChannel;
    std::string m_MoveRightChannel;
    std::string m_MoveFwdChannel;
    std::string m_MoveBackChannel;
    std::string m_ZoomInChannel;
    std::string m_ZoomOutChannel;

    nRenderContext m_RenderContext;
    /// Store the type and parameters of this camera's culling
    enum CullT
    {
        SPHERE,
        FRUSTUM
    } m_CullMode;

    union
    {
        struct 
        {
            float m_CullSphereRadius;
            vector3 m_CullSphereOffset; //from the camera's current position, relative to the camera's orientation (-z will be transformed to the camera's forward vector)
        }; 
    };
    nAutoRef<BBCullingMgr> m_rCullingMgr;

    // script-side commands
    nCmdProto * m_ManipulateCmdProto;
};

#endif
