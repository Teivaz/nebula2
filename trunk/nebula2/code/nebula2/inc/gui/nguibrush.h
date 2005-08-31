#ifndef N_GUIBRUSH_H
#define N_GUIBRUSH_H
//------------------------------------------------------------------------------
/**
    @class nGuiBrush
    @ingroup Gui

    @brief A brush object which caches the pointer to the GUI resource inside
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
    /// load resource
    bool Load();
    /// unload resource
    void Unload();
    /// return true if loaded
    bool IsLoaded() const;

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
    name(n),
    guiResource(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiBrush::~nGuiBrush()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiBrush::SetName(const nString& n)
{
    this->name = n;
    if (this->IsLoaded())
    {
        this->Unload();
    }
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
/**
*/
inline
bool
nGuiBrush::IsLoaded() const
{
    return (0 != this->guiResource);
}

//------------------------------------------------------------------------------
#endif
