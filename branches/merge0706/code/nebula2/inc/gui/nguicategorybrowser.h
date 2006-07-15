#ifndef N_GUICATEGORYBROWSER_H
#define N_GUICATEGORYBROWSER_H
//------------------------------------------------------------------------------
/**
    @class nGuiCategoryBrowser
    @ingroup Gui
    @brief A category/file browser widget.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"
#include "gui/nguidirlister.h"

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
    /// enable/disable quick lookup by keypressing
    void SetLookUpEnabled(bool b);
    /// is lookup enabled ?
    bool GetLookUpEnabled();

protected:
    /// update the file lister from the current category selection
    void UpdateFileLister();
    nString dirPath;
    nRef<nGuiDirLister>  refCatLister;
    nRef<nGuiDirLister>  refFileLister;
    bool lookUpEnabled ;
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
/**
*/
inline
void
nGuiCategoryBrowser::SetLookUpEnabled(bool b)
{
    this->lookUpEnabled  = b;
}
//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiCategoryBrowser::GetLookUpEnabled()
{
    return this->lookUpEnabled ;
}
//------------------------------------------------------------------------------
#endif
