#include "ceui/texture.h"
#include "ceui/renderer.h"
#include "gfx2/ngfxserver2.h"

namespace CEUI
{

//------------------------------------------------------------------------------
/**
*/
Texture::Texture(CEGUI::Renderer* renderer) : CEGUI::Texture(renderer) {
    widgetRects.SetFlags(nArray<CeGuiRectangle>::DoubleGrowSize);
    fromFile = false;
    fromMemory = false;
}

//------------------------------------------------------------------------------
/**
*/
Texture::~Texture() {
}

//------------------------------------------------------------------------------
/**
*/
void Texture::loadFromFile(const CEGUI::String& fileName, const CEGUI::String& resGroup) {
    texture = nGfxServer2::Instance()->NewTexture(fileName.c_str());
    texture->SetFilename(fileName.c_str());
    texture->Load();
    fromFile = true;
}

//------------------------------------------------------------------------------
/**
*/
void Texture::loadFromMemory(const void* bufPtr, uint bufWidth, uint bufHeight) {
    texture = nGfxServer2::Instance()->NewTexture(0);
    texture->SetUsage(nTexture2::CreateEmpty);
    texture->SetType(nTexture2::TEXTURE_2D);
    texture->SetFormat(nTexture2::A8R8G8B8);
    texture->SetWidth(bufWidth);
    texture->SetHeight(bufHeight);
    texture->Load();
    struct nTexture2::LockInfo lockInfo;
    if (texture->Lock(nTexture2::WriteOnly, 0, lockInfo)) {
        for (uint y = 0; y < bufHeight; y++) {
            memcpy((CEGUI::uint8*)lockInfo.surfPointer + y*lockInfo.surfPitch, (CEGUI::uint32*)bufPtr + y*bufWidth, sizeof(CEGUI::uint32)*bufWidth);
        }
        texture->Unlock(0);
    }
    fromMemory = true;
}

}