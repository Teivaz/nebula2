#ifndef N_GUICATEGORYBROWSER_H
#define N_GUICATEGORYBROWSER_H
//------------------------------------------------------------------------------
/**
    @class nGuiCategoryBrowser
    @ingroup NebulaGuiSystem

    A category/file browser widget.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"

class nGuiDirLister;

//------------------------------------------------------------------------------
class nGuiCategoryBrowser : public nGuiFormLayout
{
public:
    /// constructor
    nGuiCategoryBrowser();
    /// destructor
    virtual ~nGuiCategoryBrowser();
    /// set the root directory
    void SetDirectory(const char* dir);
    /// get the root directory
    const char* GetDirectory() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// get the currently selected path, or empty string if no valid selection
    nString GetSelectedPath() const;

protected:
    nString dirPath;
    nRef<nGuiDirLister>  refCatLister;
    nRef<nGuiDirLister>  refFileLister;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCategoryBrowser::SetDirectory(const char* dir)
{
    n_assert(dir);
    this->dirPath = dir;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiCategoryBrowser::GetDirectory() const
{
    return this->dirPath.Get();
}

//------------------------------------------------------------------------------
#endif

    
