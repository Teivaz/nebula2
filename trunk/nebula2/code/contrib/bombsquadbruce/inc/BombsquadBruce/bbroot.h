#ifndef N_CROOT_H
#define N_CROOT_H

//------------------------------------------------------------------------------
/**
    @class BBRoot
    @ingroup BombsquadBruceContribModule
    @brief Root class for every visible 3D object (except terrain)

    BBRoot is the common ground between BBWorld and BBRenderer.

    (C)	2003 Rafael Van Daele-Hunt
*/

#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "mathlib/matrix.h"
#include <string>
#include "scene/nrendercontext.h"
class nSceneServer;
class nShapeNode;

//------------------------------------------------------------------------------
class BBRoot : public nRoot
{
public:
	/// Constructor
    BBRoot();
	/// Destructor
    virtual ~BBRoot();

	/// Persistance
    virtual bool SaveCmds(nPersistServer* persistServer);

    //script commands
    void SetPosition( const vector3& );
    vector3 GetPosition() const;

    virtual void SetShapeNode( const char* shapenodePath );

    void SetPathVariable( nVariable::Handle, const char* NOH_Path); // stores the object itself, not the path
    
	void SetCollisionRadius( float rad );
    void SetRotation( float angleInRad ); // rotates around the local y axis -- BUGGY?  NO TIME TO CHECK AT THE MOMENT
    void SetScale( const vector3& );

    /// toggles whether or not this BBRoot will be attached to the scene graph (assuming it otherwise would be)
    void Show( bool show = true );
	void Hide();

    // nonscript commands
    virtual void Attach(nSceneServer& sceneServer, uint& frameId);
    const nShapeNode* GetShapeNode() const;
    float GetRotation() const; // in radians.  This reflects the last call to SetRotation, not necessarily the actual current rotation around the local y-axis.
    void SetTransformMatrix(const matrix44 &);
    const matrix44 & GetTransformMatrix() const { return m_TransformMatrix; }
    float GetCollisionRadius() const;

    template<typename T> void SetVariable( nVariable::Handle handle, T val )
    { // defined inside the class for VC++ 6 compatibility
        if( m_RenderContext.GetVariable( handle ) )
        {
            n_assert( "BBRoot::SetVariable -- var already exists!" );
            n_printf( "BBRoot::SetVariable -- var %d already exists!", handle );
        }
        else
        {
            const nVariable var( handle, val );
            m_RenderContext.AddVariable( var );
        }
    }

    nRenderContext m_RenderContext;
private:
    matrix44 m_TransformMatrix;			/// Position and orientation of object in the world		-- why do you still use this when you have the render context?
    mutable float m_CollisionRadius;            /// -1.0f means use the bounding box to determine collision radius
    float m_Rotation; // seems like a hack TODO
    bool m_Show;
};

inline void BBRoot::Show( bool show )
{
	m_Show = show;
}

inline void BBRoot::Hide()
{
	m_Show = false;
}

#endif

