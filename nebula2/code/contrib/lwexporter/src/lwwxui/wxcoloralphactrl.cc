//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxcoloralphactrl.h"
#include "lwwxui/wxcustomevents.h"

IMPLEMENT_DYNAMIC_CLASS(wxColorAlphaCtrl, wxPanel)

enum
{
    ID_COLOR_CTRL = wxID_HIGHEST + 1,
    ID_SLIDER_CTRL,
};

BEGIN_EVENT_TABLE(wxColorAlphaCtrl, wxPanel)
    EVT_COMMAND(ID_COLOR_CTRL, wxEVT_CUSTOM_CHANGE, wxColorAlphaCtrl::OnCustomChange)
    EVT_COMMAND(ID_SLIDER_CTRL, wxEVT_CUSTOM_CHANGE, wxColorAlphaCtrl::OnCustomChange)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
/**
*/
wxColorAlphaCtrl::wxColorAlphaCtrl() :
    colorCtrl(0),
    alphaCtrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxColorAlphaCtrl::wxColorAlphaCtrl(wxWindow* parent, wxWindowID id, 
                                   const wxPoint& pos, const wxSize& size) :
    colorCtrl(0),
    alphaCtrl(0)
{
    this->Create(parent, id, pos, size);
}

//----------------------------------------------------------------------------
/**
*/
bool 
wxColorAlphaCtrl::Create(wxWindow* parent, wxWindowID id, 
                         const wxPoint& pos, const wxSize& size,
                         long style, const wxString& name)
{
    if (wxPanel::Create(parent, id, pos, size, style, name))
    {
        this->colorCtrl = new wxColorCtrl(this, ID_COLOR_CTRL);
        wxStaticText* alphaLbl = new wxStaticText(this, wxID_ANY, "Alpha");
        this->alphaCtrl = new wxFloatSliderCtrl(this, ID_SLIDER_CTRL, 0.0f, 1.0f);

        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        if (topSizer)
        {
            topSizer->Add(this->colorCtrl, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 10);
            topSizer->Add(alphaLbl, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
            topSizer->Add(this->alphaCtrl, 1, wxALIGN_CENTER_VERTICAL);
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
wxColorAlphaCtrl::~wxColorAlphaCtrl()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxSize 
wxColorAlphaCtrl::DoGetBestSize() const
{
    return this->bestSize;
}

//----------------------------------------------------------------------------
/**
*/
void 
wxColorAlphaCtrl::OnCustomChange(wxCommandEvent& WXUNUSED(event))
{
    // fire off a custom event to let anyone interested know the
    // color or alpha probably changed
    wxCommandEvent event(wxEVT_CUSTOM_CHANGE, this->GetId());
    event.SetEventObject(this);
    this->GetEventHandler()->ProcessEvent(event);
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
