//-----------------------------------------------------------------------------
//  ccguibuttonlego_main.cc
//  (C) 2004 Rafael Van Daele-Hunt
//-----------------------------------------------------------------------------
#include "BombsquadBruce/gui/ccguibuttonlego.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(CCGuiButtonLego, "nguibutton");

//-----------------------------------------------------------------------------
/**
*/
CCGuiButtonLego::CCGuiButtonLego()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
CCGuiButtonLego::~CCGuiButtonLego()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
bool CCGuiButtonLego::Inside(const vector2& p)
{
    return ((nGuiWidget*) this->GetParent())->Inside(p);
}
//-----------------------------------------------------------------------------
/**
*/
bool CCGuiButtonLego::IsEnabled() const 
{
    return ((nGuiWidget*) this->GetParent()->GetParent())->IsEnabled();
}
//-----------------------------------------------------------------------------
/**
  Always returns false, so that the OnButtonDown command also gets routed 
  to other CCGuiButtonLegos rather than stopping with this one
  (see nGuiWidget::OnButtonDown()).

  The action taken by the "virtual button" can either be assigned arbitrarily
  to one of the CCGuiButtonLegos that make it up, or another metabutton
  on which the legos are created (could be invisible, or perhaps contain the 
  text and be otherwise transparent).

  It might simplify things to have made this class CCGuiLabelLego instead,
  but then I'd have had to add extra code to handle pressed brushes.
*/
bool CCGuiButtonLego::OnButtonDown(const vector2& mousePos)
{
    nGuiButton::OnButtonDown( mousePos );
    return false;
    
}