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

//------------------------------------------------------------------------------
namespace Graphics
{
class AnimTable : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(AnimTable);

public:
    /// constructor
    AnimTable();
    /// destructor
    virtual ~AnimTable();
    /// return instance pointer
    static AnimTable* Instance();
    /// set filename
    void SetFilename(const nString& n);
    /// get filename
    const nString& GetFilename() const;
    /// open the table
    bool Open();
    /// close the table
    void Close();
    /// return true if open
    bool IsOpen() const;
    /// map abstract anim name to anim clip name
    const nString& Lookup(const nString& column, const nString& animName) const;

private:
    static AnimTable* Singleton;
    nXmlSpreadSheet xmlSpreadsheet;
};

RegisterFactory(AnimTable);

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
    return this->xmlSpreadsheet.IsOpen();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
AnimTable::SetFilename(const nString& n)
{
    this->xmlSpreadsheet.SetFilename(n);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
AnimTable::GetFilename() const
{
    return this->xmlSpreadsheet.GetFilename();
}

};
//------------------------------------------------------------------------------
#endif    
