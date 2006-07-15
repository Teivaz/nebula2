//------------------------------------------------------------------------------
//  nguicategorybrowser_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicategorybrowser.h"
#include "gui/nguitextbutton.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiCategoryBrowser, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiCategoryBrowser::nGuiCategoryBrowser() :
    dirPath("home:export/gfxlib"),
    lookUpEnabled(0)
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
    Updates the file lister from the currently selected entry in the
    dir lister.
*/
void
nGuiCategoryBrowser::UpdateFileLister()
{
    const char* selDir = this->refCatLister->GetSelection();
    if (selDir)
    {
        nString dirName = this->refCatLister->GetDirectory();
        dirName.Append("/");
        dirName.Append(selDir);
        this->refFileLister->SetDirectory(dirName.Get());
    }
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
    catLister->SetLookUpEnabled(this->lookUpEnabled);
    this->AttachForm(catLister, Top, 0.0f);
    this->AttachForm(catLister, Left, 0.0f);
    this->AttachPos(catLister, Right, 0.295f);
    this->AttachForm(catLister, Bottom, 0.0f);
    catLister->OnShow();
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
    fileLister->SetLookUpEnabled(this->lookUpEnabled);
    this->AttachForm(fileLister, Top, 0.0f);
    this->AttachForm(fileLister, Right, 0.0f);
    this->AttachPos(fileLister, Left, 0.305f);
    this->AttachForm(fileLister, Bottom, 0.0f);
    fileLister->OnShow();
    this->refFileLister = fileLister;

    kernelServer->PopCwd();

    this->UpdateFileLister();
    nGuiServer::Instance()->RegisterEventListener(this);
    nGuiFormLayout::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiCategoryBrowser::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->ClearAttachRules();
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
            this->UpdateFileLister();
        }
        if (event.GetWidget() == this->refFileLister.get())
        {
            // file entry has changed
            nGuiEvent event(this, nGuiEvent::SelectionChanged);
            nGuiServer::Instance()->PutEvent(event);
        }
    } 
    else if (event.GetType() == nGuiEvent::SelectionDblClicked)
    {
        // pass through doubleclick event
        if (event.GetWidget() == this->refFileLister.get())
        {
            nGuiEvent event(this, nGuiEvent::SelectionDblClicked);
            nGuiServer::Instance()->PutEvent(event);
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
