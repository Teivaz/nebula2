#ifndef N_GUIFLIPBUTTON_H
#define N_GUIFLIPBUTTON_H
//------------------------------------------------------------------------------
/**
	@class nGuiFlipButton
    @ingroup NebulaGuiSystem

    A button which flips through several settings on click.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiFlipButton : public nGuiTextLabel
{
public:
    /// constructor
    nGuiFlipButton();
    /// destructor
    virtual ~nGuiFlipButton();
    /// Rendering.
    virtual bool Render();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up
    virtual bool OnButtonUp(const vector2& mousePos);
    /// add state
    void AddState(const char* text);
    /// set current state index
    void SetState(int i);
    /// get current state index
    int GetState() const;

private:
    bool mouseOver;
    nArray<nString> textArray;
    int curState;
    bool pressed;
};

//------------------------------------------------------------------------------
#endif
