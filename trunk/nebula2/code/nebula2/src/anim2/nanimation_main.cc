#define N_IMPLEMENTS nAnimation
//------------------------------------------------------------------------------
//  nanimation_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "anim2/nanimation.h"

nNebulaClass(nAnimation, "nresource");

//------------------------------------------------------------------------------
/**
*/
nAnimation::nAnimation() :
    groupArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimation::~nAnimation()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    NOTE: the actual load operation should be implemented by subclasses.
*/
bool
nAnimation::Load()
{
    n_assert(!this->valid);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimation::Unload()
{
    this->groupArray.Clear();
}

//------------------------------------------------------------------------------
/**
    This method should be overwritten by subclasses.
*/
void 
nAnimation::SampleCurves(float time, int groupIndex, int firstCurveIndex, int numCurves, vector4* keyArray)
{
    // empty
}
