#ifndef N_TRANSFORMNODE_H
#define N_TRANSFORMNODE_H
//------------------------------------------------------------------------------
/**
    @class nTransformNode
    @ingroup NebulaSceneSystemNodes

    A transform node groups its child nodes and defines position, orientation 
    and scale of a scene node. Transformation hierarchies can be created 
    using the Nebula object name space hierarchy.

    Note that conversions through the GetXXX() method generally don't work.
    It is not possible to do a SetMatrix() and get the resulting
    orientation as quaternion with GetQuat().
    Similarly GetQuat() does NOT return the orientation set by
    SetEuler().

    See also @ref N2ScriptInterface_ntransformnode
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_SCENENODE_H
#include "scene/nscenenode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nTransformNode
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nTransformNode : public nSceneNode
{
public:
    /// constructor
    nTransformNode();
    /// destructor
    virtual ~nTransformNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// called by nSceneServer when object is attached to scene
    virtual void Attach(nSceneServer* sceneServer, nRenderContext* renderContext);
    /// indicate the scene server that this node provides transformation
    virtual bool HasTransform() const;
    /// update transform and render into scene server
    virtual void RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix);

    /// set view space flag
    void SetViewSpace(bool b);
    /// get view space flag
    bool GetViewSpace() const;
    /// set active flag
    void SetActive(bool b);
    /// get active flag
    bool GetActive() const;
    /// set position in parent space
    void SetPosition(const vector3& p);
    /// get position in parent space (not valid if SetMatrix() was used)
    const vector3& GetPosition() const;
    /// set orientation as euler angles (rotation order is x -> y -> z, unit is RAD)
    void SetEuler(const vector3& e);
    /// get orientation as euler angles in parent space 
    const vector3& GetEuler() const;
    /// set orientation as quaternion in parent space
    void SetQuat(const quaternion& q);
    /// get orientation as quaternion in parent space
    const quaternion& GetQuat() const;
    /// set scale in parent space
    void SetScale(const vector3& s);
    /// get scale in parent space
    const vector3& GetScale() const;

    static nKernelServer* kernelServer;

private:
    /// set a flags
    void SetFlags(ushort mask);
    /// unset a flags
    void UnsetFlags(ushort mask);
    /// check a flag mask
    bool CheckFlags(ushort mask) const;
    /// update matrix if dirty
    void UpdateMatrix();

    enum
    {
        DIRTY = (1<<0),         // the matrix is dirty
        USEQUAT = (1<<1),       // compute orientation from quaternion, otherwise from euler
        VIEWSPACE = (1<<2),     // this node lives in view space
        ACTIVE = (1<<3),        // active/inactive
    };

protected:
    vector3 pos;
    vector3 euler;
    vector3 scale;
    quaternion quat;
    matrix44 matrix;
    ushort transformFlags;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetFlags(ushort mask)
{
    this->transformFlags |= mask;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::UnsetFlags(ushort mask)
{
    this->transformFlags &= ~mask;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTransformNode::CheckFlags(ushort mask) const
{
    return ((this->transformFlags & mask) == mask);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetViewSpace(bool b)
{
    if (b) this->SetFlags(VIEWSPACE);
    else   this->UnsetFlags(VIEWSPACE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTransformNode::GetViewSpace() const
{
    return this->CheckFlags(VIEWSPACE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetActive(bool b)
{
    if (b) this->SetFlags(ACTIVE);
    else   this->UnsetFlags(ACTIVE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTransformNode::GetActive() const
{
    return this->CheckFlags(ACTIVE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetPosition(const vector3& p)
{
    this->pos = p;
    this->SetFlags(DIRTY);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nTransformNode::GetPosition() const
{
    return this->pos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetEuler(const vector3& e)
{
    this->euler = e;
    this->SetFlags(DIRTY);
    this->UnsetFlags(USEQUAT);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nTransformNode::GetEuler() const
{
    return this->euler;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetQuat(const quaternion& q)
{
    this->quat = q;
    this->SetFlags(DIRTY | USEQUAT);
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
nTransformNode::GetQuat() const
{
    return this->quat;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetScale(const vector3& s)
{
    this->scale = s;
    this->SetFlags(DIRTY);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nTransformNode::GetScale() const
{
    return this->scale;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::UpdateMatrix()
{
    if (this->CheckFlags(DIRTY))
    {
        if (this->CheckFlags(USEQUAT))
        {
            // use quaternion
            this->matrix.ident();
            this->matrix.scale(this->scale);
            this->matrix.mult_simple(matrix44(this->quat));
            this->matrix.translate(this->pos);
        }
        else
        {
            // use euler
            this->matrix.ident();
            this->matrix.scale(this->scale);
            this->matrix.rotate_x(this->euler.x);
            this->matrix.rotate_y(this->euler.y);
            this->matrix.rotate_z(this->euler.z);
            this->matrix.translate(this->pos);
        }

        this->UnsetFlags(DIRTY);
    }
}

//------------------------------------------------------------------------------
#endif
