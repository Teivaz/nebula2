//------------------------------------------------------------------------------
//  nguicategorybrowser_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicategorybrowser.h"
#include "gui/nguidirlister.h"
#include "gui/nguitextbutton.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiCategoryBrowser, "nguiformlayout");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguicategorybrowser

    @cppclass
    nGuiCategoryBrowser
    
    @superclass
    nguiformlayout
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiCategoryBrowser::nGuiCategoryBrowser() :
    dirPath("home:export/gfxlib")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiCategoryBrowser::~nGuiCategoryBrowser()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCategoryBrowser::OnShow()
{
    kernelServer->PushCwd(this);

    // create the category lister widget
    nGuiDirLister* catLister = (nGuiDirLister*) kernelServer->New("nguidirlister", "CatLister");
    n_assert(catLister);
    catLister->SetDefaultBrush("list_background");
    catLister->SetHighlightBrush("list_selection");
    catLister->SetFont("GuiSmall");
    catLister->SetDirectory(this->GetDirectory());
    catLister->SetIgnoreSubDirs(false);
    catLister->SetIgnoreFiles(true);
    catLister->SetSelectionEnabled(true);
    this->AttachForm(catLister, Top, 0.0f);
    this->AttachForm(catLister, Left, 0.0f);
    this->AttachPos(catLister, Right, 0.295f);
    this->AttachForm(catLister, Bottom, 0.0f);
    this->refCatLister = catLister;

    // create the file lister widget
    nGuiDirLister* fileLister = (nGuiDirLister*) kernelServer->New("nguidirlister", "FileLister");
    n_assert(fileLister);
    fileLister->SetDefaultBrush("list_background");
    fileLister->SetHighlightBrush("list_selection");
    fileLister->SetFont("GuiSmall");
    fileLister->SetIgnoreSubDirs(true);
    fileLister->SetIgnoreFiles(false);
    fileLister->SetSelectionEnabled(true);
    this->AttachForm(fileLister, Top, 0.0f);
    this->AttachForm(fileLister, Right, 0.0f);
    this->AttachPos(fileLister, Left, 0.305f);
    this->AttachForm(fileLister, Bottom, 0.0f);
    this->refFileLister = fileLister;

    kernelServer->PopCwd();

    this->refGuiServer->RegisterEventListener(this);

    nGuiFormLayout::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiCategoryBrowser::OnHide()
{
    this->refGuiServer->UnregisterEventListener(this);

    if (this->refCatLister.isvalid())
    {
        this->refCatLister->Release();
        n_assert(!this->refCatLister.isvalid());
    }
    if (this->refFileLister.isvalid())
    {
        this->refFileLister->Release();
        n_assert(!this->refFileLister.isvalid());
    }
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiCategoryBrowser::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::SelectionChanged)
    {
        if (event.GetWidget() == this->refCatLister.get())
        {
            // category has changed, update file lister
            const char* selDir = this->refCatLister->GetSelection();
            n_assert(selDir);
            nString dirName = this->refCatLister->GetDirectory();
            dirName.Append("/");
            dirName.Append(selDir);
            this->refFileLister->SetDirectory(dirName.Get());
        }
        if (event.GetWidget() == this->refFileLister.get())
        {
            // file entry has changed
            nGuiEvent event(this, nGuiEvent::SelectionChanged);
            this->refGuiServer->PutEvent(event);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Returns the currently selected absolute path name, or an emtpy
    string, if no valid selection exists.
*/
nString
nGuiCategoryBrowser::GetSelectedPath() const
{
    nString path;
    if (this->refFileLister.isvalid())
    {
        const char* rootDir = this->refFileLister->GetDirectory();
        const char* selection = this->refFileLister->GetSelection();
        if (selection)
        {
            path = rootDir;
            path.Append("/");
            path.Append(selection);
        }
    }
    return path;
}
