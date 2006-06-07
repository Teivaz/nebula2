//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxintsliderctrl.h"
#include "lwwxui/wxcustomevents.h"

//----------------------------------------------------------------------------
/**
    @class wxFloatSliderCtrl::TextCtrlEvtHandler
    @brief Custom event handler for the wxTextCtrl sub-control.

    We want to update the wxSlider sub-control whenever the wxTextCtrl
    sub-control loses input focus. Unfortunately creating a custom event
    handler seems like the only way to accomplish this.
*/
class wxIntSliderCtrl::TextCtrlEvtHandler : public wxEvtHandler
{
public:
    TextCtrlEvtHandler(wxIntSliderCtrl* parent) :
        parent(parent)
    {
        // empty
    }

    virtual bool ProcessEvent(wxEvent& event)
    {
        if (event.GetEventType() == wxEVT_KILL_FOCUS)
        {
            assert(this->parent);
            assert(this->parent->textCtrl);
            assert(this->parent->sliderCtrl);

            if (this->parent && this->parent->textCtrl && this->parent->sliderCtrl)
            {
                nString tempStr(this->parent->textCtrl->GetValue().c_str());
                int origVal = tempStr.AsInt();
                int clampedVal = this->parent->ClampValue(origVal);
                if (origVal != clampedVal)
                {
                    tempStr.SetInt(clampedVal);
                    this->parent->textCtrl->SetValue(tempStr.Get());
                }
                this->parent->sliderCtrl->SetValue(clampedVal);
                // fire off a custom event to let anyone interested know the
                // value of the control probably changed
                wxCommandEvent changeEvent(wxEVT_CUSTOM_CHANGE, parent->GetId());
                changeEvent.SetEventObject(parent);
                parent->GetEventHandler()->ProcessEvent(changeEvent);
            }
        }
        else if (event.GetEventType() == wxEVT_CHAR)
        {
            // Only allow numbers and dots to be entered in the 
            // wxTextCtrl sub-control.
            int keyCode = ((wxKeyEvent*)&event)->GetKeyCode();
            // Don't filter most of the special keys
            if ((keyCode > WXK_SPACE) && (keyCode < WXK_START))
            {
                switch (keyCode)
                {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case WXK_DELETE:
                    // let them through
                    break;

                default:
                    wxBell();
                    // eat the event so no else gets it!
                    return true;
                }
            }
        }
        event.Skip();
        return false;
    }

private:
    wxIntSliderCtrl* parent;
};

//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxIntSliderCtrl, wxPanel)

enum
{
    ID_SLIDER = wxID_HIGHEST + 1,
};

BEGIN_EVENT_TABLE(wxIntSliderCtrl, wxPanel)
    EVT_COMMAND_SCROLL(ID_SLIDER, wxIntSliderCtrl::OnScroll)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
wxIntSliderCtrl::wxIntSliderCtrl() :
    sliderCtrl(0),
    textCtrl(0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxIntSliderCtrl::wxIntSliderCtrl(wxWindow* parent, wxWindowID id, 
                                 int minVal, int maxVal,
                                 const wxPoint& pos, const wxSize& size) :
    sliderCtrl(0),
    textCtrl(0)
{
    this->Create(parent, id, minVal, maxVal, pos, size);
}

//----------------------------------------------------------------------------
/**
*/
bool 
wxIntSliderCtrl::Create(wxWindow* parent, wxWindowID id,
                        int minVal, int maxVal,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name)
{
    if (wxPanel::Create(parent, id, pos, size, style, name))
    {
        this->sliderCtrl = new wxSlider(this, ID_SLIDER, minVal, minVal, maxVal);
        this->sliderCtrl->SetMinSize(wxSize(50, 24));
        this->textCtrl = new wxTextCtrl(this, wxID_ANY);
        this->textCtrl->SetMinSize(wxSize(70, 20)); // fixme
        this->textCtrl->PushEventHandler(new TextCtrlEvtHandler(this));

        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        topSizer->Add(this->sliderCtrl, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
        topSizer->Add(this->textCtrl, 0, wxALIGN_CENTER_VERTICAL);
        this->bestSize = topSizer->Fit(this);
        this->SetSizer(topSizer);
        // work around for a bug in wxWidgets sizing, if I don't do
        // this the sizing goes nuts and all the sub-controls
        // end up on top of each other
        this->bestSize.SetWidth(this->bestSize.GetWidth() + 1);
        
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
wxIntSliderCtrl::~wxIntSliderCtrl()
{
    // have to pop and delete the custom event handler we created and pushed 
    // back in wxFloatSliderCtrl::Create() otherwise bad things can happen
    if (this->textCtrl)
        this->textCtrl->PopEventHandler(true);

    this->textCtrl = 0;
    this->sliderCtrl = 0;
}

//----------------------------------------------------------------------------
/**
*/
int 
wxIntSliderCtrl::ClampValue(int val) const
{
    assert(this->sliderCtrl);
    
    if (this->sliderCtrl)
    {
        if (val < this->sliderCtrl->GetMin())
        {
            val = this->sliderCtrl->GetMin();
        }
        else if (val > this->sliderCtrl->GetMax())
        {
            val = this->sliderCtrl->GetMax();
        }
    }
    return val;
}

//----------------------------------------------------------------------------
/**
    @brief Handle the scroll event of the wxSlider sub-control.

    The wxTextCtrl sub-control will be updated to reflect the new position
    within the wxSlider sub-control.
*/
void 
wxIntSliderCtrl::OnScroll(wxScrollEvent& event)
{
    assert(this->textCtrl);
    if (this->textCtrl)
    {
        this->textCtrl->SetValue(nString::FromInt(event.GetPosition()).Get());
    }
    // fire off a custom event to let anyone interested know the
    // value of the control probably changed
    wxCommandEvent changeEvent(wxEVT_CUSTOM_CHANGE, this->GetId());
    changeEvent.SetEventObject(this);
    this->GetEventHandler()->ProcessEvent(changeEvent);
}

//----------------------------------------------------------------------------
/**
*/
wxSize 
wxIntSliderCtrl::DoGetBestSize() const
{
    return this->bestSize;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
