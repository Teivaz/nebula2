#ifndef N_GUIFILEDIALOG_H
#define N_GUIFILEDIALOG_H
//------------------------------------------------------------------------------
/**
    A file dialog base class. Implement OnOk() and OnCancel() behaviour in a 
    derived subclass.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"

class nGuiDirLister;
class nGuiTextEntry;
class nGuiTextButton;

//------------------------------------------------------------------------------
class nGuiFileDialog : public nGuiClientWindow
{
public:
    /// constructor
    nGuiFileDialog();
    /// destructor
    virtual ~nGuiFileDialog();
    /// set the root directory
    void SetDirectory(const char* path);
    /// get the root directory
    const char* GetDirectory() const;
    /// select between save or load mode
    void SetSaveMode(bool b);
    /// get save/load mode
    bool GetSaveMode() const;
    /// set text for ok button
    void SetOkText(const char* t);
    /// get text for ok button
    const char* GetOkText() const;
    /// set text for cancel button
    void SetCancelText(const char* t);
    /// get text for cancel button
    const char* GetCancelText() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// called when ok button pressed
    virtual bool OnOk();
    /// called when cancel button pressed
    virtual bool OnCancel();

protected:
    nRef<nGuiDirLister> refDirLister;
    nRef<nGuiTextEntry> refTextEntry;
    nRef<nGuiTextButton> refOkButton;
    nRef<nGuiTextButton> refCancelButton;

    nString dirPath;
    bool saveMode;
    nString okText;
    nString cancelText;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetDirectory(const char* path)
{
    n_assert(path);
    this->dirPath = path;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetDirectory() const
{
    return this->dirPath.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetSaveMode(bool b)
{
    this->saveMode = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiFileDialog::GetSaveMode() const
{
    return this->saveMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetOkText(const char* t)
{
    n_assert(t);
    this->okText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetOkText() const
{
    return this->okText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetCancelText(const char* t)
{
    this->cancelText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetCancelText() const
{
    return this->cancelText.Get();
}

//------------------------------------------------------------------------------
#endif
