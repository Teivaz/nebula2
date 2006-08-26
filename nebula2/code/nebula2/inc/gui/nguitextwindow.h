#ifndef N_GUITEXTWINDOW_H
#define N_GUITEXTWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiTextWindow
    @ingroup Gui

    @brief A text viewer window which can display text from a text file.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiTextView;
class nGuiTextButton;

//------------------------------------------------------------------------------
class nGuiTextWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiTextWindow();
    /// destructor
    virtual ~nGuiTextWindow();
    /// set text filename
    void SetFilename(const nString& name);
    /// get text filename
    const nString& GetFilename() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);

private:
    /// update text view from file content
    void UpdateTextView();

    nString filename;
    nRef<nGuiTextView> refTextView;
    nRef<nGuiTextButton> refCloseButton;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTextWindow::SetFilename(const nString& name)
{
    this->filename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiTextWindow::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
#endif

