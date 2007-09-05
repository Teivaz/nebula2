//------------------------------------------------------------------------------
//  nguicontextmenu_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguievent.h"
#include "gui/nguiserver.h"
#include "gui/nguitextbutton.h"
#include "gui/nguiskin.h"
#include "gui/nguicontextmenu.h"

nNebulaScriptClass(nGuiContextMenu, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiContextMenu::nGuiContextMenu() :
    selectedIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiContextMenu::~nGuiContextMenu()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Attaches the text buttons and sizes to the proper size
*/
void
nGuiContextMenu::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    /// reset selection index
    this->selectedIndex = -1;

    // set window attributes
    this->SetTitleBar(false);
    this->SetCloseButton(false);
    this->SetResizable(false);
    this->SetMovable(false);
    this->SetModal(true);
    this->SetDefaultBrush(0);

    // call parent class
    nGuiClientWindow::OnShow();

    // get client area layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // create text buttons
    vector2 btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("menu_n");
    float maxWidth = 0.0f;
    nGuiTextButton* prevBtn = 0;
    int i = 0;
    int num = this->entryArray.Size();
    for (i = 0; i < num; i++)
    {
        char uniqueObjName[N_MAXPATH];
        snprintf(uniqueObjName, sizeof(uniqueObjName), "Entry%d", i);
        nGuiTextButton* btn = (nGuiTextButton*) kernelServer->New("nguitextbutton", uniqueObjName);
        n_assert(btn);
        btn->SetText(this->entryArray[i].Get());
        btn->SetColor(skin->GetMenuTextColor());
        btn->SetFont("GuiSmall");
        btn->SetAlignment(nGuiTextButton::Left);
        btn->SetDefaultBrush("menu_n");
        btn->SetPressedBrush("menu_h");
        btn->SetHighlightBrush("menu_h");

        layout->AttachForm(btn, nGuiFormLayout::Left, 0.0f);
        layout->AttachForm(btn, nGuiFormLayout::Right, 0.0f);
        if (0 == prevBtn)
        {
            // first button
            layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
        }
        else
        {
            layout->AttachWidget(btn, nGuiFormLayout::Top, prevBtn, 0.0f);
        }
        this->buttonArray.Append(btn);
        btn->OnShow();
        prevBtn = btn;

        // computes widest button
        vector2 textExtent = btn->GetTextExtent();
        if (textExtent.x > maxWidth)
        {
            maxWidth = textExtent.x;
        }
    }
    kernelServer->PopCwd();

    // compute window size and position window under mouse
    btnSize.x = maxWidth + 0.01f;
    for (i = 0; i < num; i++)
    {
        this->buttonArray[i]->SetMinSize(btnSize);
        this->buttonArray[i]->SetMaxSize(btnSize);
    }

    const vector2& mousePos = nGuiServer::Instance()->GetMousePos();
    vector2 windowPos = mousePos + nGuiServer::Instance()->GetPixelSize();
    vector2 size(btnSize.x, this->buttonArray.Size() * btnSize.y);
    rectangle rect(windowPos, windowPos + size);
    nGuiServer::Instance()->MoveRectToVisibleArea(rect);
    this->SetRect(rect);
    this->UpdateLayout(rect);
}

//------------------------------------------------------------------------------
/**
    Releases all buttons
*/
void
nGuiContextMenu::OnHide()
{
    int i;
    int num = this->buttonArray.Size();
    for (i = 0; i < num; i++)
    {
        this->buttonArray[i]->Release();
    }
    this->buttonArray.Clear();

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiContextMenu::OnEvent(const nGuiEvent& event)
{
    // handle button ups
    if (event.GetType() == nGuiEvent::RButtonUp)
    {
        const vector2& mousePos = nGuiServer::Instance()->GetMousePos();

        // generate MenuEntrySelected event
        int i;
        int num = this->buttonArray.Size();
        for (i = 0; i < num; i++)
        {
            if ((event.GetWidget() == this->buttonArray[i]) && (this->buttonArray[i]->Inside(mousePos)))
            {
                this->selectedIndex = i;
                nGuiEvent newEvent(this, nGuiEvent::MenuEntrySelected);
                nGuiServer::Instance()->PutEvent(newEvent);
                break;
            }
        }
        this->SetCloseRequested(true);
    }
    nGuiClientWindow::OnEvent(event);
}

