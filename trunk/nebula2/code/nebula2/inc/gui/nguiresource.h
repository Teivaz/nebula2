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

private:
    nString resName;
    nString texName;
    rectangle absUvRect;
    rectangle relUvRect;
    bool relUvRectDirty;
    vector4 color;
    nRef<nTexture2> refTexture;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiResource::nGuiResource() :
    color(1.0f, 1.0f, 1.0f, 1.0f),
    relUvRectDirty(true)
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
    this->relUvRectDirty = true;
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
    this->relUvRectDirty = true;
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
const rectangle&
nGuiResource::GetRelUvRect()
{
    if (this->relUvRectDirty)
    {
        if (!this->IsValid())
        {
            this->Load();
        }
        this->relUvRectDirty = false;

        vector2 corrUvPos(this->absUvRect.v0.x + 0.5f, this->absUvRect.v0.y + 0.5f);
        vector2 absUvSize = this->absUvRect.size();
        vector2 corrUvSize(absUvSize.x - 1.0f, absUvSize.y - 1.0f);

        float texHeight = float(this->GetTextureHeight());
        float texWidth  = float(this->GetTextureWidth());

        this->relUvRect.v0.x = (corrUvPos.x + 0.5f) / texWidth;
        this->relUvRect.v0.y = 1.0f - ((corrUvPos.y + corrUvSize.y) / texHeight);
        this->relUvRect.v1.x = (corrUvPos.x + 0.5f + corrUvSize.x) / texWidth;
        this->relUvRect.v1.y = 1.0f - ((corrUvPos.y + 0.5f) / texHeight);
    }
    return this->relUvRect;
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
#endif

