//------------------------------------------------------------------------------
//  ceui/render.h
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#ifndef CEUI_RENDERER_H
#define CEUI_RENDERER_H

#include "CEGUIRenderer.h"
#include "gfx2/ndynamicshadermesh.h"

namespace CEUI
{

struct CeGuiRectangle;
class Texture;
class ResourceProvider;

class Renderer : public CEGUI::Renderer
{
public:
    /// constructor
    Renderer();
    /// destructor
    virtual ~Renderer();
    ///
    virtual CEGUI::Texture* createTexture();
    ///
    virtual CEGUI::Texture* createTexture(float size);
    ///
    virtual CEGUI::Texture* createTexture(const CEGUI::String& fileName, const CEGUI::String& resGroup);
    ///
    virtual void destroyTexture(CEGUI::Texture* tex);
    ///
    virtual void destroyAllTextures();
    ///
    virtual void addQuad(const CEGUI::Rect& destRect, float z, const CEGUI::Texture* texture, const CEGUI::Rect& texRect, const CEGUI::ColourRect& colors, CEGUI::QuadSplitMode quadSplitMode);
    ///
    virtual void doRender();
    ///
    virtual void clearRenderList();
    ///
    virtual CEGUI::ResourceProvider* createResourceProvider();
    ///
    virtual float getWidth() const;
    ///
    virtual float getHeight() const;
    ///
    virtual CEGUI::Size getSize() const;
    ///
    virtual CEGUI::Rect getRect() const;
    ///
    virtual CEGUI::uint getMaxTextureSize() const;
    ///
    virtual CEGUI::uint getHorzScreenDPI() const;
    ///
    virtual CEGUI::uint getVertScreenDPI() const;
    ///
    virtual void setQueueingEnabled(bool setting);
    ///
    virtual bool isQueueingEnabled() const;

private:
    nDynamicShaderMesh mesh;
    matrix44 transform;
    nArray<Texture*> textures;
    ResourceProvider* resourceProvider;
    bool queueingEnabled;
};

//------------------------------------------------------------------------------
/**
*/
inline
float Renderer::getWidth() const {
    return (float)nGfxServer2::Instance()->GetDisplayMode().GetWidth();
}

//------------------------------------------------------------------------------
/**
*/
inline
float Renderer::getHeight() const {
    return (float)nGfxServer2::Instance()->GetDisplayMode().GetHeight();
}

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::Size Renderer::getSize() const {
    const nDisplayMode2& dm = nGfxServer2::Instance()->GetDisplayMode();
    return CEGUI::Size((float)dm.GetWidth(), (float)dm.GetHeight());
}

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::Rect Renderer::getRect() const {
    const nDisplayMode2& dm = nGfxServer2::Instance()->GetDisplayMode();
    return CEGUI::Rect(0, 0, (float)dm.GetWidth(), (float)dm.GetHeight());
}

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::uint Renderer::getMaxTextureSize() const {
    return 1024;
}

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::uint Renderer::getHorzScreenDPI() const {
    return 96;
}

//------------------------------------------------------------------------------
/**
*/
inline
CEGUI::uint Renderer::getVertScreenDPI() const {
    return 96;
}

//------------------------------------------------------------------------------
/**
*/
inline
void Renderer::setQueueingEnabled(bool setting) {
    queueingEnabled = setting;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool Renderer::isQueueingEnabled() const {
    return queueingEnabled;
}

} // namespace CEUI
//------------------------------------------------------------------------------
#endif
