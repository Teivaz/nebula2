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
*/
nGuiResource*
nGuiBrush::GetGuiResource()
{
    if (!this->refSkin.isvalid())
    {
        nGuiServer* guiServer = nGuiServer::Instance();

        // validate gui resource
        // FIXME: increment ref count of gui resource texture
        this->refSkin = guiServer->GetSkin();
        this->guiResource = this->refSkin->FindBrush(this->name.Get());
        if (0 == this->guiResource)
        {
            // hmm, try alternate skin
            guiServer->ToggleSystemGui();
            this->refSkin = guiServer->GetSkin();
            this->guiResource = this->refSkin->FindBrush(this->name.Get());
            guiServer->ToggleSystemGui();
            if (0 == this->guiResource)
            {
                n_error("nGuiBrush: could not resolve brush '%s'!", this->name.Get());
                return 0;
            }
        }
    }
    n_assert(0 != this->guiResource);
    return this->guiResource;
}
