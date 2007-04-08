//------------------------------------------------------------------------------
//  ceui/texture.h
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEGUI_TEXTURE_H
#define CEGUI_TEXTURE_H

#include "CEGUITexture.h"
#include "gfx2/ntexture2.h"

namespace CEUI
{

struct CeGuiVertex
{
    float coord[3];
    float uv0[2];
    float color[4];
};

struct CeGuiTriangle
{
    CeGuiVertex vert[3];
};

struct CeGuiRectangle
{
    CeGuiTriangle tri[2];
};

class Renderer;

class Texture : public CEGUI::Texture
{
    friend class Renderer;

public:
    /// constructor
    Texture(CEGUI::Renderer* renderer);
    /// destructor
    virtual ~Texture();
    ///
    virtual void loadFromFile(const CEGUI::String& fileName, const CEGUI::String& resGroup);
    ///
    virtual void loadFromMemory(const void* bufPtr, uint bufWidth, uint bufHeight, CEGUI::Texture::PixelFormat pixelFormat);
    ///
    virtual void updateFromMemory(const void* bufPtr, uint xOffset, uint yOffset, uint subWidth, uint subHeight);
    ///
    virtual CEGUI::ushort getWidth() const;
    ///
    virtual CEGUI::ushort getHeight() const;
    ///
    nTexture2* getTexture2();
    ///
    bool isLoadFromFile() const;
    ///
    bool isLoadFromMemory() const;

protected:
    nArray<CeGuiRectangle> widgetRects;

private:
    nRef<nTexture2> texture;
    bool fromFile;
    bool fromMemory;
};

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::ushort Texture::getWidth() const {
    return texture->GetWidth();
}

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::ushort Texture::getHeight() const {
    return texture->GetHeight();
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2* Texture::getTexture2() {
    return texture.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool Texture::isLoadFromFile() const {
    return fromFile;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool Texture::isLoadFromMemory() const {
    return fromMemory;
}

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
