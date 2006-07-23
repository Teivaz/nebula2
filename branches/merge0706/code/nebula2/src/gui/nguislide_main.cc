//------------------------------------------------------------------------------
//  nguislide_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "gui/nguislide.h"
#include "gui/nguiresource.h"

nNebulaClass(nGuiSlide, "gui::nguilabel");

//------------------------------------------------------------------------------
/**
*/
nGuiSlide::nGuiSlide()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSlide::~nGuiSlide()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlide::SetRenderColor(vector4 color)
{
    if(!this->defaultBrush.GetName().IsEmpty())
    {
        nGuiResource* res = this->defaultBrush.GetGuiResource();
        res->SetColor(color);
    }
}
