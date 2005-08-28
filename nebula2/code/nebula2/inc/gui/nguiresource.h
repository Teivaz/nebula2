#ifndef N_GUIRESOURCE_H
#define N_GUIRESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nGuiResource
    @ingroup Gui
    @brief A gui resource for Nebula2's gui subsystem.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "gfx2/ngfxserver2.h"
#include "mathlib/rectangle.h"

class nGuiServer;

//------------------------------------------------------------------------------
class nGuiResource
{
public:
    /// constructor
    nGuiResource();
    /// destructor
    ~nGuiResource();
    /// set resource identifier name
    void SetName(const char* n);
    /// get resource identifier name
    const char* GetName() const;
    /// set texture path
    void SetTextureName(const char* n);
    /// get texture path
    const char* GetTextureName() const;
    /// set uv rect in texture in texel coords
    void SetAbsUvRect(const rectangle& r);
    /// get uv rect in texture in texel coords
    const rectangle& GetAbsUvRect() const;
    /// get relative uv rect 
    const rectangle& GetRelUvRect();
    /// set modulate color
    void SetColor(const vector4& v);
    /// get modulate color
    const vector4& GetColor() const;
    /// check to see if resource is valid
    bool IsValid() const;
    /// load resource (call if not valid)
    bool Load();
    /// unload resource
    void Unload();
    /// get pointer to texture resource
    nTexture2* GetTexture();
    /// get texture width
    int GetTextureWidth();
    /// get texture height
    int GetTextureHeight();
    /// Make texture dynamic/static.
    void SetDynamic(bool v);
    /// Is dynamic (aka. Does `GetTexture' return a render target?).
    bool IsDynamic() const;
    /// set the touched flag
    void SetTouched(bool b);
    /// get the touched flag
    bool IsTouched() const;

private:
    nString resName;
    nString texName;
    rectangle absUvRect;
    rectangle relUvRect;
    vector4 color;
    nRef<nTexture2> refTexture;
    bool dynamic;
    bool touched;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiResource::nGuiResource() :
    color(1.0f, 1.0f, 1.0f, 1.0f),
    dynamic(false),
    touched(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiResource::~nGuiResource()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiResource::SetName(const char* n)
{
    n_assert(n);
    this->resName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiResource::GetName() const
{
    return this->resName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiResource::SetTextureName(const char* name)
{
    if (this->IsValid())
    {
        this->Unload();
    }
    this->texName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiResource::GetTextureName() const
{
    return this->texName.IsEmpty() ? 0 : this->texName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiResource::SetAbsUvRect(const rectangle& r)
{
    this->absUvRect = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
const rectangle&
nGuiResource::GetAbsUvRect() const
{
    return this->absUvRect;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiResource::SetColor(const vector4& v)
{
    this->color = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiResource::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiResource::IsValid() const
{
    return (this->refTexture.isvalid() && this->refTexture->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2*
nGuiResource::GetTexture()
{
    if (!this->IsValid())
    {
        this->Load();
    }
    return this->refTexture.isvalid() ? this->refTexture.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiResource::GetTextureWidth()
{
    if (!this->IsValid())
    {
        this->Load();
    }
    return this->refTexture->GetWidth();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiResource::GetTextureHeight()
{
    if (!this->IsValid())
    {
        this->Load();
    }
    return this->refTexture->GetHeight();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiResource::SetDynamic(bool v)
{
    this->dynamic = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiResource::IsDynamic() const
{
    return this->dynamic;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiResource::SetTouched(bool b)
{
    this->touched = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiResource::IsTouched() const
{
    return this->touched;
}

//------------------------------------------------------------------------------
#endif

