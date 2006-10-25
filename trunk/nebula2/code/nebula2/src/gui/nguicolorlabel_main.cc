//------------------------------------------------------------------------------
//  nguicolorlabel_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicolorlabel.h"
#include "gui/nguiserver.h"
#include "gfx2/ntexture2.h"

nNebulaClass(nGuiColorLabel, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiColorLabel::nGuiColorLabel() :
    color(1.0f, 1.0f, 1.0f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiColorLabel::~nGuiColorLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorLabel::OnShow()
{
    nGuiWidget::OnShow();

    // initialize white texture
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    nString texName = skin->GetTexturePrefix();
    texName.Append("white");
    texName.Append(skin->GetTexturePostfix());
    nTexture2* tex = nGfxServer2::Instance()->NewTexture(texName.Get());
    if (!tex->IsValid())
    {
        tex->SetFilename(texName);
        bool loaded = tex->Load();
        if (!loaded)
        {
            n_error("nGuiColorLabel: failed to load texture '%s'!\n", texName.Get());
        }
    }
    this->refTexture = tex;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorLabel::OnHide()
{
    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }
    nGuiWidget::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiColorLabel::Render()
{
    if (this->IsShown())
    {
        n_assert(this->refTexture.isvalid());

        // render the texture
        static const rectangle uvs(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f));
        nGuiServer::Instance()->DrawTexture(this->GetScreenSpaceRect(), uvs, this->color, this->refTexture.get());
        return true;
    }
    return false;
}

