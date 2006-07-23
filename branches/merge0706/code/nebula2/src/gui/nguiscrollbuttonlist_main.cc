//------------------------------------------------------------------------------
//  gui/nGuiScrollButtonList.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiscrollbuttonlist.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

nNebulaClass(nGuiScrollButtonList, "gui::nguicheckbuttongroup2");

//------------------------------------------------------------------------------
/**
    Constructor for the nGuiScrollButtonList object.
*/
nGuiScrollButtonList::nGuiScrollButtonList():
    horizontal(false),
    btnIndex(0),
    visibleButtons(5),
    scrollSize(1),
    allButtonsDeselected(false),
    scrollButtonsEnabled(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor for the nGuiScrollButtonList object.
*/
nGuiScrollButtonList::~nGuiScrollButtonList()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
nGuiScrollButtonList::OnShow()
{
    n_assert(this->options.Size() > 0);

    if(this->refCheckButtons.Size() > 0) this->refCheckButtons.Clear();

    nGuiFormLayout::OnShow();
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    // compute form layout size
    vector2 layoutSize;
    int numBtns = this->options.Size();

    vector2 btnSize;
    if (this->GetDefaultBrushes().Size() > 0)
    {
        btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->GetDefaultBrushes()[0].Get());
    }
    else
    {
        btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    }

    btnSize.x *= this->buttonScale.x;
    btnSize.y *= this->buttonScale.y;

    // compute forward button size
    vector2 fwdBtnSize;
    if (this->fwdButtonDefaultBrush.IsValid())
    {
        fwdBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->fwdButtonDefaultBrush.Get());
    }
    else
    {
        fwdBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    }

    if (this->horizontal)
    {
        layoutSize.x = btnSize.x * numBtns + (numBtns + 1) * this->GetButtonSpacing() + fwdBtnSize.x * 2;
        layoutSize.y = btnSize.y;
    }
    else
    {
        layoutSize.x = btnSize.x;
        layoutSize.y = btnSize.y * numBtns + (numBtns + 1) * this->GetButtonSpacing() + fwdBtnSize.y * 2;
    }
    this->SetMinSize(layoutSize);
    this->SetMaxSize(layoutSize);
    nGuiFormLayout::Edge edge = this->horizontal ? nGuiFormLayout::Left : nGuiFormLayout::Top;

    kernelServer->PushCwd(this);

    nGuiButton* scrollBtn = 0;
    if (!this->forwardbutton.isvalid() && this->scrollButtonsEnabled)
    {
        scrollBtn = (nGuiButton*) kernelServer->New("nguibutton", "fowardBtn");
        if (this->fwdButtonDefaultBrush.IsValid())
        {
            scrollBtn->SetDefaultBrush(this->fwdButtonDefaultBrush.Get());
        }
        else
        {
            scrollBtn->SetDefaultBrush("button_n");
        }
        if (this->fwdButtonPressedBrush.IsValid())
        {
            scrollBtn->SetPressedBrush(this->fwdButtonPressedBrush.Get());
        }
        else
        {
            scrollBtn->SetPressedBrush("button_p");
        }
        if (this->fwdButtonHighlightBrush.IsValid())
        {
            scrollBtn->SetHighlightBrush(fwdButtonHighlightBrush.Get());
        }
        else
        {
            scrollBtn->SetHighlightBrush("button_h");
        }

        scrollBtn->SetMinSize(fwdBtnSize);
        scrollBtn->SetMaxSize(fwdBtnSize);
        this->AttachForm(scrollBtn, nGuiFormLayout::Left, 0.0f);
        this->AttachForm(scrollBtn, nGuiFormLayout::Top, 0.0f);
        scrollBtn->OnShow();
        this->forwardbutton = scrollBtn;
    }

    kernelServer->PopCwd();
    UpdateVisibleListButtons();
    kernelServer->PushCwd(this);

    if (!this->backwardbutton.isvalid() && this->scrollButtonsEnabled)
    {
        scrollBtn = (nGuiButton*) kernelServer->New("nguibutton", "backwardBtn");
        if (this->bwdButtonDefaultBrush.IsValid())
        {
            scrollBtn->SetDefaultBrush(this->bwdButtonDefaultBrush.Get());
        }
        else
        {
            scrollBtn->SetDefaultBrush("button_n");
        }
        if (this->bwdButtonPressedBrush.IsValid())
        {
            scrollBtn->SetPressedBrush(this->bwdButtonPressedBrush.Get());
        }
        else
        {
            scrollBtn->SetPressedBrush("button_p");
        }
        if (this->bwdButtonHighlightBrush.IsValid())
        {
            scrollBtn->SetHighlightBrush(bwdButtonHighlightBrush.Get());
        }
        else
        {
            scrollBtn->SetHighlightBrush("button_h");
        }

        scrollBtn->SetMinSize(fwdBtnSize);
        scrollBtn->SetMaxSize(fwdBtnSize);
        this->AttachForm(scrollBtn, nGuiFormLayout::Right, 0.0f);
        this->AttachWidget(scrollBtn, edge, this->refCheckButtons.Back(), this->GetButtonSpacing());
        scrollBtn->OnShow();
        this->backwardbutton = scrollBtn;
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
nGuiScrollButtonList::UpdateVisibleListButtons()
{
    this->ClearAttachRules();
    kernelServer->PushCwd(this);

    /// first clean all check btns ans emoty btns
    for (int i = 0; i < this->refCheckButtons.Size(); i++)
    {
        this->refCheckButtons[i]->Release();
    }
    this->refCheckButtons.Clear();

    for (int i = 0; i < this->emptyBtns.Size(); i++)
    {
        this->emptyBtns[i]->Release();
    }
    this->emptyBtns.Clear();

    nGuiFormLayout::Edge edge = this->horizontal ? nGuiFormLayout::Left : nGuiFormLayout::Top;
    nGuiFormLayout::Edge sideEdge = this->horizontal ? nGuiFormLayout::Top : nGuiFormLayout::Left;

    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);
    // compute check button size
    vector2 btnSize;
    if (this->GetDefaultBrushes().Size() > 0)
    {
        btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->GetDefaultBrushes()[0].Get());
    }
    else
    {
        btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    }

    btnSize.x *= this->buttonScale.x;
    btnSize.y *= this->buttonScale.y;
    // initialize buttons
    int i;
    nGuiCheckButton* prevBtn = 0;
    int numVisBtns = this->btnIndex + n_min(this->visibleButtons, this->options.Size());
    for (i = this->btnIndex; i < numVisBtns; i++)
    {
        nString name = "Button";
        name.AppendInt(i);
        nGuiCheckButton* btn = (nGuiCheckButton*) kernelServer->New("nguicheckbutton", name.Get());
        if (this->GetDefaultBrushes().Size() > 0)
        {
            btn->SetDefaultBrush(this->GetDefaultBrushes()[i].Get());
        }
        else
        {
            btn->SetDefaultBrush("button_n");
        }
        if (this->GetPressedBrushes().Size() > 0)
        {
            btn->SetPressedBrush(this->GetPressedBrushes()[i].Get());
        }
        else
        {
            btn->SetPressedBrush("button_p");
        }
        if (this->GetHighlightBrushes().Size() > 0)
        {
            btn->SetHighlightBrush(this->GetHighlightBrushes()[i].Get());
        }
        else
        {
            btn->SetHighlightBrush("button_h");
        }

        if (this->tooltips.Size() > 0)
        {
            btn->SetTooltip(this->tooltips[i].Get());
        }

         if (this->btnTexts.Size() > 0)
        {
            btn->SetText(this->btnTexts[i].Get());
        }

        btn->SetMinSize(btnSize);
        btn->SetMaxSize(btnSize);
        //btn->SetText(this->options[i].Get());
        btn->SetFont(this->font.Get());
        btn->SetAlignment(nGuiTextLabel::Center);
        btn->SetColor(skin->GetButtonTextColor());
        float offset = this->btnOffsets.Size() > i ? this->btnOffsets[i] : 0;
        this->AttachForm(btn, sideEdge, offset);

        if (prevBtn)
        {
            this->AttachWidget(btn, edge, prevBtn, this->GetButtonSpacing());
        }
        else
        {
            if (this->forwardbutton.isvalid())
            {
                this->AttachWidget(btn, edge, this->forwardbutton, this->GetButtonSpacing());
            }
            else
            {
                this->AttachForm(btn, edge, this->GetButtonSpacing());
            }
        }
        btn->OnShow();
        this->refCheckButtons.Append(nRef<nGuiCheckButton>(btn));

        prevBtn = btn;
    }

    nGuiButton* prevEmptyBtn = 0;
    /// fill list with empty btns
    int numEmptyBtns = this->visibleButtons - this->refCheckButtons.Size();
    for (int i = 0; i < numEmptyBtns; i++)
    {
        nString name = "EmptyButton";
        name.AppendInt(i);
        nGuiButton* emptyBtn = (nGuiButton*) kernelServer->New("nguibutton", name.Get());
        if (this->emptyButtonBrush.IsValid())
        {
            emptyBtn->SetDefaultBrush(this->emptyButtonBrush.Get());
            emptyBtn->SetPressedBrush(this->emptyButtonBrush.Get());
            emptyBtn->SetHighlightBrush(this->emptyButtonBrush.Get());
        }
        else
        {
            emptyBtn->SetDefaultBrush("button_n");
            emptyBtn->SetPressedBrush("button_p");
            emptyBtn->SetHighlightBrush("button_h");
        }

        emptyBtn->SetMinSize(btnSize);
        emptyBtn->SetMaxSize(btnSize);

        float offset = this->btnOffsets.Size() > i ? this->btnOffsets[i] : 0;
        this->AttachForm(emptyBtn, sideEdge, offset);
        if (prevEmptyBtn)
        {
            this->AttachWidget(emptyBtn, edge, prevBtn, this->GetButtonSpacing());
        }
        else
        {
            this->AttachWidget(emptyBtn, edge, this->refCheckButtons.Back(), this->GetButtonSpacing());
        }
        emptyBtn->OnShow();
        this->emptyBtns.Append(emptyBtn);

        prevEmptyBtn = emptyBtn;
    }

    kernelServer->PopCwd();
    this->UpdateLayout(this->GetRect());
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiScrollButtonList::OnHide()
{
    // up to parent class
    nGuiCheckButtonGroup2::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiScrollButtonList::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::Action  && this->scrollButtonsEnabled)
    {
        bool replicateEvent = false;
        if (event.GetWidget() == this->forwardbutton)
        {
            replicateEvent = true;
            this->btnIndex = this->btnIndex + this->scrollSize;
            if (this->btnIndex >= this->options.Size() - this->visibleButtons) this->btnIndex = 0;
            UpdateVisibleListButtons();
        }
        else if (event.GetWidget() == this->backwardbutton)
        {
            replicateEvent = true;
            this->btnIndex = this->btnIndex - this->scrollSize;
            if (this->btnIndex < 0 )
            {
                if (this->visibleButtons < this->options.Size())
                {
                    this->btnIndex = this->options.Size() - this->visibleButtons  - 1;
                }
                else
                {
                    this->btnIndex = 0;
                }
            }

            UpdateVisibleListButtons();
        }


        if (replicateEvent)
        {
            // replicate event
            nGuiEvent event(this, nGuiEvent::Action);
            nGuiServer::Instance()->PutEvent(event);
        }
    }

    if (event.GetType() == nGuiEvent::Action)
    {
        this->allButtonsDeselected = false;
    }
    nGuiCheckButtonGroup2::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
*/
nGuiCheckButton*
nGuiScrollButtonList::GetSelectedListButton() const
{
    if(this->allButtonsDeselected)
    {
        return 0;
    }
    else
    {
        return this->refCheckButtons[this->selIndex].get();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiScrollButtonList::SetSelection(int index)
{
    this->allButtonsDeselected = false;
    this->selIndex = index;
    this->UpdateCheckButtons();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiScrollButtonList::UpdateCheckButtons()
{
    // reset all check buttons
    if (this->refCheckButtons.Size() > 0)
    {
        int i;
        int num = this->refCheckButtons.Size();
        for (i = 0; i < num; i++)
        {
            this->refCheckButtons[i]->SetIsPartOfGroup(true);
            this->refCheckButtons[i]->SetState(false);
        }

        if(!this->allButtonsDeselected)
        {
            // set active check button
            this->refCheckButtons[this->selIndex]->SetState(true);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
const nArray<nString>&
nGuiScrollButtonList::GetButtonTexts() const
{
    return this->btnTexts;
}