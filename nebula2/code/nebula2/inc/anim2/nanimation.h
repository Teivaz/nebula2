#ifndef N_ANIMATION_H
#define N_ANIMATION_H
//------------------------------------------------------------------------------
/**
    @class nAnimation
    @ingroup NebulaAnimationSystem

    An nAnimation is a nResource subclass which holds one or more
    animation curves, which must all share the same number of
    keys, framerate and loop type. The nAnimation object
    can be sampled for a given curve index at a given sample time.

    One nAnimation object points to the data of exactly one
    "animation group" in an animation file (an animation group is
    a collection of curves that share the same attributes (number of keys,
    frame rate and looping type). Several nAnimation objects may
    access one animation file (since one animation file may contain
    several animation groups).

    Subclasses of nAnimation may provide file streaming or
    sampling in-memory animation data.

    After creation the object should be configured by setting the
    file and group name, after that, LoadResource() should
    be called (and also when IsValid() returns false), this is
    normal nResource class behaviour.

    To get a sample from a curve, call the SampleCurves() method.

    Animation fileformats:

    <b>ASCII: .nanim2      (not streamable)</b>

    @verbatim    
    type nanim2
    numgroups [numGroups]
    numkeys [numKeys]

    FOR EACH GROUP {
        group [numCurves] [startKey] [numKeys] [keyStride] [keyTime] [clamp|repeat]
        FOR EACH CURVE {
            curve [step|linear|quat|none] [firstKeyIndex] [collapsedKey(x,y,z,w)]
            curve
            ...
        }
        FOR EACH KEY (Keys are interleaved) {
            key [x y z w]   // first key of first non-collapsed curve
            key [x y z w]   // first key of second non-collapsed curve
            key [x y z w]   // first key of third non-collapsed curve
            ...             // etc...
        }
    }
    @endverbatim

    <b>BINARY: .nax2   (optionally streamable)</b>
    
    @verbatim            
    HEADER {
        int32 magic         // NAX2
        int32 numGroups     // number of groups in file
        int32 numKeys       // number of keys in file
    }

    FOR EACH GROUP {
        int32 numCurves         // number of curves in group
        int32 startKey          // first key index
        int32 numKeys           // number of keys in group
        int32 keyStride         // key stride in key pool
        int32 keyOffset;        // file offset to start of interleaved key data of this group
        float keyTime           // key duration
        int32 loopType          // nAnimation::LoopType
        int32 curveOffset;      // file offset to start of first curve header of this group
    }

    FOR EACH CURVE {
        int16 ipolType          // nAnimation::Curve::IpolType
        int32 firstKeyIndex     // index of first curve key in key pool
        float[4] collapsedKey   // the key value if this is a collapsed curve
    }

    FOR EACH KEY {
        float[4] value
    }
    @endverbatim

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_RESOURCE_H
#include "resource/nresource.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#undef N_DEFINES
#define N_DEFINES nResource
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nAnimation : public nResource
{
public:
    /// holds anim curve information
    class Curve
    {
    public:
        /// interpolation type
        enum IpolType
        {
            None = 0,       ///< always return the const key
            Step,           ///< no interpolation done between keys
            Quat,           ///< do quaternion interpolation
            Linear,         ///< linear interpolation done between keys
        };

        /// constructor
        Curve();
        /// destructor
        ~Curve();
        /// set the interpolation type
        void SetIpolType(IpolType t);
        /// get the interpolation type
        IpolType GetIpolType() const;
        /// set the curve's const value
        void SetConstValue(const vector4& val);
        /// get the curve's const value
        const vector4& GetConstValue() const;
        /// convert a string to an ipol type
        static IpolType StringToIpolType(const char* str);
        /// set index of the first key in the global key array
        void SetFirstKeyIndex(int index);
        /// get first key index
        int GetFirstKeyIndex() const;

    private:
        IpolType ipolType;
        vector4 constValue;
        int firstKeyIndex;
    };

    /// holds anim group information (a group of curves)
    class Group
    {
    public:
        /// loop types
        enum LoopType
        {
            Clamp = 0,
            Repeat,
        };

        /// constructor
        Group();
        /// destructor
        ~Group();
        /// set number of curves in group
        void SetNumCurves(int c);
        /// get number of curves in group
        int GetNumCurves() const;
        /// a curve object at index
        Curve& GetCurveAt(int i) const;
        /// set start key
        void SetStartKey(int k);
        /// get start key
        int GetStartKey() const;
        /// set number of keys (identical for all curves)
        void SetNumKeys(int k);
        /// get number of keys
        int GetNumKeys() const;
        /// set the key stride
        void SetKeyStride(int stride);
        /// get the key stride
        int GetKeyStride() const;
        /// set the key duration
        void SetKeyTime(float t);
        /// get the key duration
        float GetKeyTime() const;
        /// set the loop type (identical for all curves)
        void SetLoopType(LoopType t);
        /// get the loop type
        LoopType GetLoopType() const;
        /// convert string to loop type
        static LoopType StringToLoopType(const char* str);
        /// convert a time stamp into 2 global key indices and an inbetween value
        void TimeToIndex(float time, int& keyIndex0, int& keyIndex1, float& inbetween) const;

    private:
        int startKey;           ///< index of start key
        int numKeys;            ///< number of keys shared by all curves
        int keyStride;          ///< key stride in key array
        float keyTime;          ///< number of keys 
        LoopType loopType;      ///< the loop type
        nArray<Curve> curveArray;
    };

    /// constructor
    nAnimation();
    /// destructor
    virtual ~nAnimation();
    /// load the resource (sets the valid flag)
    virtual bool Load();
    /// unload the resource (clears the valid flag)
    virtual void Unload();
    /// sample values from curve range
    virtual void SampleCurves(float time, int groupIndex, int firstCurveIndex, int numCurves, vector4* keyArray);
    /// set number of groups in animation
    void SetNumGroups(int g);
    /// get number of groups in animation
    int GetNumGroups() const;
    /// get group at index
    Group& GetGroupAt(int i) const;

    static nKernelServer* kernelServer;

protected:
    /// fix the firstKeyIndex and keyStride members in the contained anim curve objects
    void FixKeyOffsets();

    nArray<Group> groupArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Curve::Curve() :
    ipolType(None),
    firstKeyIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Curve::~Curve()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Curve::SetIpolType(IpolType t)
{
    this->ipolType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Curve::IpolType
nAnimation::Curve::GetIpolType() const
{
    return this->ipolType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Curve::SetConstValue(const vector4& val)
{
    this->constValue = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nAnimation::Curve::GetConstValue() const
{
    return this->constValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Curve::SetFirstKeyIndex(int index)
{
    this->firstKeyIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimation::Curve::GetFirstKeyIndex() const
{
    return this->firstKeyIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Curve::IpolType
nAnimation::Curve::StringToIpolType(const char* str)
{
    n_assert(str);
    if (0 == strcmp("none", str))
    {
        return None;
    }
    else if (0 == strcmp("step", str))
    {
        return Step;
    }
    else if (0 == strcmp("quat", str))
    {
        return Quat;
    }
    else 
    {
        return Linear;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Group::Group() :
    startKey(0),
    numKeys(0),
    keyStride(0),
    keyTime(0),
    loopType(Repeat),
    curveArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Group::~Group()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::SetNumCurves(int c)
{
    this->curveArray.SetFixedSize(c);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimation::Group::GetNumCurves() const
{
    return this->curveArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Curve&
nAnimation::Group::GetCurveAt(int index) const
{
    return this->curveArray[index];
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::SetStartKey(int k)
{
    this->startKey = k;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimation::Group::GetStartKey() const
{
    return this->startKey;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::SetNumKeys(int k)
{
    this->numKeys = k;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimation::Group::GetNumKeys() const
{
    return this->numKeys;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::SetKeyStride(int stride)
{
    this->keyStride = stride;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimation::Group::GetKeyStride() const
{
    return this->keyStride;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::SetKeyTime(float t)
{
    this->keyTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimation::Group::GetKeyTime() const
{
    return this->keyTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::SetLoopType(LoopType t)
{
    this->loopType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Group::LoopType
nAnimation::Group::GetLoopType() const
{
    return this->loopType;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Group::LoopType
nAnimation::Group::StringToLoopType(const char* str)
{
    n_assert(str);
    if (0 == strcmp("repeat", str))
    {
        return Repeat;
    }
    else
    {
        return Clamp;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::Group::TimeToIndex(float time, int& keyIndex0, int& keyIndex1, float& inbetween) const
{
    float frame  = time / this->keyTime;
    int intFrame = int(frame);
    keyIndex0    = intFrame - startKey;
    keyIndex1    = keyIndex0 + 1;
    inbetween    = frame - float(intFrame);
    if (Clamp == this->loopType)
    {
        // 'clam' loop type
        if (keyIndex0 < 0)                   keyIndex0 = 0;
        else if (keyIndex0 >= this->numKeys) keyIndex0 = this->numKeys - 1;
        if (keyIndex1 < 0)                   keyIndex1 = 0;
        else if (keyIndex1 >= this->numKeys) keyIndex1 = this->numKeys - 1;
    }
    else
    {
        // 'repeat' loop type
        keyIndex0 %= this->numKeys;
        keyIndex1 %= this->numKeys;
    }
    keyIndex0 *= this->keyStride;
    keyIndex1 *= this->keyStride;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimation::SetNumGroups(int numGroups)
{
    this->groupArray.SetFixedSize(numGroups);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimation::GetNumGroups() const
{
    return this->groupArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation::Group&
nAnimation::GetGroupAt(int i) const
{
    return this->groupArray[i];
}

//------------------------------------------------------------------------------
#endif
