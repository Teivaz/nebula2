#ifndef N_GUICYCLEBUTTON_H
#define N_GUICYCLEBUTTON_H
//------------------------------------------------------------------------------
/**
    @class nGuiCycleButton
    @ingroup NebulaGuiSystem
    @brief A gui button which contains several text labels.
    
    Only one label can be active at a time. If the button will be clicked,
    the next label becomes active. (Note: The labels must be unique)
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiCycleButton : public nGuiTextLabel
{
public:
    /// constructor
    nGuiCycleButton();
    /// destructor
    virtual ~nGuiCycleButton();
    /// Rendering.
    virtual bool Render();
    /// handle mouse moved event
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up event
    virtual bool OnButtonUp(const vector2& mousePos);
    /// Extend text label list by `label'.
    void PutTextLabel(const char* label);
    /// Is text label `label' available?
    bool HasTextLabel(const char* label) const;
    /// Remove text label `label' if available.
    void RemoveTextLabel(const char* label);
    /// Remove all text labels
    void RemoveAllTextLabels();

protected:
    bool focus;
    bool pressed;
    nArray<nString> textLabels;
    int currentTextLabel;
};

//------------------------------------------------------------------------------
#endif

