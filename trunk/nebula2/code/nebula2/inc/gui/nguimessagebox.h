#ifndef N_GUIMESSAGEBOX_H
#define N_GUIMESSAGEBOX_H
//------------------------------------------------------------------------------
/**
    @class nGuiMessageBox
    @ingroup Gui

    @brief A complete modal message box window.

    The message box will emit a gui event (Dialog*) and call one of its
    OnOk(), OnCancel() virtual methods right before it is closed. You can
    either derive a subclass and override those methods, or check for the gui
    event to get the user's reaction.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

//------------------------------------------------------------------------------
class nGuiMessageBox : public nGuiClientWindow
{
public:
    /// message box types
    enum Type
    {
        NoButtons,          // no buttons at all
        Ok,                 // message box has one Ok button
        OkCancel,           // message box has Ok and Cancel button
    };

    /// constructor
    nGuiMessageBox();
    /// destructor
    virtual ~nGuiMessageBox();
    /// set message text
    void SetMessageText(const char* t);
    /// get message text
    const char* GetMessageText() const;
    /// set ok button text
    void SetOkText(const char* t);
    /// get ok button text
    const char* GetOkText() const;
    /// set cancel button text
    void SetCancelText(const char* t);
    /// get cancel button text
    const char* GetCancelText() const;
    /// set message box tye
    void SetType(Type t);
    /// get message box type
    Type GetType() const;
    /// set icon brush (optional)
    void SetIconBrush(const char* t);
    /// get icon brush
    const char* GetIconBrush() const;
    /// enable/disable automatic size computation
    void SetAutoSize(bool b);
    /// get automatic size flag
    bool GetAutoSize() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// called when ok button pressed
    virtual void OnOk();
    /// called when cancel button pressed
    virtual void OnCancel();

protected:
    nRef<nGuiTextLabel> refTextLabel;
    nRef<nGuiTextButton> refOkButton;
    nRef<nGuiTextButton> refCancelButton;
    nRef<nGuiButton> refIconButton;

    nString msgText;
    nString okText;
    nString cancelText;
    nString iconBrush;
    Type type;
    bool autoSize;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMessageBox::SetAutoSize(bool b)
{
    this->autoSize = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiMessageBox::GetAutoSize() const
{
    return this->autoSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMessageBox::SetIconBrush(const char* t)
{
    this->iconBrush = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiMessageBox::GetIconBrush() const
{
    return this->iconBrush.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMessageBox::SetMessageText(const char* t)
{
    this->msgText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiMessageBox::GetMessageText() const
{
    return this->msgText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMessageBox::SetOkText(const char* t)
{
    this->okText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiMessageBox::GetOkText() const
{
    return this->okText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMessageBox::SetCancelText(const char* t)
{
    this->cancelText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiMessageBox::GetCancelText() const
{
    return this->cancelText.Get();
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiMessageBox::SetType(Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiMessageBox::Type
nGuiMessageBox::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
#endif
