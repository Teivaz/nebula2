//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxtexturefilectrl.h"
#include "lwwrapper/nlwglobals.h"
#include "kernel/nfileserver2.h"

static nString ConvertPathToAbsolute(const nString& inPath);
static nString ConvertPathToRelative(const nString& inPath);

//----------------------------------------------------------------------------
/**
    @class wxTextureFileCtrl::TextCtrlEvtHandler
    @brief Custom event handler for the wxTextCtrl sub-control.
*/
class wxTextureFileCtrl::TextCtrlEvtHandler : public wxEvtHandler
{
public:
    TextCtrlEvtHandler(wxTextureFileCtrl* parent) :
        parent(parent)
    {
        n_assert(parent);
    }

    virtual bool ProcessEvent(wxEvent& event)
    {
        if (event.GetEventType() == wxEVT_KILL_FOCUS)
        {
            if (this->parent->textCtrl)
            {
                nString curPath(this->parent->textCtrl->GetValue().c_str());
                nString relPath(ConvertPathToRelative(curPath));
                this->parent->textCtrl->SetValue(relPath.Get());
            }
        }
        event.Skip();
        return false;
      }

private:
    wxTextureFileCtrl* parent;
};

//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxTextureFileCtrl, wxPanel)

enum
{
    ID_FILE_BTN = wxID_HIGHEST + 1,
};

BEGIN_EVENT_TABLE(wxTextureFileCtrl, wxPanel)
    EVT_BUTTON(ID_FILE_BTN, wxTextureFileCtrl::OnFileBtn)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
/**
*/
wxTextureFileCtrl::wxTextureFileCtrl() :
    textCtrl(0),
    btnCtrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxTextureFileCtrl::wxTextureFileCtrl(wxWindow* parent, wxWindowID id,
                                     const wxPoint& pos, const wxSize& size) :
    textCtrl(0),
    btnCtrl(0)
{
    this->Create(parent, id, pos, size);
}

//----------------------------------------------------------------------------
/**
*/
bool
wxTextureFileCtrl::Create(wxWindow* parent, wxWindowID id,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name)
{
    if (wxPanel::Create(parent, id, pos, size, style, name))
    {
        this->textCtrl = new wxTextCtrl(this, wxID_ANY);
        this->textCtrl->PushEventHandler(new TextCtrlEvtHandler(this));
        this->btnCtrl = new wxButton(this, ID_FILE_BTN, "...",
                                     wxDefaultPosition, wxSize(20,20));

        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        if (topSizer)
        {
            topSizer->Add(this->textCtrl, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
            topSizer->Add(this->btnCtrl, 0, wxALIGN_CENTER_VERTICAL);
            this->bestSize = topSizer->Fit(this);
            this->SetSizer(topSizer);
            // work around for a bug in wxWidgets sizing, if I don't do
            // this the sizing goes nuts and all the sub-controls
            // end up on top of each other
            this->bestSize.SetWidth(this->bestSize.GetWidth() + 1);
        }

        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
wxTextureFileCtrl::~wxTextureFileCtrl()
{
    // have to pop and delete the custom event handler we created and pushed
    // back in Create() otherwise bad things can happen
    if (this->textCtrl)
        this->textCtrl->PopEventHandler(true);

    this->textCtrl = 0;
    this->btnCtrl = 0;
}

//----------------------------------------------------------------------------
/**
*/
wxSize
wxTextureFileCtrl::DoGetBestSize() const
{
    return this->bestSize;
}

//----------------------------------------------------------------------------
/**
*/
static
nString
ConvertPathToAbsolute(const nString& inPath)
{
    // if the path starts with "textures:" we can just mangle it
    nString texturesAssign("textures:");
    if ((inPath.Length() > texturesAssign.Length()) &&
        strncmp(inPath.Get(), texturesAssign.Get(), texturesAssign.Length()) == 0)
    {
        return nFileServer2::Instance()->ManglePath(inPath);
    }

    // if the path doesn't start with "textures:" we assume it must be
    // relative to Lightwave Images dir
    nLWGlobals::DirInfoFunc dirInfo;
    nString lwImagesDir(dirInfo.GetImagesDir());
    lwImagesDir.ToLower();
    lwImagesDir.ConvertBackslashes();
    lwImagesDir.StripTrailingSlash();
    lwImagesDir += "/";
    lwImagesDir += inPath;
    return lwImagesDir;
}

//----------------------------------------------------------------------------
/**
*/
static
nString
ConvertPathToRelative(const nString& inPath)
{
    if (inPath.IsEmpty())
        return inPath;

    nString cleanPath(inPath);
    cleanPath.ToLower();
    cleanPath.ConvertBackslashes();

    nLWGlobals::DirInfoFunc dirInfo;
    nString lwImagesDir(dirInfo.GetImagesDir());
    lwImagesDir.ToLower();
    lwImagesDir.ConvertBackslashes();
    lwImagesDir.StripTrailingSlash();
    lwImagesDir += "/";

    if ((cleanPath.Length() > lwImagesDir.Length()) &&
        strncmp(cleanPath.Get(), lwImagesDir.Get(), lwImagesDir.Length()) == 0)
    {
        // set the path relative to the Lightwave images dir
        return nString(&(cleanPath.Get()[lwImagesDir.Length()]));
    }

    nString textureExportDir(nFileServer2::Instance()->ManglePath("textures:"));
    textureExportDir.ToLower();
    textureExportDir.ConvertBackslashes();
    textureExportDir.StripTrailingSlash();
    textureExportDir += "/";

    if ((cleanPath.Length() > textureExportDir.Length()) &&
        strncmp(cleanPath.Get(), textureExportDir.Get(), textureExportDir.Length()) == 0)
    {
        // set the path relative to the dir where textures will be exported
        nString outPath("textures:");
        outPath.Append(&(cleanPath.Get()[textureExportDir.Length()]));
        return outPath;
    }

    nString texturesAssign("textures:");

    if ((cleanPath.Length() > texturesAssign.Length()) &&
        strncmp(cleanPath.Get(), texturesAssign.Get(), texturesAssign.Length()) == 0)
    {
        // if the path is relative to "textures:" already we don't
        // have to do anything
        return cleanPath;
    }

    if (cleanPath.FindCharIndex(':', 0) == -1)
    {
        // assume that if the path is relative it's relative to the images
        // dir and therefore valid, this doesn't work with UNC paths...
        // but who cares?
        return cleanPath;
    }

    nString errorMsg;
    errorMsg.Format("The texture must be in the '%s' directory tree,\n" \
                    "or the '%s' directory tree,\n" \
                    "please pick another texture.",
                    lwImagesDir.Get(), textureExportDir.Get());
    wxMessageBox(errorMsg.Get(), "Error");
    return nString();
}

//----------------------------------------------------------------------------
/**
*/
void
wxTextureFileCtrl::OnFileBtn(wxCommandEvent& WXUNUSED(event))
{
    n_assert(this->textCtrl);
    if (this->textCtrl)
    {
        nString curPath(this->textCtrl->GetValue().c_str());
        nString defaultDir(ConvertPathToAbsolute(curPath.ExtractDirName()));
        nString defaultFile(curPath.ExtractFileName());

        wxFileDialog dialog(this, "Select Texture",
                            defaultDir.Get(), defaultFile.Get());
        if (dialog.ShowModal() == wxID_OK)
        {
            curPath = ConvertPathToRelative(dialog.GetPath().c_str());
            this->textCtrl->SetValue(curPath.Get());
        }
    }
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
