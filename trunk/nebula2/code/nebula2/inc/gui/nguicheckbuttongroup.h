#ifndef N_GUICHECKBUTTONGROUP_H
#define N_GUICHECKBUTTONGROUP_H
//------------------------------------------------------------------------------
/**
	@class nGuiCheckButtonGroup
    @ingroup NebulaGuiSystem

    Groups child check buttons so that only one is selected at any time.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiCheckButtonGroup : public nGuiWidget
{
public:
    /// constructor
    nGuiCheckButtonGroup();
    /// destructor
    virtual ~nGuiCheckButtonGroup();
    /// rendering
    virtual bool Render();
    /// audio rendering
    virtual bool RenderAudio();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// is position inside?
    virtual bool Inside(const vector2& p);
    /// set the current "on button"
    void SetOnButtonIndex(int index);
    /// get the current "on button"
    int GetOnButtonIndex() const;
};
//------------------------------------------------------------------------------
#endif

