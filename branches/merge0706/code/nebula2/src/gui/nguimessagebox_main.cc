//------------------------------------------------------------------------------
//  nguimessagebox_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguimessagebox.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"
#include "gui/nguibutton.h"

nNebulaClass(nGuiMessageBox, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiMessageBox::nGuiMessageBox() :
    okText("Ok"),
    cancelText("Cancel"),
    type(Ok),
    autoSize(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiMessageBox::~nGuiMessageBox()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiMessageBox::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    this->SetMovable(true);
    this->SetResizable(false);
    this->SetCloseButton(false);
    if (0 == this->GetDefaultBrush())
    {
        this->SetDefaultBrush("bgmessagebox");
    }

    nGuiClientWindow::OnShow();

    // get client area layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // message text field
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "TextLabel");
    n_assert(textLabel);
    textLabel->SetText(this->GetMessageText());
    textLabel->SetFont(skin->GetWindowFont());
    textLabel->SetColor(skin->GetTextColor());
    textLabel->SetClipping(false);
    textLabel->SetAlignment(nGuiTextLabel::Center);
    textLabel->SetWordBreak(true);
    textLabel->SetVCenter(true);
    //textfield layout
    layout->AttachForm(textLabel, nGuiFormLayout::Top, 0.0f);

    if (!this->iconBrush.IsEmpty())
    {
        // add optional icon
        vector2 iconSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->iconBrush.Get());

        nGuiButton* iconButton = (nGuiButton*) kernelServer->New("nguibutton", "iconbutton");
        n_assert(iconButton);

        iconButton->SetDisabledBrush(this->iconBrush.Get());

        iconButton->SetMinSize(iconSize);
        iconButton->SetMaxSize(iconSize);
        iconButton->Disable();  // use it as a icon - so disable it.
        iconButton->OnShow();

        this->refIconButton = iconButton;

        layout->AttachForm(iconButton, nGuiFormLayout::Left, -0.007f);
        layout->AttachForm(iconButton, nGuiFormLayout::Top, -0.01f);
        //textfield layout
        layout->AttachWidget(textLabel, nGuiFormLayout::Left, iconButton, 0.0f);
    }
    else
    {
        //textfield layout
        layout->AttachForm(textLabel, nGuiFormLayout::Left, 0.0f);
    }

    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    // create ok button

    if (this->type != NoButtons)
    {
        if ((OkCancel == this->type) || (Ok == this->type))
        {
            nGuiTextButton* okButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "OkButton");
            n_assert(okButton);
            okButton->SetText(this->GetOkText());
            okButton->SetFont(skin->GetButtonFont());
            okButton->SetAlignment(nGuiTextButton::Center);
            okButton->SetDefaultBrush("button_n");
            okButton->SetPressedBrush("button_p");
            okButton->SetHighlightBrush("button_h");
            okButton->SetMinSize(buttonSize);
            okButton->SetMaxSize(buttonSize);
            okButton->SetColor(skin->GetButtonTextColor());
            if (this->iconBrush.IsEmpty())
            {
                if (Ok == this->type)
                {
                    layout->AttachPos(okButton, nGuiFormLayout::HCenter, 0.5f);
                }
                else
                {
                    layout->AttachForm(okButton, nGuiFormLayout::Left, 0.005f);
                }
            }
            else
            {
                layout->AttachWidget(okButton, nGuiFormLayout::Left, this->refIconButton, 0.005f);
            }

            layout->AttachForm(okButton, nGuiFormLayout::Bottom, 0.005f);
            okButton->OnShow();
            this->refOkButton = okButton;

            //textfield layout
            layout->AttachForm(textLabel, nGuiFormLayout::Right, 0.005f);
            layout->AttachWidget(textLabel, nGuiFormLayout::Bottom, okButton, 0.005f);
        }

        // create cancel button
        if (OkCancel == this->type)
        {
            nGuiTextButton* cancelButton = (nGuiTextButton*) kernelServer->New("nguitextbutton", "CancelButton");
            n_assert(cancelButton);
            cancelButton->SetText(this->GetCancelText());
            cancelButton->SetFont(skin->GetButtonFont());
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
        }
    }
    else
    {
        //textfield layout
        layout->AttachForm(textLabel, nGuiFormLayout::Right, 0.005f);
        layout->AttachForm(textLabel, nGuiFormLayout::Bottom, 0.005f);
    }

    textLabel->OnShow();
    this->refTextLabel = textLabel;

    kernelServer->PopCwd();

    // compute our size
    vector2 windowSize;
    windowSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->GetDefaultBrush());
    if (this->autoSize)
    {
        vector2 msgSize = textLabel->GetTextExtent() * 1.2f;
        if (msgSize.x > windowSize.x)
        {
            windowSize.x = msgSize.x;
        }
        if (msgSize.y > windowSize.y)
        {
            windowSize.y = msgSize.y;
        }
    }
    vector2 windowPos = vector2(0.5f, 0.5f) - (windowSize * 0.5f);
    rectangle windowRect(windowPos, windowPos + windowSize);
    this->SetMinSize(windowSize);
    this->SetMaxSize(windowSize);
    this->SetRect(windowRect);

    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiMessageBox::OnHide()
{
    if (this->refTextLabel.isvalid())
    {
        this->refTextLabel->Release();
        n_assert(!this->refTextLabel.isvalid());
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
    if (this->refIconButton.isvalid())
    {
        this->refIconButton->Release();
        n_assert(!this->refIconButton.isvalid());
    }
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiMessageBox::OnEvent(const nGuiEvent& event)
{
    // check for Ok
    if (this->refOkButton.isvalid())
    {
        if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refOkButton.get()))
        {
            this->SetCloseRequested(true);
            this->OnOk();
            nGuiServer::Instance()->PutEvent(nGuiEvent(this, nGuiEvent::DialogOk));
        }
    }
    if (this->refCancelButton.isvalid())
    {
        if ((event.GetType() == nGuiEvent::ButtonUp) && (event.GetWidget() == this->refCancelButton.get()))
        {
            this->SetCloseRequested(true);
            this->OnCancel();
            nGuiServer::Instance()->PutEvent(nGuiEvent(this, nGuiEvent::DialogCancel));
        }
    }
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Called when the Ok button is pressed. Optionally override this
    method in a subclass.
*/
void
nGuiMessageBox::OnOk()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called when the Cancel button is pressed. Optionally override this
    method in a subclass.
*/
void
nGuiMessageBox::OnCancel()
{
    // empty
}


