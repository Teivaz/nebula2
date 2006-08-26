#ifndef N_ANIMBUILDER_H
#define N_ANIMBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nAnimBuilder
    @ingroup NebulaGraphicsSystemTools

    The nAnimBuilder class is similar to the nMeshBuilder class, but
    assists in creating, loading, saving and processing of animation data.

    Supported file formats are nanim2 and nax2 (ascii and binary
    animation file formats).

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "mathlib/vector.h"
#include "util/narray.h"
#include "util/nstring.h"
#include "kernel/nfile.h"

class nFileServer2;

//------------------------------------------------------------------------------
class nAnimBuilder
{
public:
    /// an animation key
    class Key
    {
    public:
        /// constructor
        Key();
        /// constructor with value
        Key(const vector4& val);
        /// set value
        void Set(const vector4& val);
        /// get value
        const vector4& Get() const;
        /// set x component
        void SetX(float x);
        /// set y component
        void SetY(float y);
        /// set z component
        void SetZ(float z);
        /// set w component
        void SetW(float w);

        vector4 value;
    };

    /// an animation curve
    class Curve
    {
    public:
        /// interpolation types
        enum IpolType
        {
            NONE,           // not interpolated (invalid)
            STEP,           // step through keys
            LINEAR,         // linear interpolation
            QUAT,           // quaternion interpolation
        };

        /// default constructor
        Curve();
        /// fixed size constructor
        Curve(int numKeys, const Key& fillKey);
        /// add a key to the end of the array
        void SetKey(int index, const Key& key);
        /// get number of keys
        int GetNumKeys() const;
        /// get key at index
        Key& GetKeyAt(int index);
        /// set the interpolation type of the curve
        void SetIpolType(IpolType t);
        /// get the interpolation type of the curve
        IpolType GetIpolType() const;
        /// set the first key index
        void SetFirstKeyIndex(int i);
        /// get the first key index
        int GetFirstKeyIndex() const;
        /// optimize the curve
        bool Optimize();
        /// set collapsed flag
        void SetCollapsed(bool b);
        /// check whether the curve has been collapsed by Optimize()
        bool IsCollapsed() const;
        /// set the collapsed key
        void SetCollapsedKey(const vector4& key);
        /// get the collapsed key
        const vector4& GetCollapsedKey() const;
        /// convert ipol type to string
        static const char* IpolType2String(IpolType t);
        /// convert string to ipol type
        static IpolType String2IpolType(const char* str);

        nArray<Key> keyArray;
        IpolType ipolType;
        bool isCollapsed;
        int firstKeyIndex;
        vector4 collapsedKey;
    };

    /// an animation curve group
    class Group
    {
    public:
        /// loop types
        enum LoopType
        {
            CLAMP,
            REPEAT,
        };

        /// constructor
        Group();
        /// add an animation curve object
        void AddCurve(const Curve& curve);
        /// get number of animation curves
        int GetNumCurves() const;
        /// get animation curve at index
        Curve& GetCurveAt(int index) const;
        /// set loop type
        void SetLoopType(LoopType t);
        /// get loop type
        LoopType GetLoopType() const;
        /// set the start key
        void SetStartKey(int start);
        /// get the start key
        int GetStartKey() const;
        /// set number of keys
        void SetNumKeys(int num);
        /// get number of keys
        int GetNumKeys() const;
        /// set the key duration
        void SetKeyTime(float t);
        /// get the key duration
        float GetKeyTime() const;
        /// set the key stride
        void SetKeyStride(int stride);
        /// get the key stride
        int GetKeyStride() const;
        /// check if the curves are valid
        bool Validate() const;
        /// convert loop type to string
        static const char* LoopType2String(LoopType t);
        /// convert string to loop type
        static LoopType String2LoopType(const char* str);

        nArray<Curve> curveArray;
        LoopType loopType;
        nString groupName;
        int startKey;
        int numKeys;
        int keyStride;
        float keyTime;
    };

    /// constructor
    nAnimBuilder();
    /// destructor
    ~nAnimBuilder();

    /// updates the first key index and key stride members (call before saving the file)
    void FixKeyOffsets();
    /// save anim file (filename extension decides format)
    bool Save(nFileServer2* fileServer, const char* filename);
    /// save nanim2 file
    bool SaveNanim2(nFileServer2* fileServer, const char* filename);
    /// save binary data to existing file
    bool SaveNax2(nFile* file);
    /// save nax2 file
    bool SaveNax2(nFileServer2* fileServer, const char* filename);
    /// load anim file (filename extension decides format)
    bool Load(nFileServer2* fileServer, const char* filename);
    /// load nanim2 file
    bool LoadNanim2(nFileServer2* fileServer, const char* filename);
    /// load nax2 file
    bool LoadNax2(nFileServer2* fileServer, const char* filename);

    /// clear all
    void Clear();
    /// add an animation group
    void AddGroup(Group& group);
    /// get number of groups
    int GetNumGroups() const;
    /// get group at index
    Group& GetGroupAt(int i);

    /// optimize the animation data
    int Optimize();

private:
    /// return total number of keys
    int GetNumKeys();

