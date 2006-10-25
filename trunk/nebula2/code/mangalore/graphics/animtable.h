#ifndef GRAPHICS_ANIMTABLE_H
#define GRAPHICS_ANIMTABLE_H
//------------------------------------------------------------------------------
/**
    @class Graphics::AnimTable

    Creates a mapping from abstract animation names to actual anim clip names.

    (C) 2005 Radon Labs GmbH
*/
#include "foundation/refcounted.h"
#include "xml/nxmlspreadsheet.h"
#include "util/narray2.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class AnimTable : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(AnimTable);

public:
    /// an animation info helper class
    class AnimInfo
    {
    public:
        /// constructor
        AnimInfo();
        /// get mapped animation name
        const nString& GetAnimName() const;
        /// get optional hotspot time
        nTime GetHotSpotTime() const;

    private:
        friend class AnimTable;
        nString animName;
        nTime hotSpotTime;
    };

    /// constructor
    AnimTable();
    /// destructor
    virtual ~AnimTable();
    /// return instance pointer
    static AnimTable* Instance();
    /// set file name
    void SetFilename(const nString& n);
    /// get file name
    const nString& GetFilename() const;
    /// open the table
    bool Open();
    /// close the table
    void Close();
    /// return true if open
    bool IsOpen() const;
    /// lookup animation info by name
    const AnimInfo& Lookup(const nString& column, const nString& animName, bool random = true) const;

private:
    static AnimTable* Singleton;
    nString filename;
    nArray<nString> columnIndexMap;
    nArray<nString> rowIndexMap;
    nArray2< nArray<AnimInfo> > animInfos;
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline
AnimTable::AnimInfo::AnimInfo() :
    hotSpotTime(0.0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AnimTable::AnimInfo::GetAnimName() const
{
    return this->animName;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
AnimTable::AnimInfo::GetHotSpotTime() const
{
    return this->hotSpotTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
AnimTable*
AnimTable::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
AnimTable::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AnimTable::SetFilename(const nString& n)
{
    this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AnimTable::GetFilename() const
{
    return this->filename;
}

};
//------------------------------------------------------------------------------
#endif
