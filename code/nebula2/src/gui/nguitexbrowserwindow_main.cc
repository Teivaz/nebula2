//------------------------------------------------------------------------------
//  nguitexbrowserwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitexbrowserwindow.h"
#include "gui/nguitextview.h"
#include "gui/nguibutton.h"
#include "gui/nguievent.h"
#include "gui/nguitextureview.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiTexBrowserWindow, "nguiclientwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiTexBrowserWindow::nGuiTexBrowserWindow() :
    refTextureRoot("/sys/share/rsrc/tex")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTexBrowserWindow::~nGuiTexBrowserWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTexBrowserWindow::OnShow()
{
    // call parent class
    nGuiClientWindow::OnShow();

    // set the window title
    this->SetTitle("Texture Browser");

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // "prev texture" button
    nGuiButton* prevBtn = (nGuiButton*) kernelServer->New("nguibutton", "Prev");
    n_assert(prevBtn);
    vector2 prevBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("arrowleft_n");
    prevBtn->SetDefaultBrush("arrowleft_n");
    prevBtn->SetPressedBrush("arrowleft_p");
    prevBtn->SetHighlightBrush("arrowleft_h");
    prevBtn->SetMinSize(prevBtnSize);
    prevBtn->SetMaxSize(prevBtnSize);
    layout->AttachForm(prevBtn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(prevBtn, nGuiFormLayout::Left, 0.0f);
    prevBtn->OnShow();
    this->refPrevButton = prevBtn;

    // "next texture" button
    nGuiButton* nextBtn = (nGuiButton*) kernelServer->New("nguibutton", "Next");
    n_assert(nextBtn);
    vector2 nextBtnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("arrowright_n");
    nextBtn->SetDefaultBrush("arrowright_n");
    nextBtn->SetPressedBrush("arrowright_p");
    nextBtn->SetHighlightBrush("arrowright_h");
    nextBtn->SetMinSize(nextBtnSize);
    nextBtn->SetMaxSize(nextBtnSize);
    layout->AttachForm(nextBtn, nGuiFormLayout::Top, 0.0f);
    layout->AttachWidget(nextBtn, nGuiFormLayout::Left, prevBtn, 0.0f);
    nextBtn->OnShow();
    this->refNextButton = nextBtn;

    // info field
    nGuiTextView* infoField = (nGuiTextView*) kernelServer->New("nguitextview", "InfoField");
    n_assert(infoField);
    layout->AttachWidget(infoField, nGuiFormLayout::Top, prevBtn, 0.005f);
    layout->AttachForm(infoField, nGuiFormLayout::Left, 0.0f);
    layout->AttachForm(infoField, nGuiFormLayout::Right, 0.0f);
    layout->AttachPos(infoField, nGuiFormLayout::Bottom, 0.25f);
    infoField->OnShow();
    this->refInfoField = infoField;

    // texture view
    nGuiTextureView* texView = (nGuiTextureView*) kernelServer->New("nguitextureview", "TexView");
    n_assert(texView);
    texView->SetDefaultBrush("pink");
    layout->AttachWidget(texView, nGuiFormLayout::Top, infoField, 0.005f);
    layout->AttachForm(texView, nGuiFormLayout::Left, 0.005f);
    layout->AttachForm(texView, nGuiFormLayout::Right, 0.005f);
    layout->AttachForm(texView, nGuiFormLayout::Bottom, 0.005f);
    texView->OnShow();
    this->refTexView = texView;

    kernelServer->PopCwd();

    // set new window rect
    rectangle rect(vector2(0.0f, 0.0f), vector2(0.4f, 0.6f));
    this->SetRect(rect);

    // update all layouts
    this->UpdateLayout(this->rect);

    // set to first texture
    this->SetNextTexture();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTexBrowserWindow::OnHide()
{
    if (this->refPrevButton.isvalid())
    {
        this->refPrevButton->Release();
    }
    if (this->refNextButton.isvalid())
    {
        this->refNextButton->Release();
    }
    if (this->refInfoField.isvalid())
    {
        this->refInfoField->Release();
    }
    if (this->refTexView.isvalid())
    {
        this->refTexView->Release();
    }

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiTexBrowserWindow::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (event.GetWidget() == this->refPrevButton.get())
        {
            this->SetPrevTexture();
        }
        else if (event.GetWidget() == this->refNextButton.get())
        {
            this->SetNextTexture();
        }
    }

    // call parent class
    nGuiClientWindow::OnEvent(event);
}

//------------------------------------------------------------------------------
/**
    Set internal texture pointer to next texture.
*/
void
nGuiTexBrowserWindow::SetNextTexture()
{
    nTexture2* tex = 0;
    if (this->refCurrentTexture.isvalid())
    {
        tex = (nTexture2*) this->refCurrentTexture->GetSucc();
        // if there is no next texture, we just stop at the last texture
        // (we don't wrap around)
    }
    else if (0 == tex)
    {
        // exception: rewind to first texture
        tex = (nTexture2*) this->refTextureRoot->GetHead();
    }
    if (tex)
    {
        this->refCurrentTexture = tex;

        // update texture dependend stuff
        this->UpdateTitle();
        this->UpdateInfoField();
        this->UpdateTextureView();
    }
}

//------------------------------------------------------------------------------
/**
    Set internal texture pointer to preview texture.
*/
void
nGuiTexBrowserWindow::SetPrevTexture()
{
    nTexture2* tex = 0;
    if (this->refCurrentTexture.isvalid())
    {
        tex = (nTexture2*) this->refCurrentTexture->GetPred();
        // if there is no previous texture, we just stop at the first texture
        // (we don't wrap around)
    }
    else if (0 == tex)
    {
        // exception: rewind to first texture
        tex = (nTexture2*) this->refTextureRoot->GetHead();
    }
    if (tex)
    {
        this->refCurrentTexture = tex;

        // update texture dependend stuff
        this->UpdateTitle();
        this->UpdateInfoField();
        this->UpdateTextureView();
    }
}

//------------------------------------------------------------------------------
/**
    Update the window title with the number of texture.
*/
void
nGuiTexBrowserWindow::UpdateTitle()
{
    // count loaded textures
    nTexture2* curTexture = this->refCurrentTexture.isvalid() ? this->refCurrentTexture.get() : 0;
    int numTextures = 0;
    int curTexturePosition = 0;
    nRoot* child;
    for (child = this->refTextureRoot->GetHead(); child; child = child->GetSucc())
    {
        numTextures++;
        if (curTexture == child)
        {
            curTexturePosition = numTextures;
        }
    }
    char title[128];
    sprintf(title, "Texture Browser (%d / %d)", curTexturePosition, numTextures);
    this->SetTitle(title);
}

//------------------------------------------------------------------------------
/**
    Update the info field with some useful information about the current
    texture.
*/
void
nGuiTexBrowserWindow::UpdateInfoField()
{
    if (!this->refInfoField.isvalid())
    {
        return;
    }
    nGuiTextView* infoField = this->refInfoField.get();
    infoField->BeginAppend();
    if (this->refCurrentTexture.isvalid())
    {
        nString str;
        nTexture2* tex = this->refCurrentTexture.get();

        // filename
        nString filename = tex->GetFilename();
        if (filename.IsEmpty())
        {
            // no filename, use the resource name instead
            str = "Name:\t\t";
            str.Append(tex->GetName());
        }
        else
        {
            // filename given, use filename
            str = "Filename:\t";
            str.Append(filename.Get());
        }
        infoField->AppendLine(str.Get());

        // usage flags
        int usage = tex->GetUsage();
        bool isRenderTarget = tex->IsRenderTarget();
        str = "Flags:\t\t";
        bool noFlags = true;
        if (isRenderTarget)
        {
            str.Append("RenderTarget ");
            noFlags = false;
        }
        if (usage & nTexture2::Dynamic)
        {
            str.Append("Dynamic ");
            noFlags = false;
        }
        if (usage & (nTexture2::CreateFromRawCompoundFile | nTexture2::CreateFromDDSCompoundFile))
        {
            str.Append("FromCompoundFile ");
            noFlags = false;
        }
        if (noFlags)
        {
            str.Append("none");
        }
        infoField->AppendLine(str.Get());

        // type
        nTexture2::Type type = tex->GetType();
        str = "Type:\t\t";
        switch (type)
        {
            case nTexture2::TEXTURE_NOTYPE: str.Append("NoType (Error!)"); break;
            case nTexture2::TEXTURE_2D:     str.Append("2D"); break;
            case nTexture2::TEXTURE_3D:     str.Append("3D"); break;
            case nTexture2::TEXTURE_CUBE:   str.Append("Cube"); break;
            default:             str.Append("Unknown???"); break;
        }
        infoField->AppendLine(str.Get());

        // format
        nTexture2::Format format = tex->GetFormat();
        str = "Format:\t\t";
        switch (format)
        {
            case nTexture2::NOFORMAT:      str.Append("NoFormat (Error!)"); break;
            case nTexture2::X8R8G8B8:      str.Append("X8R8G8B8 (32-bit, no alpha)"); break;
            case nTexture2::A8R8G8B8:      str.Append("A8R8G8B8 (32-bit, 8 bit alpha)"); break;
            case nTexture2::R5G6B5:        str.Append("R5G6B5 (16-bit, no alpha)"); break;
            case nTexture2::A1R5G5B5:      str.Append("A1R5G5B5 (16-bit, 1 bit alpha)"); break;
            case nTexture2::A4R4G4B4:      str.Append("A4R4G4B4 (16-bit, 4 bit alpha)"); break;
            case nTexture2::P8:            str.Append("P8 palettized)"); break;
            case nTexture2::G16R16:        str.Append("G16R16 (two 16-bit integer channels)"); break;
            case nTexture2::DXT1:          str.Append("DXT1 (compressed w/o alpha)"); break;
            case nTexture2::DXT2:          str.Append("DXT2 (not supported)"); break;
            case nTexture2::DXT3:          str.Append("DXT3 (compressed with explicit alpha)"); break;
            case nTexture2::DXT4:          str.Append("DXT4 (not supported)"); break;
            case nTexture2::DXT5:          str.Append("DXT5 (compressed with interpolated alpha)"); break;
            case nTexture2::R16F:          str.Append("R16F (one 16-bit float channel)"); break;
            case nTexture2::G16R16F:       str.Append("G16R16F (two 16-bit float channels)"); break;
            case nTexture2::A16B16G16R16F: str.Append("A16B16G16R16F (four 16-bit float channels)"); break;
            case nTexture2::R32F:          str.Append("R32F (one 32-bit float channel)"); break;
            case nTexture2::G32R32F:       str.Append("G32R32F (two 32-bit float channels)"); break;
            case nTexture2::A32B32G32R32F: str.Append("A32B32G32R32F (four 32-bit float channels)"); break;
            default:                       str.Append("Unknown!");
        }
        infoField->AppendLine(str.Get());

        // dimension, number of mip levels
        int width = tex->GetWidth();
        int height = tex->GetHeight();
        int numMipLevels = tex->GetNumMipLevels();
        str = "Dimension:\t";
        str.AppendInt(width);
        str.Append(" x ");
        str.AppendInt(height);
        str.Append(", ");
        str.AppendInt(numMipLevels);
        str.Append(" mip levels");
        infoField->AppendLine(str.Get());

        // memory size
        char buf[128];
        float sizeInKbyte = float(tex->GetByteSize()) / 1000.0f;
        sprintf(buf, "Memory Size:\t%.1f kByte", sizeInKbyte);
        infoField->AppendLine(buf);
    }
    else
    {
        // this should never happen
        infoField->AppendLine("No Texture!");
    }
    infoField->EndAppend();
}

//------------------------------------------------------------------------------
/**
    Update the texture view label.
*/
void
nGuiTexBrowserWindow::UpdateTextureView()
{
    if (this->refCurrentTexture.isvalid())
    {
        this->refTexView->SetTexture(this->refCurrentTexture.get());
    }
    else
    {
        this->refTexView->SetTexture(0);
    }
}
