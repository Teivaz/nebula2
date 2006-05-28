//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxcoloralphactrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxColorAlphaCtrl, wxPanel)

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
        this->colorCtrl = new wxColorCtrl(this, wxID_ANY/*, wxDefaultPosition, wxSize(20, 20)*/);
        wxStaticText* alphaLbl = new wxStaticText(this, wxID_ANY, "Alpha");
        this->alphaCtrl = new wxFloatSliderCtrl(this, wxID_ANY, 0.0f, 1.0f);

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
// EOF
//----------------------------------------------------------------------------
