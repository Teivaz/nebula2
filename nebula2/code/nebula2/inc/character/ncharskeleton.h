#ifndef N_CHARSKELETON_H
#define N_CHARSKELETON_H
//------------------------------------------------------------------------------
/**
    @class nCharSkeleton
    @ingroup NebulaCharacterAnimationSystem

    @brief Implements a character skeleton made of nCharJoint objects.

    (C) 2002 RadonLabs GmbH
*/

#include "character/ncharjoint.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nCharSkeleton
{
public:
    /// constructor
    nCharSkeleton();
    /// destructor
    ~nCharSkeleton();
    /// copy constructor
    nCharSkeleton(const nCharSkeleton& src);
    /// assignment operator
    nCharSkeleton& operator=(const nCharSkeleton& src);
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
nCharSkeleton::nCharSkeleton(const nCharSkeleton& src) :
    jointArray(0, 0)
{
    *this = src;
}

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton& 
nCharSkeleton::operator=(const nCharSkeleton& src)
{
    if(&src == this) return (*this);

    this->jointArray = src.jointArray;

    // update parent joint references 
    int i;
    for(i=0; i<jointArray.Size(); i++)
    {
        int parentJointIndex = jointArray[i].GetParentJointIndex();
        if(parentJointIndex != -1)
        {
            nCharJoint* parentJoint = &jointArray[parentJointIndex];
            jointArray[i].SetParentJoint(parentJoint);
        }
    }
    return *this;
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
