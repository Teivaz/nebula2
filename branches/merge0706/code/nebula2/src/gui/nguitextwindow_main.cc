//------------------------------------------------------------------------------
//  nguitextwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextwindow.h"
#include "gui/nguitextview.h"
#include "gui/nguitextbutton.h"
#include "kernel/nfileserver2.h"

nNebulaClass(nGuiTextWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiTextWindow::nGuiTextWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextWindow::~nGuiTextWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextWindow::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // call parent class
    nGuiClientWindow::OnShow();

    rectangle windowRect(vector2(0.15f, 0.15f), vector2(0.85f, 0.85f));
    this->SetRect(windowRect);

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");

    // close button
    nGuiTextButton* closeButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "CloseButton");
    n_assert(closeButton);
    closeButton->SetText("Close");
    closeButton->SetFont("GuiSmall");
    closeButton->SetAlignment(nGuiTextButton::Center);
    closeButton->SetDefaultBrush("button_n");
    closeButton->SetPressedBrush("button_p");
    closeButton->SetHighlightBrush("button_h");
    closeButton->SetMinSize(buttonSize);
    closeButton->SetMaxSize(buttonSize);
    closeButton->SetColor(skin->GetButtonTextColor());
    layout->AttachForm(closeButton, nGuiFormLayout::Right, 0.005f);
    layout->AttachForm(closeButton, nGuiFormLayout::Bottom, 0.005f);
    closeButton->OnShow();
    this->refCloseButton = closeButton;

    // text view
    nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "TextView");
    n_assert(textView);
    textView->SetDefaultBrush("list_background");
    textView->SetFont("GuiDefault");
    layout->AttachForm(textView, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(textView, nGuiFormLayout::Left, 0.005f);
    layout->AttachForm(textView, nGuiFormLayout::Right, 0.005f);
    layout->AttachWidget(textView, nGuiFormLayout::Bottom, closeButton, 0.005f);
    textView->OnShow();
    this->refTextView = textView;

    kernelServer->PopCwd();

    // read file into text view
    this->UpdateTextView();

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextWindow::OnHide()
{
    if (this->refTextView.isvalid())
    {
        this->refTextView->Release();
    }
    if (this->refCloseButton.isvalid())
    {
        this->refCloseButton->Release();
    }
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextWindow::OnEvent(const nGuiEvent& event)
{
    if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refCloseButton))
    {
        this->SetCloseRequested(true);
    }
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextWindow::UpdateTextView()
{
    char buf[1024];
    nGuiTextView* textView = this->refTextView;

    this->refTextView->BeginAppend();
    if (this->filename.IsEmpty())
    {
        textView->AppendLine("No filename!");
    }
    else
    {
        nFile* file = nFileServer2::Instance()->NewFileObject();
        if (file->Open(this->filename.Get(), "r"))
        {
            while (file->GetS(buf, sizeof(buf)))
            {
                textView->AppendLine(buf);
            }
            file->Close();
        }
        else
        {
            snprintf(buf, sizeof(buf), "Could not open text file '%s'!", this->filename.Get());
            textView->AppendLine(buf);
        }
        file->Release();
    }
    this->refTextView->EndAppend();
}
