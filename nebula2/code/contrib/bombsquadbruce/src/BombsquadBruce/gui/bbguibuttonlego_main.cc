//-----------------------------------------------------------------------------
//  bbguibuttonlego_main.cc
//  (C) 2004 Rafael Van Daele-Hunt
//-----------------------------------------------------------------------------
#include "BombsquadBruce/gui/bbguibuttonlego.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(BBGuiButtonLego, "nguibutton");

//-----------------------------------------------------------------------------
/**
*/
BBGuiButtonLego::BBGuiButtonLego()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
BBGuiButtonLego::~BBGuiButtonLego()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
bool BBGuiButtonLego::Inside(const vector2& p)
{
    return ((nGuiWidget*) this->GetParent())->Inside(p);
}
//-----------------------------------------------------------------------------
/**
*/
bool BBGuiButtonLego::IsEnabled() const 
{
    return ((nGuiWidget*) this->GetParent()->GetParent())->IsEnabled();
}
//-----------------------------------------------------------------------------
/**
  Always returns false, so that the OnButtonDown command also gets routed 
  to other BBGuiButtonLegos rather than stopping with this one
  (see nGuiWidget::OnButtonDown()).

  The action taken by the "virtual button" can either be assigned arbitrarily
  to one of the BBGuiButtonLegos that make it up, or another metabutton
  on which the legos are created (could be invisible, or perhaps contain the 
  text and be otherwise transparent).

  It might simplify things to have made this class CCGuiLabelLego instead,
  but then I'd have had to add extra code to handle pressed brushes.
*/
bool BBGuiButtonLego::OnButtonDown(const vector2& mousePos)
{
    nGuiButton::OnButtonDown( mousePos );
    return false;
    
}