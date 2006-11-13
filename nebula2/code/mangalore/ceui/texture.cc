//------------------------------------------------------------------------------
//  ceui/texture.cc
//  (c) 2006 Nebula2 Community
//------------------------------------------------------------------------------
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
    if (!texture->IsLoaded()) {
        texture->Load();
    }
    fromFile = true;
}

//------------------------------------------------------------------------------
/**
*/
void Texture::loadFromMemory(const void* bufPtr, uint bufWidth, uint bufHeight, CEGUI::Texture::PixelFormat pixelFormat) {
    texture = nGfxServer2::Instance()->NewTexture(0);
    texture->SetUsage(nTexture2::CreateEmpty | nTexture2::Dynamic);
    texture->SetType(nTexture2::TEXTURE_2D);
    switch (pixelFormat) {
    case PF_RGB:
        texture->SetFormat(nTexture2::R5G6B5);
        break;
    case PF_RGBA:
        texture->SetFormat(nTexture2::A8R8G8B8);
        break;
    }
    texture->SetWidth(bufWidth);
    texture->SetHeight(bufHeight);
    texture->Load();
    nTexture2::LockInfo lockInfo;
    if (texture->Lock(nTexture2::WriteOnly, 0, lockInfo)) {
        for (uint y = 0; y < bufHeight; y++) {
            CEGUI::uint8* dstBuf = (CEGUI::uint8*)lockInfo.surfPointer + y * lockInfo.surfPitch;
            CEGUI::uint32* srcBuf = (CEGUI::uint32*)bufPtr + y * bufWidth;
            memcpy(dstBuf, srcBuf, sizeof(CEGUI::uint32) * bufWidth);
        }
        texture->Unlock(0);
    }
    fromMemory = true;
}

//------------------------------------------------------------------------------
/**
*/
void Texture::updateFromMemory(const void* bufPtr, uint xOffset, uint yOffset, uint subWidth, uint subHeight) {
    n_assert(fromMemory);
    nTexture2::LockInfo lockInfo;
    if (texture->Lock(nTexture2::WriteOnly, 0, lockInfo)) {
        for (uint y = 0; y < subHeight; y++) {
            CEGUI::uint8* dstBuf = (CEGUI::uint8*)lockInfo.surfPointer + (yOffset + y) * lockInfo.surfPitch;
            CEGUI::uint32* srcBuf = (CEGUI::uint32*)bufPtr + y * subWidth;
            memcpy(dstBuf + sizeof(CEGUI::uint32) * xOffset, srcBuf, sizeof(CEGUI::uint32) * subWidth);
        }
        texture->Unlock(0);
    }
}

} // namespace CEUI
