//------------------------------------------------------------------------------
//  nguisysteminfowindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguisysteminfowindow.h"
#include "gui/nguitextview.h"
#include "kernel/nfileserver2.h"
#include "resource/nresourceserver.h"
#include "kernel/ntimeserver.h"

nNebulaClass(nGuiSystemInfoWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiSystemInfoWindow::nGuiSystemInfoWindow() :
    refResourceServer("/sys/servers/resource")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiSystemInfoWindow::~nGuiSystemInfoWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSystemInfoWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // set title
    this->SetTitle("System Information");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // create text view field
    nGuiTextView* textView = (nGuiTextView*) kernelServer->New("nguitextview", "TextView");
    n_assert(textView);
    layout->AttachForm(textView, nGuiFormLayout::Top, 0.005f);
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
nGuiSystemInfoWindow::OnHide()
{
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
nGuiSystemInfoWindow::OnFrame()
{
    this->UpdateTextView();
    nGuiClientWindow::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Update the text view.
*/
void
nGuiSystemInfoWindow::UpdateTextView()
{
    nResourceServer* resServer = this->refResourceServer.get();
    nString str;
    nGuiTextView* textView = this->refTextView.get();

    textView->BeginAppend();

    // home assign
    str = "Home Directory:\t\t";
    str.Append(nFileServer2::Instance()->ManglePath("home:"));
    textView->AppendLine(str.Get());

    // user assign
    str = "User Directory:\t\t";
    str.Append(nFileServer2::Instance()->ManglePath("user:"));
    textView->AppendLine(str.Get());

    // proj assign
    str = "Project Directory:\t";
    str.Append(nFileServer2::Instance()->ManglePath("proj:"));
    textView->AppendLine(str.Get());

    // texture assign
    str = "Texture Directory:\t";
    str.Append(nFileServer2::Instance()->ManglePath("textures:"));
    textView->AppendLine(str.Get());

    // mesh assign
    str = "Mesh Directory:\t\t";
    str.Append(nFileServer2::Instance()->ManglePath("meshes:"));
    textView->AppendLine(str.Get());

    // shader assign
    str = "Shader Directory:\t";
    str.Append(nFileServer2::Instance()->ManglePath("shaders:"));
    textView->AppendLine(str.Get());

    // time
    char buf[128];
    snprintf(buf, sizeof(buf), "Nebula Time:\t\t%.4f", kernelServer->GetTimeServer()->GetTime());
    textView->AppendLine(buf);

    // resource numbers
    const float mega = 1024 * 1024;
    sprintf(buf, "Number of Meshes:\t%d (%.3f MBytes)",
        resServer->GetNumResources(nResource::Mesh),
        float(resServer->GetResourceByteSize(nResource::Mesh)) / mega);
    textView->AppendLine(buf);

    sprintf(buf, "Number of Textures:\t%d (%.3f MBytes)",
        resServer->GetNumResources(nResource::Texture),
        float(resServer->GetResourceByteSize(nResource::Texture)) / mega);
    textView->AppendLine(buf);

    sprintf(buf, "Number of Shaders:\t%d", resServer->GetNumResources(nResource::Shader));
    textView->AppendLine(buf);

    sprintf(buf, "Number of Animations:\t%d (%.3f MBytes)",
        resServer->GetNumResources(nResource::Animation),
        float(resServer->GetResourceByteSize(nResource::Animation)) / mega);
    textView->AppendLine(buf);

    sprintf(buf, "Number of Fonts:\t%d", resServer->GetNumResources(nResource::Font));
    textView->AppendLine(buf);

    sprintf(buf, "Num Sound Resources:\t%d (%.3f MBytes)",
        resServer->GetNumResources(nResource::SoundResource),
        float(resServer->GetResourceByteSize(nResource::SoundResource)) / mega);
    textView->AppendLine(buf);

    sprintf(buf, "Num Sound Instances:\t%d", resServer->GetNumResources(nResource::SoundInstance));
    textView->AppendLine(buf);

    textView->EndAppend();
}
