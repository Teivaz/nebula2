//------------------------------------------------------------------------------
//  nguibrush.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguibrush.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"
#include "gui/nguiresource.h"

//------------------------------------------------------------------------------
/**
    Initialize the gui resource attached to the brush.
*/
bool
nGuiBrush::Load()
{
    if (!this->name.IsEmpty())
    {
        n_assert(!this->IsLoaded());
        n_assert(!this->refSkin.isvalid());
        n_assert(0 == this->guiResource);

        nGuiServer* guiServer = nGuiServer::Instance();
        this->refSkin = guiServer->GetSkin();
        this->guiResource = this->refSkin->FindBrush(this->name.Get());
        if (0 == this->guiResource)
        {
            // try alternate skin...
            guiServer->ToggleSystemGui();
            this->refSkin = guiServer->GetSkin();
            this->guiResource = this->refSkin->FindBrush(this->name.Get());
            guiServer->ToggleSystemGui();
            if (0 == this->guiResource)
            {
                n_error("nGuiBrush: could not resolve brush '%s'!", this->name.Get());
                return false;
            }
        }
        n_assert(0 != this->guiResource);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    De-initialize the gui resource attached to the brush.
*/
void
nGuiBrush::Unload()
{
    if (this->guiResource)
    {
        this->guiResource = 0;
    }
    this->refSkin.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
nGuiResource*
nGuiBrush::GetGuiResource()
{
    if (!this->IsLoaded())
    {
        this->Load();
    }
    this->guiResource->SetTouched(true);
    return this->guiResource;
}
