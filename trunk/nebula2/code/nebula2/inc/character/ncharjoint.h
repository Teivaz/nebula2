#ifndef N_CHARJOINT_H
#define N_CHARJOINT_H
//------------------------------------------------------------------------------
/**
    @class nCharJoint

    A joint in a character skeleton.

    06-Feb-03   floh    fixed for Nebula2

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

//------------------------------------------------------------------------------
class nCharJoint
{
public:
    /// constructor
    nCharJoint();
    /// destructor
    ~nCharJoint();
    /// set parent joint index
    void SetParentJointIndex(int index);
    /// get parent joint index
    int GetParentJointIndex() const;
    /// set parent joint pointer
    void SetParentJoint(nCharJoint* p);
    /// get the parent joint
    nCharJoint* GetParentJoint() const;
    /// set the pose
    void SetPose(const vector3& t, const quaternion& q, const vector3& s);
    /// get pose translate
    const vector3& GetPoseTranslate() const;
    /// get pose rotate
    const quaternion& GetPoseRotate() const;
    /// get pose scale
    const vector3& GetPoseScale() const;
    /// set translation
    void SetTranslate(const vector3& t);
    /// get translation
    const vector3& GetTranslate() const;
    /// set rotation
    void SetRotate(const quaternion& q);
    /// get rotation
    const quaternion& GetRotate() const;
    /// set scale
    void SetScale(const vector3& s);
    /// get scale
    const vector3& GetScale() const;
    /// evaluate joint
    void Evaluate();
    /// get the skinning matrix with translation
    const matrix44& GetSkinMatrix44() const;
    /// get the skinning matrix without translation (for normals)
    const matrix33& GetSkinMatrix33() const;

private:
    vector3 poseTranslate;
    quaternion poseRotate;
    vector3 poseScale;

    vector3 translate;
    quaternion rotate;
    vector3 scale;

    matrix44 poseMatrix;
    matrix44 invPoseMatrix;

    matrix44 matrix;
    matrix44 skinMatrix44;
    matrix33 skinMatrix33;
    int parentJointIndex;
    nCharJoint* parentJoint;
    bool matrixDirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint::nCharJoint() :
    parentJoint(0),
    parentJointIndex(-1),
    poseScale(1.0f, 1.0f, 1.0f),
    scale(1.0f, 1.0f, 1.0f),
    matrixDirty(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint::~nCharJoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetParentJoint(nCharJoint* p)
{
    this->parentJoint = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint*
nCharJoint::GetParentJoint() const
{
    return this->parentJoint;
}

//------------------------------------------------------------------------------
/**
    The parent joint index can be -1 if this joint is the root joint.
*/
inline
void
nCharJoint::SetParentJointIndex(int index)
{
    this->parentJointIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharJoint::GetParentJointIndex() const
{
    return this->parentJointIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetPose(const vector3& t, const quaternion& q, const vector3& s)
{
    this->poseTranslate = t;
    this->poseRotate = q;
    this->poseScale = s;

    this->poseMatrix.ident();
    this->poseMatrix.scale(this->poseScale);
    this->poseMatrix.mult_simple(matrix44(this->poseRotate));
    this->poseMatrix.translate(this->poseTranslate);

    // set the initial matrix so that it undoes the pose matrix
    this->matrix = poseMatrix;

    // globale pose matrix and compute global inverse pose matrix
    if (this->parentJoint)
    {
        this->poseMatrix.mult_simple(this->parentJoint->poseMatrix);
    }
    this->invPoseMatrix = this->poseMatrix;
    this->invPoseMatrix.invert_simple();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetPoseTranslate() const
{
    return this->poseTranslate;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion& 
nCharJoint::GetPoseRotate() const
{
    return this->poseRotate;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetPoseScale() const
{
    return this->poseScale;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetTranslate(const vector3& t)
{
    this->translate = t;
    this->matrixDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetTranslate() const
{
    return this->translate;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetRotate(const quaternion& q)
{
    this->rotate = q;
    this->matrixDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
nCharJoint::GetRotate() const
{
    return this->rotate;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharJoint::SetScale(const vector3& s)
{
    this->scale = s;
    this->matrixDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nCharJoint::GetScale() const
{
    return this->scale;
}

//------------------------------------------------------------------------------
/**
    This computes the skinning matrix from the pose matrix, the translation,
    the rotation and the scale of the joint. The parent joint must already be
    uptodate!
*/
inline
void
nCharJoint::Evaluate()
{
    if (this->matrixDirty)
    {
        this->matrix.ident();
        this->matrix.scale(this->scale);
        this->matrix.mult_simple(matrix44(this->rotate));
        this->matrix.translate(this->translate);
        this->matrixDirty = false;
    }

    if (this->parentJoint)
    {
        this->matrix.mult_simple(this->parentJoint->matrix);
    }
    this->skinMatrix44 = this->invPoseMatrix * this->matrix;
    this->skinMatrix33.set(this->skinMatrix44.M11, this->skinMatrix44.M12, this->skinMatrix44.M13,
                           this->skinMatrix44.M21, this->skinMatrix44.M22, this->skinMatrix44.M23,
                           this->skinMatrix44.M31, this->skinMatrix44.M32, this->skinMatrix44.M33);
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nCharJoint::GetSkinMatrix44() const
{
    return this->skinMatrix44;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix33&
nCharJoint::GetSkinMatrix33() const
{
    return this->skinMatrix33;
}

//------------------------------------------------------------------------------
#endif
