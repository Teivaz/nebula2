#ifndef N_GUIWATCHERWINDOW_H
#define N_GUIWATCHERWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiWatcherWindow
    @ingroup Gui
    @brief A debug watcher window.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiTextLabel;
class nGuiTextEntry;
class nGuiTextView;

//------------------------------------------------------------------------------
class nGuiWatcherWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiWatcherWindow();
    /// destructor
    virtual ~nGuiWatcherWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();

private:
    /// update the text view with new watcher pattern
    void UpdateTextView();

    nRef<nGuiTextLabel> refTextLabel;
    nRef<nGuiTextEntry> refTextEntry;
    nRef<nGuiTextView>  refTextView;

    nClass* envClass;
};

//------------------------------------------------------------------------------
#endif

