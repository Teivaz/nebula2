//------------------------------------------------------------------------------
//  nguiwatcherwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiwatcherwindow.h"
#include "gui/nguitextlabel.h"
#include "gui/nguitextentry.h"
#include "gui/nguitextview.h"
#include "gui/nguiserver.h"
#include "kernel/nenv.h"

nNebulaClass(nGuiWatcherWindow, "gui::nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiWatcherWindow::nGuiWatcherWindow()
{
    this->envClass = kernelServer->FindClass("nenv");
    n_assert(0 != this->envClass);
}

//------------------------------------------------------------------------------
/**
*/
nGuiWatcherWindow::~nGuiWatcherWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiWatcherWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // set title
    this->SetTitle("Debug Watcher");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // create text label
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "TextLabel");
    n_assert(textLabel);
    textLabel->SetText("Pattern:");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Right);
    vector2 textSize = textLabel->GetTextExtent();
    vector2 textMinSize(0.0f, textSize.y * 1.25f);
    vector2 textMaxSize(1.0f, textSize.y * 1.25f);
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetMinSize(textMinSize);
    textLabel->SetMaxSize(textMaxSize);
    layout->AttachForm(textLabel, nGuiFormLayout::Top, 0.005f);
    layout->AttachForm(textLabel, nGuiFormLayout::Left, 0.005f);
    layout->AttachPos(textLabel, nGuiFormLayout::Right, 0.25f);
    this->refTextLabel = textLabel;

    // create text entry field
    nGuiTextEntry* textEntry = (nGuiTextEntry*) kernelServer->New("nguitextentry", "TextEntry");
    n_assert(textEntry);
    textEntry->SetText("*");
    textEntry->SetFont("GuiSmall");
    textEntry->SetAlignment(nGuiTextEntry::Left);
    textEntry->SetDefaultBrush("textentry_n");
    textEntry->SetPressedBrush("textentry_p");
    textEntry->SetHighlightBrush("textentry_h");
    textEntry->SetCursorBrush("textcursor");
    textEntry->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textEntry->SetMinSize(textMinSize);
    textEntry->SetMaxSize(textMaxSize);
    layout->AttachForm(textEntry, nGuiFormLayout::Top, 0.005f);
    layout->AttachWidget(textEntry, nGuiFormLayout::Left, textLabel, 0.005f);
    layout->AttachPos(textEntry, nGuiFormLayout::Right, 0.75f);
    this->refTextEntry = textEntry;

    // create text view field
    nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "TextView");
    n_assert(textView);
    layout->AttachWidget(textView, nGuiFormLayout::Top, textLabel, 0.005f);
    layout->AttachForm(textView, nGuiFormLayout::Left, 0.0f);
    layout->AttachForm(textView, nGuiFormLayout::Right, 0.0f);
    layout->AttachForm(textView, nGuiFormLayout::Bottom, 0.005f);
    this->refTextView = textView;

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
nGuiWatcherWindow::OnHide()
{
    // release child widgets
    if (this->refTextLabel.isvalid())
    {
        this->refTextLabel->Release();
    }
    if (this->refTextEntry.isvalid())
    {
        this->refTextEntry->Release();
    }
    if (this->refTextView.isvalid())
    {
        this->refTextView->Release();
    }

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
    The watcher display must be updated every frame.
*/
void
nGuiWatcherWindow::OnFrame()
{
    this->UpdateTextView();
    nGuiClientWindow::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Update the text view depending on the current content of the
    text entry widget.
*/
void
nGuiWatcherWindow::UpdateTextView()
{
    nGuiTextView* textView = this->refTextView.get();
    textView->BeginAppend();
    nRoot* watcherVars = kernelServer->Lookup("/sys/var");
    int numMatches = 0;
    if (watcherVars)
    {
        // get current wather pattern
        const char* watchPattern = this->refTextEntry->GetText();
        n_assert(watchPattern);

        // for each watcher variable...
        nEnv* curVar;
        for (curVar = (nEnv*) watcherVars->GetHead(); curVar; curVar = (nEnv*) curVar->GetSucc())
        {
            if (curVar->IsA(this->envClass))
            {
                const char* varName = curVar->GetName();
                if (n_strmatch(varName, watchPattern))
                {
                    numMatches++;
                    char line[512];
                    switch (curVar->GetType())
                    {
                        case nArg::Int:
                            sprintf(line, "%s:\t%d", varName, curVar->GetI());
                            break;

                        case nArg::Float:
                            sprintf(line, "%s:\t%f", varName, curVar->GetF());
                            break;

                        case nArg::String:
                            sprintf(line, "%s:\t%s", varName, curVar->GetS());
                            break;

                        case nArg::Bool:
                            sprintf(line, "%s:\t%s", varName, curVar->GetB() ? "true" : "false");
                            break;

                        case nArg::Float4:
                            {
                                const nFloat4& f4 = curVar->GetF4();
                                sprintf(line, "%s: %f %f %f %f\n", varName, f4.x, f4.y, f4.z, f4.w);
                            }
                            break;

                        default:
                            sprintf(line,"%s: <unknown data type>\n", varName);
                            break;
                    }
                    textView->AppendLine(line);
                }
            }
        }
    }
    if (0 == numMatches)
    {
        textView->AppendLine("no matches");
    }
    textView->EndAppend();
}
