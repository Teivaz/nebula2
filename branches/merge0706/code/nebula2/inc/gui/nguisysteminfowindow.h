#ifndef N_GUISYSTEMINFOWINDOW_H
#define N_GUISYSTEMINFOWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiSystemInfoWindow
    @ingroup Gui
    @brief A Nebula system information window.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nResourceServer;
class nGuiTextView;

//------------------------------------------------------------------------------
class nGuiSystemInfoWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiSystemInfoWindow();
    /// destructor
    virtual ~nGuiSystemInfoWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();

private:
    /// update the text view with current data
    void UpdateTextView();

    nAutoRef<nResourceServer> refResourceServer;
    nRef<nGuiTextView>  refTextView;
};
//------------------------------------------------------------------------------
#endif

