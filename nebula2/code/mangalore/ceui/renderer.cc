//------------------------------------------------------------------------------
//  ceui/render.cc
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
#include "ceui/renderer.h"
#include "ceui/texture.h"
#include "ceui/resourceprovider.h"
#include "gfx2/nshader2.h"

namespace CEUI
{

//------------------------------------------------------------------------------
/**
*/
Renderer::Renderer() {
    nShader2* shader = nGfxServer2::Instance()->NewShader(0);
    shader->SetFilename("shaders:cegui.fx");
    n_verify(shader->Load());
    mesh.SetShader(shader);
    mesh.Initialize(nGfxServer2::TriangleList, nMesh2::Coord | nMesh2::Uv0 | nMesh2::Color, nMesh2::WriteOnly, false);
    transform.scale(vector3(2.0f/getWidth(), -2.0f/getHeight(), 1.0f));
    transform.set_translation(vector3(-1.0f/getWidth() - 1.0f, -1.0f/getHeight() + 1.0f, 0.0f));
    resourceProvider = 0;
    queueingEnabled = false;
    d_identifierString = "CEGUI::MangaloreRenderer - Nebula 2 renderer module for CEGUI";
}

//------------------------------------------------------------------------------
/**
*/
Renderer::~Renderer() {
    destroyAllTextures();
    n_delete(resourceProvider);
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::Texture* Renderer::createTexture() {
    Texture* texture = n_new(Texture(this));
    textures.Append(texture);
    return texture;
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::Texture* Renderer::createTexture(float size) {
    Texture* texture = n_new(Texture(this));
    textures.PushBack(texture);
    return texture;
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::Texture* Renderer::createTexture(const CEGUI::String& fileName, const CEGUI::String& resGroup) {
    Texture* texture = n_new(Texture(this));
    texture->loadFromFile(fileName, resGroup);
    textures.Append(texture);
    return texture;
}

//------------------------------------------------------------------------------
/**
*/
void Renderer::destroyTexture(CEGUI::Texture* tex) {
    Texture* texture = static_cast<Texture*>(tex);
    int i = textures.FindIndex(texture);
    if (i != -1) {
        textures.Erase(i);
        n_delete(texture);
    }    
}

//------------------------------------------------------------------------------
/**
*/
void Renderer::destroyAllTextures() {
    for (int i = 0; i = textures.Size(); i++) {
        delete textures[i];
    }
    textures.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void Renderer::addQuad(const CEGUI::Rect& destRect, float z, const CEGUI::Texture* texture, const CEGUI::Rect& texRect, const CEGUI::ColourRect& colors, CEGUI::QuadSplitMode quadSplitMode) {
    static CEGUI::Rect cursorRect;

    if (isQueueingEnabled()) {
        CeGuiVertex vertices[4];
        // top left
        vertices[0].coord[0] = destRect.d_left;
        vertices[0].coord[1] = destRect.d_top;
        vertices[0].coord[2] = z;
        vertices[0].uv0[0] = texRect.d_left;
        vertices[0].uv0[1] = texRect.d_top;
        vertices[0].color[0] = colors.d_top_left.getRed();
        vertices[0].color[1] = colors.d_top_left.getGreen();
        vertices[0].color[2] = colors.d_top_left.getBlue();
        vertices[0].color[3] = colors.d_top_left.getAlpha();
        // bottom left
        vertices[1].coord[0] = destRect.d_left;
        vertices[1].coord[1] = destRect.d_bottom;
        vertices[1].coord[2] = z;
        vertices[1].uv0[0] = texRect.d_left;
        vertices[1].uv0[1] = texRect.d_bottom;
        vertices[1].color[0] = colors.d_bottom_left.getRed();
        vertices[1].color[1] = colors.d_bottom_left.getGreen();
        vertices[1].color[2] = colors.d_bottom_left.getBlue();
        vertices[1].color[3] = colors.d_bottom_left.getAlpha();
        // bottom right
        vertices[2].coord[0] = destRect.d_right;
        vertices[2].coord[1] = destRect.d_bottom;
        vertices[2].coord[2] = z;
        vertices[2].uv0[0] = texRect.d_right;
        vertices[2].uv0[1] = texRect.d_bottom;
        vertices[2].color[0] = colors.d_bottom_right.getRed();
        vertices[2].color[1] = colors.d_bottom_right.getGreen();
        vertices[2].color[2] = colors.d_bottom_right.getBlue();
        vertices[2].color[3] = colors.d_bottom_right.getAlpha();
        // top right
        vertices[3].coord[0] = destRect.d_right;
        vertices[3].coord[1] = destRect.d_top;
        vertices[3].coord[2] = z;
        vertices[3].uv0[0] = texRect.d_right;
        vertices[3].uv0[1] = texRect.d_top;
        vertices[3].color[0] = colors.d_top_right.getRed();
        vertices[3].color[1] = colors.d_top_right.getGreen();
        vertices[3].color[2] = colors.d_top_right.getBlue();
        vertices[3].color[3] = colors.d_top_right.getAlpha();

        CeGuiRectangle rect;
        if (quadSplitMode == CEGUI::TopLeftToBottomRight) {
            rect.tri[0].vert[0] = vertices[0];
            rect.tri[0].vert[1] = vertices[1];
            rect.tri[0].vert[2] = vertices[2];
            rect.tri[1].vert[0] = vertices[0];
            rect.tri[1].vert[1] = vertices[2];
            rect.tri[1].vert[2] = vertices[3];
        } else {
            rect.tri[0].vert[0] = vertices[3];
            rect.tri[0].vert[1] = vertices[0];
            rect.tri[0].vert[2] = vertices[1];
            rect.tri[1].vert[0] = vertices[0];
            rect.tri[1].vert[1] = vertices[1];
            rect.tri[1].vert[2] = vertices[3];
        }
        ((Texture*)texture)->widgetRects.PushBack(rect);
    } else if (texRect != cursorRect) {
        cursorRect = texRect;
        uint width = uint(texture->getWidth()*texRect.getWidth());
        uint height = uint(texture->getHeight()*texRect.getHeight());
        nTexture2* srcTexture = ((Texture*)texture)->getTexture2();
        nMouseCursor cursor;
        cursor.CreateEmpty(32, 32);
        nTexture2* dstTexture = cursor.GetTexture();

        struct nTexture2::LockInfo srcLockInfo;
        struct nTexture2::LockInfo dstLockInfo;
        if (srcTexture->Lock(nTexture2::ReadOnly, 0, srcLockInfo), dstTexture->Lock(nTexture2::WriteOnly, 0, dstLockInfo)) {
            uint startX = uint(texture->getWidth()*texRect.d_left);
            uint startY = uint(texture->getHeight()*texRect.d_top);
            for (uint y = 0; y < height; y++) {
                memcpy((CEGUI::uint8*)dstLockInfo.surfPointer + dstLockInfo.surfPitch*y,
                    (CEGUI::uint8*)srcLockInfo.surfPointer + srcLockInfo.surfPitch*(startY + y) + sizeof(CEGUI::uint32)*startX,
                    sizeof(CEGUI::uint32)*width);
            }
            srcTexture->Unlock(0);
            dstTexture->Unlock(0);
        }
        nGfxServer2::Instance()->SetMouseCursor(cursor);
    }
}

//------------------------------------------------------------------------------
/**
*/
void Renderer::doRender() {
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->PushTransform(nGfxServer2::Model, matrix44::identity);
    gfxServer->PushTransform(nGfxServer2::View, transform);
    gfxServer->PushTransform(nGfxServer2::Projection, matrix44::identity);

    float* vertices;
    int maxVetices;
    mesh.Begin(vertices, maxVetices);
    for(nArray<Texture*>::iterator i = textures.Begin(); i != textures.End(); i++) {
        nArray<CeGuiRectangle>& widgetRects = (*i)->widgetRects;
        if (!widgetRects.Empty()) {
            mesh.GetShader()->SetTexture(nShaderState::DiffMap0, (*i)->getTexture2());
            int curRect = 0;
            while (curRect < widgetRects.Size()) {
                int numRects = min(maxVetices/6, widgetRects.Size() - curRect);
                memcpy(vertices, &widgetRects[curRect], sizeof(CeGuiRectangle)*numRects);
                mesh.Swap(6*numRects, vertices);
                curRect += numRects;
            }
        }
    }
    mesh.End(0);

    gfxServer->PopTransform(nGfxServer2::Model);
    gfxServer->PopTransform(nGfxServer2::View);
    gfxServer->PopTransform(nGfxServer2::Projection);
}

//------------------------------------------------------------------------------
/**
*/
void Renderer::clearRenderList() {
    for(nArray<Texture*>::iterator i = textures.Begin(); i != textures.End(); i++) {
        (*i)->widgetRects.Reset();
    }
}

//------------------------------------------------------------------------------
/**
*/
CEGUI::ResourceProvider* Renderer::createResourceProvider() {
    resourceProvider = n_new(ResourceProvider);
    return resourceProvider;
}

} // namespace CEUI
