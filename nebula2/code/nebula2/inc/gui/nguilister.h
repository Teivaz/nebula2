#ifndef N_GUILISTER_H
#define N_GUILISTER_H

//------------------------------------------------------------------------------
/**
    @class nGuiLister
    @ingroup NebulaGuiSystem

    A lister widget consists of vertically stacked text label widgets,
    a vertical slider wigdet, and a text entry widget. Those widgets
    must already exist as children of the lister widget, they will then
    be arranged and handled by the file lister.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"
#include "gui/nguitextentry.h"
#include "gui/nguitextlabel.h"
#include "gui/nguislider.h"

//------------------------------------------------------------------------------
class nGuiLister : public nGuiWidget
{
public:
    /// constructor
    nGuiLister();
    /// destructor
    virtual ~nGuiLister();
    /// rendering
    virtual bool Render();
    /// render audio
    virtual bool RenderAudio();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down
    virtual bool OnButtonDown(const vector2& mousePos);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// clear all text labels
    virtual void Clear();
    /// add text to textArray
    void AppendText(const char* text);
    /// get selected text
    const char* GetText() const;
    /// set selected label index
    void SetSelectedIndex(int index);
    /// get selected label index
    int GetSelectedIndex() const;
    /// get number of entries in list
    int GetNumEntries() const;
    /// set cursor brush
    void SetCursorBrush(const char* name);
    /// get cursor brush
    const char* GetCursorBrush() const;

protected:
    /// update the text labels
    void UpdateTextLabels();
    /// update the text labels from the slider range
    void UpdateTextLabelsFromSlider();
    /// draw the resource where the selected label is
    void DrawSelectionResource();

    nArray<nString> textArray;              // the text to be displayed in the labels
    int firstIndex;                         // index of first visible file in textArray
    int selectedIndex;                      // index of selected file in textArray
    nGuiTextEntry* textEntryWidget;         // the text entry widget
    nGuiSlider* sliderWidget;               // the slider widget
    nArray<nGuiTextLabel*> labelArray;      // the text label widgets
    vector2 curMousePos;
    rectangle textListRect;
    nString cursorBrush;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiLister::SetCursorBrush(const char* name)
{
    this->cursorBrush = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiLister::GetCursorBrush() const
{
    return this->cursorBrush.IsEmpty() ? 0 : this->cursorBrush.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiLister::GetNumEntries() const
{
    return this->textArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiLister::SetSelectedIndex(int index)
{
    n_assert((index >= 0) && (index < this->GetNumEntries()));
    this->selectedIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiLister::GetSelectedIndex() const
{
    return this->selectedIndex;
}

//------------------------------------------------------------------------------
#endif
