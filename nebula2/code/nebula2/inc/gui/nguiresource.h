#ifndef N_GUIRESOURCE_H
#define N_GUIRESOURCE_H
//------------------------------------------------------------------------------
/**
    @class nGuiResource
    @ingroup NebulaGuiSystem
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
    /// set uv rect in texture
    void SetUvRect(const rectangle& r);
    /// get uv rect in texture
    const rectangle& GetUvRect() const;
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

private:
    nAutoRef<nGfxServer2> refGfxServer;
    nAutoRef<nGuiServer> refGuiServer;
    nString resName;
    nString texName;
    rectangle uvRect;
    vector4 color;
    nRef<nTexture2> refTexture;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiResource::nGuiResource() :
    refGfxServer("/sys/servers/gfx"),
    refGuiServer("/sys/servers/gui"),
    uvRect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f)),
    color(1.0f, 1.0f, 1.0f, 1.0f)
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
nGuiResource::SetUvRect(const rectangle& r)
{
    this->uvRect = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
const rectangle&
nGuiResource::GetUvRect() const
{
    return this->uvRect;
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
#endif

