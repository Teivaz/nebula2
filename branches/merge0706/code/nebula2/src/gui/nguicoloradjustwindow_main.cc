//------------------------------------------------------------------------------
//  nguicoloradjustwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicoloradjustwindow.h"
#include "gui/nguicoloradjust.h"

nNebulaClass(nGuiColorAdjustWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiColorAdjustWindow::nGuiColorAdjustWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiColorAdjustWindow::~nGuiColorAdjustWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorAdjustWindow::OnShow()
{
    this->SetResizable(false);

    // call parent class
    nGuiClientWindow::OnShow();

    this->SetTitle("Color Adjustment");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // adjust Window to Display Resolution and init nGuiColorAdjust form
    vector2 displaySize = nGuiServer::Instance()->GetDisplaySize();
    float xFactor = 640/displaySize.x;
    float yFactor = 480/displaySize.y;
    const float border = 0.005f;

    nGuiColorAdjust* colorAdjust = (nGuiColorAdjust*) kernelServer->New("nguicoloradjust","ColorAdjust");
    layout->AttachForm(colorAdjust, nGuiFormLayout::Top, border * yFactor);
    layout->AttachForm(colorAdjust, nGuiFormLayout::Left, border * xFactor);
    layout->AttachForm(colorAdjust, nGuiFormLayout::Right, border * xFactor);
    layout->AttachForm(colorAdjust, nGuiFormLayout::Bottom, border * yFactor);
    colorAdjust->OnShow();
    this->refColorAdjust = colorAdjust;

    kernelServer->PopCwd();
    // set new window rect
    rectangle rect(vector2(0.0f, 0.0f), vector2(0.40f * xFactor, 0.65f * yFactor));
    this->SetRect(rect);

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorAdjustWindow::OnHide()
{
    this->refColorAdjust->Release();
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorAdjustWindow::OnEvent(const nGuiEvent& event)
{
    if (this->refColorAdjust.isvalid())
    {
        if ((event.GetWidget() == this->refColorAdjust) && (event.GetType() == nGuiEvent::SliderChanged))
        {
            nGuiEvent newEvent(this, nGuiEvent::SliderChanged);
            nGuiServer::Instance()->PutEvent(newEvent);
        }
    }
    nGuiClientWindow::OnEvent(event);
}