    nArray<Group> groupArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Key::Key()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Key::Key(const vector4& val) :
    value(val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Key::SetX(float x)
{
    this->value.x = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Key::SetY(float y)
{
    this->value.y = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Key::SetZ(float z)
{
    this->value.z = z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Key::SetW(float w)
{
    this->value.w = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Key::Set(const vector4& v)
{
    this->value = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nAnimBuilder::Key::Get() const
{
    return this->value;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Curve::Curve() :
    keyArray(128, 2048),
    ipolType(NONE),
    isCollapsed(false),
    firstKeyIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Curve::Curve(int numKeys, const Key& initKey) :
    ipolType(NONE),
    isCollapsed(true),
    firstKeyIndex(-1)
{
    this->keyArray.SetFixedSize(numKeys);
    this->keyArray.Fill(0, this->keyArray.Size(), initKey);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Curve::SetKey(int index, const Key& key)
{
    this->keyArray.At(index) = key;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::Curve::GetNumKeys() const
{
    return this->keyArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Key&
nAnimBuilder::Curve::GetKeyAt(int index)
{
    return this->keyArray.At(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Curve::SetIpolType(IpolType t)
{
    this->ipolType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Curve::IpolType
nAnimBuilder::Curve::GetIpolType() const
{
    return this->ipolType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Curve::SetFirstKeyIndex(int i)
{
    this->firstKeyIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::Curve::GetFirstKeyIndex() const
{
    return this->firstKeyIndex;
}

//------------------------------------------------------------------------------
/**
    This checks whether all keys in the curve are identical.
    If yes, the Collapsed flag will be set, and the collapsed
    key is set to the first key in the curve.

    - 18-Oct-2004   floh    optimized curves have ipolType no longer set to NONE!
*/
inline
bool
nAnimBuilder::Curve::Optimize()
{
    n_assert(this->keyArray.Size() > 0);

    this->isCollapsed = false;
    int numKeys = this->keyArray.Size();
    vector4 minVec = this->keyArray[0].Get();
    vector4 maxVec = this->keyArray[0].Get();
    int curKeyIndex;
    for (curKeyIndex = 1; curKeyIndex < numKeys; curKeyIndex++)
    {
        const vector4& curVec = this->keyArray[curKeyIndex].Get();
        minVec.minimum(curVec);
        maxVec.maximum(curVec);
    }
    vector4 dist = maxVec - minVec;
    if (dist.len() < 0.001f)
    {
        this->isCollapsed = true;
        this->collapsedKey = this->keyArray[0].Get();
    }
    return this->isCollapsed;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Curve::SetCollapsed(bool b)
{
    this->isCollapsed = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimBuilder::Curve::IsCollapsed() const
{
    return this->isCollapsed;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Curve::SetCollapsedKey(const vector4& key)
{
    this->collapsedKey = key;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nAnimBuilder::Curve::GetCollapsedKey() const
{
    return this->collapsedKey;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimBuilder::Curve::IpolType2String(IpolType t)
{
    switch (t)
    {
        case NONE:      return "none";
        case STEP:      return "step";
        case LINEAR:    return "linear";
        case QUAT:      return "quat";
    }
    n_assert(false);
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Curve::IpolType
nAnimBuilder::Curve::String2IpolType(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "quat"))           return QUAT;
    else if (0 == strcmp(str, "step"))      return STEP;
    else if (0 == strcmp(str, "linear"))    return LINEAR;
    else                                    return NONE;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Group::Group() :
    loopType(REPEAT),
    startKey(0),
    numKeys(0),
    keyStride(0),
    keyTime(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Group::AddCurve(const Curve& curve)
{
    this->curveArray.Append(curve);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::Group::GetNumCurves() const
{
    return this->curveArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Curve&
nAnimBuilder::Group::GetCurveAt(int index) const
{
    return this->curveArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Group::SetLoopType(LoopType t)
{
    this->loopType = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Group::LoopType
nAnimBuilder::Group::GetLoopType() const
{
    return this->loopType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Group::SetStartKey(int start)
{
    this->startKey = start;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::Group::GetStartKey() const
{
    return this->startKey;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Group::SetNumKeys(int num)
{
    this->numKeys = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::Group::GetNumKeys() const
{
    return this->numKeys;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Group::SetKeyStride(int stride)
{
    this->keyStride = stride;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::Group::GetKeyStride() const
{
    return this->keyStride;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::Group::SetKeyTime(float t)
{
    this->keyTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimBuilder::Group::GetKeyTime() const
{
    return this->keyTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimBuilder::Group::Validate() const
{
    // make sure all curves have the same number of keys as set in the anim group
    int numCurves = this->curveArray.Size();
    int curveIndex;
    for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
    {
        if (this->GetCurveAt(curveIndex).GetNumKeys() != this->GetNumKeys())
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nAnimBuilder::Group::LoopType2String(LoopType t)
{
    switch (t)
    {
        case CLAMP:     return "clamp";
        case REPEAT:    return "repeat";
    }
    n_assert(false);
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Group::LoopType
nAnimBuilder::Group::String2LoopType(const char* str)
{
    n_assert(str);
    if (0 == strcmp("clamp", str)) return CLAMP;
    else                           return REPEAT;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimBuilder::AddGroup(Group& group)
{
    this->groupArray.Append(group);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimBuilder::GetNumGroups() const
{
    return this->groupArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimBuilder::Group&
nAnimBuilder::GetGroupAt(int index)
{
    return this->groupArray[index];
}

//------------------------------------------------------------------------------
#endif

