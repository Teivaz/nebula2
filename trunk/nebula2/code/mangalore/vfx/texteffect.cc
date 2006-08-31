//------------------------------------------------------------------------------
//  texteffect.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "vfx/texteffect.h"
#include "resource/nresourceserver.h"
#include "graphics/server.h"
#include "graphics/cameraentity.h"
#include "gfx2/ngfxserver2.h"

namespace VFX
{
ImplementRtti(VFX::TextEffect, VFX::Effect);
ImplementFactory(VFX::TextEffect);

//------------------------------------------------------------------------------
/**
*/
TextEffect::TextEffect() :
    text("No Text!"),
    fontName("GuiDefault"),
    color(1.0f, 1.0f, 1.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TextEffect::~TextEffect()
{
    if (!this->IsFinished())
    {
        this->OnDeactivate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
TextEffect::OnStart()
{
    Effect::OnStart();

    // resolve font name
    this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName, nResource::Font);
    n_assert(this->refFont.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
TextEffect::OnFrame()
{
    Effect::OnFrame();

    if (this->IsPlaying())
    {
        // get view-projection matrix from camera
        Graphics::CameraEntity* cameraEntity = Graphics::Server::Instance()->GetLevel()->GetCamera();
        n_assert(cameraEntity);
        const matrix44& viewProjection = cameraEntity->GetViewProjection();

        // transform our world-space position to screen-space
        const vector3& worldPos = this->ComputeWorldSpaceTransform().pos_component();
        vector3 screenPos = viewProjection.mult_divw(worldPos);
        screenPos.x = (screenPos.x * 0.5f) + 0.5f;
        screenPos.y = (-screenPos.y * 0.5f) + 0.5f;

        // get the text extents
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->SetFont(this->refFont);
        vector2 textExtent = gfxServer->GetTextExtent(this->text);

        // compute a 2d screen rectangle with the 3d world position as midpoint
        float x0 = screenPos.x - textExtent.x * 0.5f;
        float y0 = screenPos.y - textExtent.y * 0.5f;
        float x1 = screenPos.x + textExtent.x * 0.5f;
        float y1 = screenPos.y + textExtent.y * 0.5f;
        rectangle rect(vector2(x0, y0), vector2(x1, y1));

        // draw the text
        gfxServer->DrawText(this->text, this->color, rect, nFont2::Top|nFont2::Left, false);
    }
}

}; // namespace VFX