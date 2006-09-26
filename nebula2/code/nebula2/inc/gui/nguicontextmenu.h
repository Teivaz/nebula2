#ifndef N_GUICONTEXTMENU_H
#define N_GUICONTEXTMENU_H
//------------------------------------------------------------------------------
/**
    @class nGuiContextMenu
    @ingroup Gui

    @brief A context menu base class. Implement OnEvent() behavior in a
    derived subclass.

    (C) 2004 RadonLabs GmbH
*/
#include "util/narray.h"
#include "gui/nguiclientwindow.h"

class nGuiTextButton;

//------------------------------------------------------------------------------
class nGuiContextMenu : public nGuiClientWindow
{
public:
    /// constructor
    nGuiContextMenu();
    /// destructor
    virtual ~nGuiContextMenu();
    /// add a menu entry
    void AddEntry(const char* title);
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// get resulting selection index (-1 if no selection)
    int GetSelectedIndex() const;
    /// get resulting selection entry text (0 if no selection)
    const char* GetSelectedEntry() const;

protected:
    nArray<nString> entryArray;
    nArray< nRef<nGuiTextButton> > buttonArray;
    int selectedIndex;
};

//------------------------------------------------------------------------------
/**
    Creates a text button as a menu entry
*/
inline
void
nGuiContextMenu::AddEntry(const char* entry)
{
    n_assert(entry);
    this->entryArray.Append(entry);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiContextMenu::GetSelectedIndex() const
{
    return this->selectedIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiContextMenu::GetSelectedEntry() const
{
    if (-1 == this->selectedIndex)
    {
        return 0;
    }
    else
    {
        return this->entryArray[this->selectedIndex].Get();
    }
}

//------------------------------------------------------------------------------
#endif
