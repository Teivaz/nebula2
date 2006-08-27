#ifndef N_GUITEXBROWSERWINDOW_H
#define N_GUITEXBROWSERWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiTexBrowserWindow
    @ingroup Gui
    @brief A window to browse the currently loaded textures and display
    information about them.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiTextView;
class nTexture2;
class nGuiButton;
class nGuiTextureView;
class nGuiTextEntry;

//------------------------------------------------------------------------------
class nGuiTexBrowserWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiTexBrowserWindow();
    /// destructor
    virtual ~nGuiTexBrowserWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// called on keyup
    virtual void OnKeyUp(nKey key);

protected:
    /// set to next texture
    void SetNextTexture();
    /// set to previous texture
    void SetPrevTexture();
    /// set texture by index
    void SetTextureByIndex(int index);
    /// update the text info field
    void UpdateInfoField();
    /// update the texture view
    void UpdateTextureView();
    /// update the window title
    void UpdateTitle();

private:
    int CountTextures();

    nRef<nGuiButton>      refPrevButton;
    nRef<nGuiButton>      refNextButton;
    nRef<nGuiTextView>    refInfoField;
    nRef<nGuiTextureView> refTexView;
    nRef<nGuiTextEntry>   refTextureIndexEntry;

    nAutoRef<nRoot>    refTextureRoot;
    nRef<nTexture2>    refCurrentTexture;

    int curTexturePosition;
};

//------------------------------------------------------------------------------
#endif

