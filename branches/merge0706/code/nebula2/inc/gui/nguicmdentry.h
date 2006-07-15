#ifndef N_GUICMDENTRY_H
#define N_GUICMDENTRY_H
//------------------------------------------------------------------------------
/**
    @class nGuiCmdEntry
    @ingroup Gui
    @brief A specialized text view widget which displays the current 
    kernel server's line buffer and lets the user enter script commands.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguitextview.h"
#include "util/neditline.h"
#include "util/ntabcomplete.h"

//------------------------------------------------------------------------------
class nGuiCmdEntry : public nGuiTextView
{
public:
    /// constructor
    nGuiCmdEntry();
    /// destructor
    virtual ~nGuiCmdEntry();
    /// called when widget is shown
    virtual void OnShow();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// handle character code entry
    virtual void OnChar(uchar charCode);
    /// handle key down event
    virtual bool OnKeyDown(nKey key);
    /// handle a key up event
    virtual void OnKeyUp(nKey key);

protected:
    /// update the internal line array from the kernel server's log buffer
    void UpdateLineArray();
    /// append the current cmd line
    void AppendCmdLine();
    /// execute the current command
    void ExecuteCommand();
    /// add current command to history
    void AddCommandToHistory();
    /// recall next command in history
    void RecallNextCommand();
    /// recall previous command in history
    void RecallPrevCommand();
    /// set local cwd
    void SetCwd(nRoot* cwd);
    /// get local cwd (can return 0)
    nRoot* GetCwd() const;
    /// update the tab completion stuff
    void UpdateTabComplete();
    /// perform a tab completion
    void DoTabCompletion();

    nAutoRef<nScriptServer> refScriptServer;
    nEditLine editLine;
    bool ctrlDown;
    int historyCursor;
    nArray<nString> cmdHistory;
    nRef<nRoot> refCwd;
    nTabComplete childTabComplete;
};

//------------------------------------------------------------------------------
/**
    Set the local cwd. A null pointer is valid (this means the global root 
    object).
*/
inline
void
nGuiCmdEntry::SetCwd(nRoot* cwd)
{
    this->refCwd = cwd;
}

//------------------------------------------------------------------------------
/**
    Returns the local cwd. May return a null pointer.
*/
inline
nRoot*
nGuiCmdEntry::GetCwd() const
{
    return this->refCwd.isvalid() ? this->refCwd.get() : 0;
}

//------------------------------------------------------------------------------
#endif    
