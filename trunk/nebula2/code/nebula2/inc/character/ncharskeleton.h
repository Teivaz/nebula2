#ifndef N_CHARSKELETON_H
#define N_CHARSKELETON_H
//------------------------------------------------------------------------------
/**
    @class nCharSkeleton

    Implements a character skeleton made of nCharJoint objects.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_CHARJOINT_H
#include "character/ncharjoint.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

//------------------------------------------------------------------------------
class nCharSkeleton
{
public:
    /// constructor
    nCharSkeleton();
    /// destructor
    ~nCharSkeleton();
    /// clear content
    void Clear();
    /// begin adding joints
    void BeginJoints(int num);
    /// add a joint to the joint skeleton
    void SetJoint(int index, int parentIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale);
    /// finish joint skeleton
    void EndJoints();
    /// get number of joint
    int GetNumJoints() const;
    /// get joint by index
    nCharJoint& GetJointAt(int index) const;

private:
    nArray<nCharJoint> jointArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::nCharSkeleton() :
    jointArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharSkeleton::Clear()
{
    this->jointArray.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton::~nCharSkeleton()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Start joint skeleton definition.

    @param  num     number of joints in skeleton
*/
inline
void
nCharSkeleton::BeginJoints(int num)
{
    n_assert(num > 0);
    this->jointArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the skeleton.
*/
inline
void
nCharSkeleton::SetJoint(int index, int parentIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale)
{
    nCharJoint newJoint;
    newJoint.SetParentJointIndex(parentIndex);
    if (-1 == parentIndex)
    {
        newJoint.SetParentJoint(0);
    }
    else
    {
        newJoint.SetParentJoint(&(this->jointArray[parentIndex]));
    }
    newJoint.SetPose(poseTranslate, poseRotate, poseScale);
    this->jointArray[index] = newJoint;
}

//------------------------------------------------------------------------------
/**
    Finish defining joints.
*/
inline
void
nCharSkeleton::EndJoints()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharSkeleton::GetNumJoints() const
{
    return this->jointArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharJoint&
nCharSkeleton::GetJointAt(int index) const
{
    return this->jointArray[index];
}

//------------------------------------------------------------------------------
#endif
