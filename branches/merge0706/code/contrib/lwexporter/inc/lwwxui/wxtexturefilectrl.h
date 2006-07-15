#ifndef N_WX_TEXTURE_FILE_CONTROL_H
#define N_WX_TEXTURE_FILE_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"
#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class wxTextureFileCtrl
    @brief Texture filename text-box control.

    A combined control that consists of a text-box that displays a texture 
    filename and a button that brings up the file selection dialog to pick
    the filename to display in the text-box. This control only allows the
    user to pick files that reside in the proj:work/images sub-tree or the
    proj:export/textures sub-tree.
*/
class wxTextureFileCtrl : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxTextureFileCtrl)

public:
    /// Default Constructor. Used during two-step construction.
    wxTextureFileCtrl();
    /// Construct and initialize control in one step.
    wxTextureFileCtrl(wxWindow* parent, wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition, 
                      const wxSize& size = wxDefaultSize);
    /// Initialize the control during two-step construction.
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = "wxTextureFileCtrl");
    /// Destructor.
    virtual ~wxTextureFileCtrl();

    /// Set the current value of the control.
    void SetValue(const nString&);
    /// Get the current value of the control.
    nString GetValue();

protected:
    /// Get the absolute minimum size of the control.
    virtual wxSize DoGetBestSize() const;

    void OnFileBtn(wxCommandEvent& event);

private:
    wxTextCtrl* textCtrl;
    wxButton* btnCtrl;
    wxSize bestSize;

    class TextCtrlEvtHandler;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
wxTextureFileCtrl::SetValue(const nString& val)
{
    assert(this->textCtrl);
    if (this->textCtrl)
        this->textCtrl->SetValue(val.Get());
}

//----------------------------------------------------------------------------
/**
*/
inline
nString
wxTextureFileCtrl::GetValue()
{
    assert(this->textCtrl);
    if (this->textCtrl)
    {
        return nString(this->textCtrl->GetValue().c_str());
    }
    return nString();
}

//----------------------------------------------------------------------------
#endif // N_WX_TEXTURE_FILE_CONTROL_H
