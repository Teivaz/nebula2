//------------------------------------------------------------------------------
//  nguiskin_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiskin.h"
#include "gui/nguiresource.h"

nNebulaScriptClass(nGuiSkin, "nroot");

//------------------------------------------------------------------------------
/**
*/
nGuiSkin::nGuiSkin() :
    brushes(256, 256),
    activeWindowColor(1.0f, 1.0f, 1.0f, 1.0f),
    inactiveWindowColor(0.8f, 0.8f, 0.8f, 1.0f),
    titleTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    buttonTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    labelTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSkin::~nGuiSkin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Begin adding skin brushes.
*/
void
nGuiSkin::BeginBrushes()
{
    this->brushes.Clear();
}

//------------------------------------------------------------------------------
/**
    Finish adding brushes.
*/
void
nGuiSkin::EndBrushes()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Add a new skin brush. The brush is defined by its name, the filename
    of a texture, and a rectangle within the texture in absolute texel 
    coordinates. Adding 2 brushes with identical name is a fatal error.

    @param  name    the brush name
    @param  tex     path to texture
    @param  uvPos   top left position of rectangle in uv space
    @param  uvSize  size of rectangle in uv space
    @param  color   modulation color
*/
void
nGuiSkin::AddBrush(const char* name, const char* tex, const vector2& uvPos, const vector2& uvSize, const vector4& color)
{
    n_assert(name);
    n_assert(tex);
    n_assert(!this->FindBrush(name));

    nGuiResource newRes;
    this->brushes.Append(newRes);
    nGuiResource& res = this->brushes.Back();
    res.SetName(name);
    
    // set texture name
    nString texPath = this->texPrefix;
    texPath.Append(tex);
    texPath.Append(this->texPostfix);
    res.SetTextureName(texPath.Get());
    bool loaded = res.Load();
    n_assert(loaded);

    // compute normalized uv coordinates
    rectangle uvRect;
    float texWidth  = float(res.GetTextureWidth());
    float texHeight = float(res.GetTextureHeight());

    uvRect.v0.x = uvPos.x / texWidth;
    uvRect.v0.y = 1.0f - ((uvPos.y + uvSize.y) / texHeight);
    uvRect.v1.x = (uvPos.x + uvSize.x) / texWidth;
    uvRect.v1.y = 1.0f - (uvPos.y / texHeight);
    res.SetUvRect(uvRect);
    res.SetColor(color);
}

//------------------------------------------------------------------------------
/**
    Find a brush's GUI resource by its name. Returns a 0 pointer if not
    found.
*/
nGuiResource*
nGuiSkin::FindBrush(const char* name)
{
    n_assert(name);
    int i;
    int num = this->brushes.Size();
    for (i = 0; i < num; i++)
    {
        if (0 == strcmp(name, this->brushes[i].GetName()))
        {
            return &(this->brushes[i]);
        }
    }
    return 0;
}
