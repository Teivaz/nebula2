//------------------------------------------------------------------------------
//  nguiconsolewindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiconsolewindow.h"
#include "gui/nguicmdentry.h"

nNebulaClass(nGuiConsoleWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiConsoleWindow::nGuiConsoleWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiConsoleWindow::~nGuiConsoleWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiConsoleWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    this->SetTitle("Shell");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // create own child widgets under the form layout
    nGuiCmdEntry* cmdEntry = (nGuiCmdEntry*) kernelServer->New("nguicmdentry", "CmdEntry");
    n_assert(cmdEntry);
    layout->AttachForm(cmdEntry, nGuiFormLayout::Left, 0.0f);
    layout->AttachForm(cmdEntry, nGuiFormLayout::Right, 0.0f);
    layout->AttachForm(cmdEntry, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(cmdEntry, nGuiFormLayout::Bottom, 0.005f);
    this->refCmdEntry = cmdEntry;

    // need to invoke on show manually on layout
    layout->OnShow();

    kernelServer->PopCwd();

    // update all layouts
    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiConsoleWindow::OnHide()
{
    // release child widgets
    if (this->refCmdEntry.isvalid())
    {
        this->refCmdEntry->Release();
    }

    // call parent class
    nGuiClientWindow::OnHide();
}
