//------------------------------------------------------------------------------
//  nguicheckbuttongroup2_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicheckbuttongroup2.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"
#include "gui/nguicheckbutton.h"

nNebulaClass(nGuiCheckButtonGroup2, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButtonGroup2::nGuiCheckButtonGroup2() :
    buttonSpacing(0.005f),
    selIndex(0)
{
    // empty
}
    
//------------------------------------------------------------------------------
/**
*/
nGuiCheckButtonGroup2::~nGuiCheckButtonGroup2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCheckButtonGroup2::OnShow()
{
    n_assert(this->options.Size() > 0);

    nGuiFormLayout::OnShow();
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // compute check button size
    vector2 btnSize;
    if (this->defaultBrushes.Size() > 0)
    {
        btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->defaultBrushes[0].Get());
    }
    else
    {
        btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    }
    
    // compute form layout size
    vector2 layoutSize;
    int numBtns = this->options.Size();
    layoutSize.x = btnSize.x * numBtns + (numBtns - 1) * this->buttonSpacing;
    layoutSize.y = btnSize.y;
    this->SetMinSize(layoutSize);
    this->SetMaxSize(layoutSize);

    kernelServer->PushCwd(this);

    // initialize buttons
    int i;
    nGuiCheckButton* prevBtn = 0;
    for (i = 0; i < numBtns; i++)
    {
        nString name = "Button";
        name.AppendInt(i);
        nGuiCheckButton* btn = (nGuiCheckButton*) kernelServer->New("nguicheckbutton", name.Get());
        if (this->defaultBrushes.Size() > 0)
        {
            btn->SetDefaultBrush(this->defaultBrushes[i].Get());
        }
        else
        {
            btn->SetDefaultBrush("button_n");
        }
        if (this->pressedBrushes.Size() > 0)
        {
            btn->SetPressedBrush(this->pressedBrushes[i].Get());
        }
        else
        {
            btn->SetPressedBrush("button_p");
        }
        if (this->highlightBrushes.Size() > 0)
        {
            btn->SetHighlightBrush(this->highlightBrushes[i].Get());
        }
        else
        {
            btn->SetHighlightBrush("button_h");
        }
        btn->SetMinSize(btnSize);
        btn->SetMaxSize(btnSize);
        btn->SetText(this->options[i].Get());
        btn->SetFont("GuiSmall");
        btn->SetAlignment(nGuiTextLabel::Center);
        btn->SetColor(skin->GetButtonTextColor());
        if (prevBtn)
        {
            this->AttachWidget(btn, Left, prevBtn, this->buttonSpacing);
        }
        else
        {
            this->AttachForm(btn, Left, 0.0f);
        }
        this->AttachForm(btn, Top, 0.0f);
        btn->OnShow();
        this->refCheckButtons.Append(nRef<nGuiCheckButton>(btn));

        prevBtn = btn;
    }
    kernelServer->PopCwd();

    // update the check button state
    this->UpdateCheckButtons();

    // register as event listener
    nGuiServer::Instance()->RegisterEventListener(this);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCheckButtonGroup2::OnHide()
{
    // unregister as event listener
    nGuiServer::Instance()->UnregisterEventListener(this);

    // clear layout rules
    this->ClearAttachRules();

    // up to parent class
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCheckButtonGroup2::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::Action)
    {
        int i;
        int num = this->refCheckButtons.Size();
        for (i = 0; i < num; i++)
        {
            if (this->refCheckButtons[i].isvalid())
            {
                if (event.GetWidget() == this->refCheckButtons[i].get())
                {
                    this->selIndex = i;
                    this->UpdateCheckButtons();
                }
            }
        }
    }
    nGuiFormLayout::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCheckButtonGroup2::UpdateCheckButtons()
{
    // reset all check buttons
    if (this->refCheckButtons.Size() > 0)
    {
        int i;
        int num = this->refCheckButtons.Size();
        for (i = 0; i < num; i++)
        {
            this->refCheckButtons[i]->SetState(false);
        }

        // set active check button
        this->refCheckButtons[this->selIndex]->SetState(true);
    }
}





