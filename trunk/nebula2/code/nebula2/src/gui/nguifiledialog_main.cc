//------------------------------------------------------------------------------
//  nguifiledialog_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguifiledialog.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"
#include "gui/nguidirlister.h"
#include "gui/nguitextbutton.h"
#include "gui/nguitextentry.h"
#include "gui/nguiskin.h"

nNebulaClass(nGuiFileDialog, "nguiclientwindow");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguifiledialog

    @cppclass
    nGuiFileDialog
    
    @superclass
    nguiclientwindow
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiFileDialog::nGuiFileDialog() :
    dirPath("home:"),
    saveMode(false),
    okText("Ok"),
    cancelText("Cancel")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiFileDialog::~nGuiFileDialog()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiFileDialog::OnShow()
{
    nGuiSkin* skin = this->refGuiServer->GetSkin();
    n_assert(skin);

    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    if (0 == this->GetTitle())
    {
        if (this->GetSaveMode())
        {
            this->SetTitle("Save File");
        }
        else
        {
            this->SetTitle("Load File");
        }
    }

    // get client area layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    vector2 buttonSize = this->refGuiServer->ComputeScreenSpaceBrushSize("button_n");
    vector2 textSize = this->refGuiServer->ComputeScreenSpaceBrushSize("textentry_n");

    // create Cancel button
    nGuiTextButton* cancelButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "CancelButton");
    n_assert(cancelButton);
    cancelButton->SetText(this->GetCancelText());
    cancelButton->SetFont("GuiSmall");
    cancelButton->SetAlignment(nGuiTextButton::Center);
    cancelButton->SetDefaultBrush("button_n");
    cancelButton->SetPressedBrush("button_p");
    cancelButton->SetHighlightBrush("button_h");
    cancelButton->SetMinSize(buttonSize);
    cancelButton->SetMaxSize(buttonSize);
    cancelButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(cancelButton, nGuiFormLayout::Right, 0.005f);
    layout->AttachForm(cancelButton, nGuiFormLayout::Bottom, 0.005f);
    cancelButton->OnShow();
    this->refCancelButton = cancelButton;

    // create Ok button
    nGuiTextButton* okButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "OkButton");
    n_assert(okButton);
    okButton->SetText(this->GetOkText());
    okButton->SetFont("GuiSmall");
    okButton->SetAlignment(nGuiTextButton::Center);
    okButton->SetDefaultBrush("button_n");
    okButton->SetPressedBrush("button_p");
    okButton->SetHighlightBrush("button_h");
    okButton->SetMinSize(buttonSize);
    okButton->SetMaxSize(buttonSize);
    okButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(okButton, nGuiFormLayout::Left, 0.005f);
    layout->AttachForm(okButton, nGuiFormLayout::Bottom, 0.005f);
    okButton->OnShow();
    this->refOkButton = okButton;

    // optional text entry
    if (this->GetSaveMode())
    {
        nGuiTextEntry* textEntry = (nGuiTextEntry*) kernelServer->New("nguitextentry", "TextEntry");
        n_assert(textEntry);
        textEntry->SetFont("GuiSmall");
        textEntry->SetAlignment(nGuiTextEntry::Left);
        textEntry->SetDefaultBrush("textentry_n");
        textEntry->SetPressedBrush("textentry_p");
        textEntry->SetHighlightBrush("textentry_h");
        textEntry->SetCursorBrush("textcursor");
        textEntry->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
        textEntry->SetShadowColor(vector4(0.0f, 0.0f, 0.0f, 0.0f));
        textEntry->SetMinSize(vector2(0.0f, textSize.y));
        textEntry->SetMaxSize(vector2(1.0f, textSize.y));
        layout->AttachForm(textEntry, nGuiFormLayout::Left, 0.005f);
        layout->AttachForm(textEntry, nGuiFormLayout::Right, 0.005f);
        layout->AttachWidget(textEntry, nGuiFormLayout::Bottom, okButton, 0.005f);
        textEntry->OnShow();
        this->refTextEntry = textEntry;
    }

    // directory lister
    nGuiDirLister* dirLister = (nGuiDirLister*) kernelServer->New("nguidirlister", "DirLister");
    n_assert(dirLister);
    dirLister->SetDefaultBrush("list_background");
    dirLister->SetHighlightBrush("list_selection");
    dirLister->SetFont("GuiSmall");
    dirLister->SetIgnoreSubDirs(true);
    dirLister->SetIgnoreFiles(false);
    dirLister->SetSelectionEnabled(true);
    layout->AttachForm(dirLister, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(dirLister, nGuiFormLayout::Right, 0.005f);
    layout->AttachForm(dirLister, nGuiFormLayout::Left, 0.005f);
    if (this->GetSaveMode())
    {
        layout->AttachWidget(dirLister, nGuiFormLayout::Bottom, this->refTextEntry.get(), 0.005f);
    }
    else
    {
        layout->AttachWidget(dirLister, nGuiFormLayout::Bottom, okButton, 0.005f);
    }
    dirLister->OnShow();
    this->refDirLister = dirLister;

    kernelServer->PopCwd();

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiFileDialog::OnHide()
{
    if (this->refDirLister.isvalid())
    {
        this->refDirLister->Release();
        n_assert(!this->refDirLister.isvalid());
    }
    if (this->refTextEntry.isvalid())
    {
        this->refTextEntry->Release();
        n_assert(!this->refTextEntry.isvalid());
    }
    if (this->refOkButton.isvalid())
    {
        this->refOkButton->Release();
        n_assert(!this->refOkButton.isvalid());
    }
    if (this->refCancelButton.isvalid())
    {
        this->refCancelButton->Release();
        n_assert(!this->refCancelButton.isvalid());
    }

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiFileDialog::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (event.GetWidget() == this->refOkButton.get())
        {
            if (this->OnOk())
            {
                this->SetDismissed(true);
            }
        }
        else if (event.GetWidget() == this->refCancelButton.get())
        {
            if (this->OnCancel())
            {
                this->SetDismissed(true);
            }
        }
    }

    // call parent class
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Called when the Ok button is pressed. Overwrite this method in
    a subclass. Return true when the window should be closed,
    false if not.
*/
bool
nGuiFileDialog::OnOk()
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Called when the Cancel button is pressed. Overwrite this method in
    a subclass. Return true when the window should be closed,
    false if not.
*/
bool
nGuiFileDialog::OnCancel()
{
    return true;
}
