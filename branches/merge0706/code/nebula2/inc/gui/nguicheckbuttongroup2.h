#ifndef N_GUICHECKBUTTONGROUP2_H
#define N_GUICHECKBUTTONGROUP2_H
//------------------------------------------------------------------------------
/**
    @class nGuiCheckButtonGroup2
    @ingroup Gui

    @brief A new style check button group which creates its check buttons and
    places them in a horizontal row.
    
    (C) 2004 RadonLabs GmbH
*/    
#include "gui/nguiformlayout.h"
#include "gui/nguicheckbutton.h"

//------------------------------------------------------------------------------
class nGuiCheckButtonGroup2 : public nGuiFormLayout
{
public:
    /// constructor
    nGuiCheckButtonGroup2();
    /// destructor
    virtual ~nGuiCheckButtonGroup2();
    /// set options
    void SetOptions(const nArray<nString>& o);
    /// get options
    const nArray<nString>& GetOptions() const;
    /// set (optional) default brush names
    void SetDefaultBrushes(const nArray<nString>& b);
    /// get default brush names
    const nArray<nString>& GetDefaultBrushes() const;
    /// set (optional) pressed brush names
    void SetPressedBrushes(const nArray<nString>& b);
    /// get pressed brush names
    const nArray<nString>& GetPressedBrushes() const;
    /// set (optional) highlight brush names
    void SetHighlightBrushes(const nArray<nString>& b);
    /// get highlight brush names
    const nArray<nString>& GetHighlightBrushes() const;
    /// set button spacing
    void SetButtonSpacing(float s);
    /// get button spacing
    float GetButtonSpacing() const;
    /// append an option
    void AppendOption(const char* str);
    /// append an tooltip
    void AppendTooltip(const char* str);
    /// set current selection
    void SetSelection(int index);
    /// get current selection
    int GetSelection() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// set font of check buttons
    virtual void SetFont(const char* str);

protected:

    /// update the child check button status
    void UpdateCheckButtons();

    int selIndex;
    nArray<nString> options;
    nArray<nString> tooltips;
    nArray<nRef<nGuiCheckButton> > refCheckButtons;

private:

    float buttonSpacing;
    nString font;
    nArray<nString> defaultBrushes;
    nArray<nString> pressedBrushes;
    nArray<nString> highlightBrushes;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetButtonSpacing(float s)
{
    this->buttonSpacing = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiCheckButtonGroup2::GetButtonSpacing() const
{
    return this->buttonSpacing;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetDefaultBrushes(const nArray<nString>& b)
{
    this->defaultBrushes = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nGuiCheckButtonGroup2::GetDefaultBrushes() const
{
    return this->defaultBrushes;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetPressedBrushes(const nArray<nString>& b)
{
    this->pressedBrushes = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nGuiCheckButtonGroup2::GetPressedBrushes() const
{
    return this->pressedBrushes;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetHighlightBrushes(const nArray<nString>& b)
{
    this->highlightBrushes = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nGuiCheckButtonGroup2::GetHighlightBrushes() const
{
    return this->highlightBrushes;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetOptions(const nArray<nString>& o)
{
    this->options = o;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nString>&
nGuiCheckButtonGroup2::GetOptions() const
{
    return this->options;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::AppendOption(const char* str)
{
    this->options.Append(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::AppendTooltip(const char* str)
{
    this->tooltips.Append(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetSelection(int index)
{
    this->selIndex = index;
    this->UpdateCheckButtons();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiCheckButtonGroup2::GetSelection() const
{
    return this->selIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCheckButtonGroup2::SetFont(const char* str)
{
    this->font = str;
}

//------------------------------------------------------------------------------
#endif
