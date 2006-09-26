#ifndef N_GUIFILEDIALOG_H
#define N_GUIFILEDIALOG_H
//------------------------------------------------------------------------------
/**
    @class nGuiFileDialog
    @ingroup Gui
    @brief A file dialog base class.

    Implement OnOk() and OnCancel() behavior in a derived subclass.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"
#include "gui/nguidirlister.h"

class nGuiTextEntry;
class nGuiTextButton;
class nGuiMessageBox;

//------------------------------------------------------------------------------
class nGuiFileDialog : public nGuiClientWindow
{
public:
    /// text ids
    enum TextId
    {
        Ok,
        Cancel,
        Delete,
        DeleteMessage,
        DeleteOk,
        DeleteCancel,
        OverwriteMessage,
        OverwriteOk,
        OverwriteCancel,

        // keep at end
        NumTextIds,
    };

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
    /// set optional pattern
    void SetPattern(const char* pat);
    /// get optional pattern
    const char* GetPattern() const;
    /// enable/disable strip extension
    void SetStripExtension(bool b);
    /// get strip extension mode
    bool GetStripExtension() const;
    /// set optional file extension
    void SetExtension(const char* ext);
    /// get optional file extension
    const char* GetExtension() const;
    /// set text for one of the gui elements
    void SetText(TextId id, const char* text);
    /// get the text for one of the gui elements
    const char* GetText(TextId id) const;
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
    /// set the initial filename
    void SetInitialFilename(const char* f);
    /// get the initial filename
    const char* GetInitialFilename() const;
    /// get selected filename (valid during OnOk())
    const char* GetFilename() const;

protected:
    /// set selected filename
    void SetFilename(const char* s);
    /// check if file exists and post a message box
    bool CheckFileExists();
    /// extracts filename either from selection or from text entry field
    void ExtractFilename();
    /// handle the complete Ok action
    void HandleOk();
    /// handle the delete action
    void HandleDelete();
    /// called when the user confirms deletion
    bool DeleteFile();
    /// Set initial selection enabled/disabled
    void SetInitialSelectionEnabled(bool b);
    /// Sets the textcolor of the dirlister
    void SetDirlisterColor(vector4& color);
    /// Sets the textcolor of the textentry
    void SetEntryColor(vector4& color);

    nRef<nGuiDirLister> refDirLister;
    nRef<nGuiTextEntry> refTextEntry;
    nRef<nGuiTextButton> refOkButton;
    nRef<nGuiTextButton> refCancelButton;
    nRef<nGuiMessageBox> refMessageBox;
    nRef<nGuiMessageBox> refDeleteMessageBox;
    nRef<nGuiTextButton> refDeleteButton;

    vector4 dirlisterTextColor;
    vector4 entryTextColor;
    nString dirPath;
    nString extension;
    nString initialFilename;
    bool saveMode;
    nString text[NumTextIds];
    nString filename;
    bool initialSelection;
    bool stripExtension;
    nString pattern;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetEntryColor(vector4& color)
{
    this->entryTextColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetDirlisterColor(vector4& color)
{
    this->dirlisterTextColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetExtension(const char* ext)
{
    this->extension = ext;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetExtension() const
{
    return this->extension.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetInitialSelectionEnabled(bool b)
{
    this->initialSelection = b;
}

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
nGuiFileDialog::SetText(TextId id, const char* t)
{
    n_assert((id >= 0) && (id < NumTextIds));
    this->text[id] = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetText(TextId id) const
{
    n_assert((id >= 0) && (id < NumTextIds));
    return this->text[id].Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetFilename(const char* s)
{
    this->filename = s;
}

//------------------------------------------------------------------------------
/**
    Get the selected filename. Returns 0 if nothing selected.
*/
inline
const char*
nGuiFileDialog::GetFilename() const
{
    return this->filename.IsEmpty() ? 0 : this->filename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetPattern(const char* p)
{
    n_assert(p);
    this->pattern = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetPattern() const
{
    return this->pattern.Get();
}

//------------------------------------------------------------------------------
/**
    Enable disable strip extension.
*/
inline
void
nGuiFileDialog::SetStripExtension(bool b)
{
    this->stripExtension = b;
}

//------------------------------------------------------------------------------
/**
    Get strip extension mode.
*/
inline
bool
nGuiFileDialog::GetStripExtension() const
{
    return this->stripExtension;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFileDialog::SetInitialFilename(const char* f)
{
    this->initialFilename = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFileDialog::GetInitialFilename() const
{
    return this->initialFilename.Get();
}

//------------------------------------------------------------------------------
#endif
