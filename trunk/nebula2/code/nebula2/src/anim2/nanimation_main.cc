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
*/
void
nAnimation::UnloadResource()
{
    this->groupArray.Clear();
}

//------------------------------------------------------------------------------
/**
    This method should be overwritten by subclasses.
*/
void 
nAnimation::SampleCurves(float /*time*/, int /*groupIndex*/, int /*firstCurveIndex*/, int /*numCurves*/, vector4* /*keyArray*/)
{
    // empty
}
