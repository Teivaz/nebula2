#ifndef N_MOUSECURSOR_H
#define N_MOUSECURSOR_H
//------------------------------------------------------------------------------
/**
    Holds mouse cursor attributes.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "gfx2/ntexture2.h"

//------------------------------------------------------------------------------
class nMouseCursor
{
public:
    /// constructor
    nMouseCursor();
    /// copy constructor
    nMouseCursor(const nMouseCursor& rhs);
    /// destructor
    ~nMouseCursor();
    /// copy operator
    void operator=(const nMouseCursor& rhs);
    /// set cursor image texture (incrs. refcount)
    void SetTexture(nTexture2* tex);
    /// get cursor image texture
    nTexture2* GetTexture() const;
    /// set hotspot x
    void SetHotspotX(int x);
    /// get hotspot x
    int GetHotspotX() const;
    /// set hotspot y
    void SetHotspotY(int y);
    /// get hotspot y
    int GetHotspotY() const;

private:
    /// delete contents
    void Delete();
    /// copy contents
    void Copy(const nMouseCursor& rhs);

    nRef<nTexture2> refTexture;
    int hotSpotX;
    int hotSpotY;
};

//------------------------------------------------------------------------------
/**
*/
inline
nMouseCursor::nMouseCursor() :
    hotSpotX(0),
    hotSpotY(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::Delete()
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::Copy(const nMouseCursor& rhs)
{
    n_assert(!this->refTexture.isvalid());
    this->refTexture = rhs.refTexture.get();
    this->hotSpotX = rhs.hotSpotX;
    this->hotSpotY = rhs.hotSpotY;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMouseCursor::~nMouseCursor()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::operator=(const nMouseCursor& rhs)
{
    this->Delete();
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::SetTexture(nTexture2* tex)
{
    this->Delete();
    if (tex)
    {
        tex->AddRef();
        this->refTexture = tex;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2*
nMouseCursor::GetTexture() const
{
    return this->refTexture.isvalid() ? this->refTexture.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::SetHotspotX(int x)
{
    this->hotSpotX = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMouseCursor::GetHotspotX() const
{
    return this->hotSpotX;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMouseCursor::SetHotspotY(int y)
{
    this->hotSpotY = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMouseCursor::GetHotspotY() const
{
    return this->hotSpotY;
}

//------------------------------------------------------------------------------
#endif