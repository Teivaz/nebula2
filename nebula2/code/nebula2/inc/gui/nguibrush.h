#ifndef N_GUIBRUSH_H
#define N_GUIBRUSH_H
//------------------------------------------------------------------------------
/**
    A brush object which chaches the pointer to the GUI resource inside
    a skin object.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "kernel/nref.h"

class nGuiResource;
class nGuiSkin;

//------------------------------------------------------------------------------
class nGuiBrush
{
public:
    /// constructor
    nGuiBrush();
    /// constructor with name
    nGuiBrush(const char* n);
    /// destructor
    ~nGuiBrush();
    /// set brush name
    void SetName(const nString& n);
    /// get brush name
    const nString& GetName() const;
    /// get cached gui resource pointer of the brush
    nGuiResource* GetGuiResource();

private:
    nString name;
    nRef<nGuiSkin> refSkin;
    nGuiResource* guiResource;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiBrush::nGuiBrush() :
    guiResource(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiBrush::nGuiBrush(const char* n) :
    name(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiBrush::~nGuiBrush()
{
    // FIXME: decrement refcount of gui resource texture
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiBrush::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiBrush::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
#endif