#ifndef N_GUIWINDOW_H
#define N_GUIWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiWindow
    @ingroup NebulaGuiSystem
    @brief A window which may contain widgets.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiWindow : public nGuiWidget
{
public:
    /// constructor
    nGuiWindow();
    /// destructor
    virtual ~nGuiWindow();
    /// render the window and all contained widgets
    virtual bool Render();
    /// make window the new focus window
    virtual void SetFocusWindow(nGuiWindow* window);
    /// get topmost visible child window
    virtual nGuiWindow* GetTopMostWindow();
    /// handle mouse move, route to focus window if exists
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down, route to focus window if exists
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up, route to focus window if exists
    virtual bool OnButtonUp(const vector2& mousePos);
    /// handle right button down, route to focus window if exists
    virtual bool OnRButtonDown(const vector2& mousePos);
    /// handle right button up, route to focus window if exists
    virtual bool OnRButtonUp(const vector2& mousePos);
    /// handle character key event, route to focus window if exists
    virtual void OnChar(uchar charCode);
    /// handle a key down event, route to focus window if exists
    virtual bool OnKeyDown(nKey key);
    /// handle a key up event, route to focus window if exists
    virtual void OnKeyUp(nKey key);
    /// called per frame
    virtual void OnFrame();
    /// set modal flag
    void SetModal(bool b);
    /// get modal flag
    bool IsModal() const;
    /// set escape key script handler
    void SetEscapeCommand(const char* cmd);
    /// get escape key script handler
    const char* GetEscapeCommand() const;

protected:
    nString escapeCommand;
    nClass* windowClass;
    bool modal;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetEscapeCommand(const char* cmd)
{
    this->escapeCommand = cmd;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiWindow::GetEscapeCommand() const
{
    return this->escapeCommand.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiWindow::SetModal(bool b)
{
    this->modal = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiWindow::IsModal() const
{
    return this->modal;
}

//------------------------------------------------------------------------------
#endif

