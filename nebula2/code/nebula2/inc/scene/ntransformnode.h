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
    GetQuat() does NOT return the orientation set by
    SetEuler().

    See also @ref N2ScriptInterface_ntransformnode
    
    (C) 2002 RadonLabs GmbH
*/
#include "scene/nscenenode.h"

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
    virtual bool RenderTransform(nSceneServer* sceneServer, nRenderContext* renderContext, const matrix44& parentMatrix);

    /// lock to viewer position
    void SetLockViewer(bool b);
    /// locked to viewer position?
    bool GetLockViewer() const;
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

protected:
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
        Dirty = (1<<0),         // the matrix is dirty
        UseQuat = (1<<1),       // compute orientation from quaternion, otherwise from euler
        Active = (1<<2),        // active/inactive
        LockViewer = (1<<3),    // locked to viewer position
    };

protected:
    nAutoRef<nGfxServer2> refGfxServer;
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
nTransformNode::SetLockViewer(bool b)
{
    if (b) this->SetFlags(LockViewer);
    else   this->UnsetFlags(LockViewer);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTransformNode::GetLockViewer() const
{
    return this->CheckFlags(LockViewer);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetActive(bool b)
{
    if (b) this->SetFlags(Active);
    else   this->UnsetFlags(Active);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nTransformNode::GetActive() const
{
    return this->CheckFlags(Active);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nTransformNode::SetPosition(const vector3& p)
{
    this->pos = p;
    this->SetFlags(Dirty);
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
    this->SetFlags(Dirty);
    this->UnsetFlags(UseQuat);
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
    this->SetFlags(Dirty | UseQuat);
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
    this->SetFlags(Dirty);
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
    if (this->CheckFlags(Dirty))
    {
        if (this->CheckFlags(UseQuat))
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
        this->UnsetFlags(Dirty);
    }
}

//------------------------------------------------------------------------------
#endif
