#ifndef N_MEMORYANIMATION_H
#define N_MEMORYANIMATION_H
//------------------------------------------------------------------------------
/**
    An nMemoryAnimation object holds its entire animation data in memory.
    Since all the animation is in memory all the time, the animation
    can be shared between many client objects. The disadvantage is of
    course the memory footprint.

    See the parent class nAnimation for more info.

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_ANIMATION_H
#include "anim2/nanimation.h"
#endif

#undef N_DEFINES
#define N_DEFINES nMemoryAnimation
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nMemoryAnimation : public nAnimation
{
public:
    /// constructor
    nMemoryAnimation();
    /// destructor
    virtual ~nMemoryAnimation();
    /// load the resource (sets the valid flag)
    virtual bool Load();
    /// unload the resource (clears the valid flag)
    virtual void Unload();
    /// sample value of given curve at given time
    virtual void SampleCurves(float time, int groupIndex, int firstCurveIndex, int numCurves, vector4* keyArray);

    static nKernelServer* kernelServer;

private:
    /// load curve group from ascii nanim2 file
    bool LoadNanim2(const char* filename);
    /// load curve group from binary nax2 file
    bool LoadNax2(const char* filename);

    nArray<vector4> keyArray;   // all keys in the anim file
};
//------------------------------------------------------------------------------
#endif
