//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxfloatsliderctrl.h"

//----------------------------------------------------------------------------
/**
    @class wxFloatSliderCtrl::TextCtrlEvtHandler
    @brief Custom event handler for the wxTextCtrl sub-control.
*/
class wxFloatSliderCtrl::TextCtrlEvtHandler : public wxEvtHandler
{
public:
    TextCtrlEvtHandler(wxFloatSliderCtrl* parent) :
        parent(parent)
    {
        n_assert(parent);
    }

    virtual bool ProcessEvent(wxEvent& event)
    {
        if (event.GetEventType() == wxEVT_KILL_FOCUS)
        {
            // Update the wxSlider sub-control whenever the 
            // wxFloatCtrl sub-control loses input focus.
            if (this->parent->floatCtrl && this->parent->sliderCtrl)
            {
                float origVal = this->parent->floatCtrl->GetFloatValue();
                float clampedVal = this->parent->ClampValue(origVal);
                this->parent->floatCtrl->SetFloatValue(clampedVal);
                int sliderPos = this->parent->ScaleFloatToInt(clampedVal);
                this->parent->sliderCtrl->SetValue(sliderPos);
            }
        }
        else
        {
            // pass the event down the handler chain
            if (this->GetNextHandler())
                return this->GetNextHandler()->ProcessEvent(event);
        }
        return false;
    }

private:
    wxFloatSliderCtrl* parent;
};

//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxFloatSliderCtrl, wxPanel)

enum
{
    ID_SLIDER = wxID_HIGHEST + 1,
};

BEGIN_EVENT_TABLE(wxFloatSliderCtrl, wxPanel)
    EVT_COMMAND_SCROLL(ID_SLIDER, wxFloatSliderCtrl::OnScroll)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
wxFloatSliderCtrl::wxFloatSliderCtrl() :
    sliderCtrl(0),
    floatCtrl(0),
    minValue(0.0f),
    maxValue(0.0f),
    curValue(0.0f)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxFloatSliderCtrl::wxFloatSliderCtrl(wxWindow* parent, wxWindowID id, 
                                     float minVal, float maxVal,
                                     const wxPoint& pos, const wxSize& size) :
    sliderCtrl(0),
    floatCtrl(0),
    curValue(minVal)
{
    this->Create(parent, id, minVal, maxVal, pos, size);
}

//----------------------------------------------------------------------------
/**
*/
bool 
wxFloatSliderCtrl::Create(wxWindow* parent, wxWindowID id,
                          float minVal, float maxVal,
                          const wxPoint& pos, const wxSize& size,
                          long style, const wxString& name)
{
    this->minValue = minVal;
    this->maxValue = maxVal;

    if (wxPanel::Create(parent, id, pos, size, style, name))
    {
        this->sliderCtrl = new wxSlider(this, ID_SLIDER, 0, 0, 100);
        this->sliderCtrl->SetPageSize(10);
        this->sliderCtrl->SetMinSize(wxSize(50, 24));
        this->floatCtrl = new wxFloatCtrl(this, wxID_ANY);
        this->floatCtrl->SetMinSize(wxSize(70, 20)); // FIXME?
        this->floatCtrl->PushEventHandler(new TextCtrlEvtHandler(this));

        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        topSizer->Add(this->sliderCtrl, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
        topSizer->Add(this->floatCtrl, 0, wxALIGN_CENTER_VERTICAL);
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
wxFloatSliderCtrl::~wxFloatSliderCtrl()
{
    // have to pop and delete the custom event handler we created and pushed 
    // back in wxFloatSliderCtrl::Create() otherwise bad things can happen
    if (this->floatCtrl)
        this->floatCtrl->PopEventHandler(true);

    this->floatCtrl = 0;
    this->sliderCtrl = 0;
}

//----------------------------------------------------------------------------
/**
    @brief Convert the given float value to an integer in the range 0-100.
*/
int 
wxFloatSliderCtrl::ScaleFloatToInt(float val) const
{
    int intVal;
    // converting the floating point value to the integer range 0-100
    // is not a very precise operation, which is generally fine except
    // for the ends of the slider where we want an exact match
    if (val == this->maxValue)
    {
        intVal = 100;
    }
    else if (val == this->minValue)
    {
        intVal = 0;
    }
    else
    {
        intVal = (int)((val / (this->maxValue - this->minValue)) * 100);
    }
    return intVal;
}

//----------------------------------------------------------------------------
/**
    @brief Convert the given integer in the range 0-100 to a float.
*/
float
wxFloatSliderCtrl::ScaleIntToFloat(int inVal) const
{
    float outVal;
    // converting the integer value to the floating point value
    // is not a very precise operation, which is generally fine except
    // for the ends of the slider where we want an exact match
    if (100 == inVal)
    {
        outVal = this->maxValue;
    }
    else if (0 == inVal)
    {
        outVal = this->minValue;
    }
    else
    {
        outVal = ((float)inVal / 100.0f) * (this->maxValue - this->minValue);
    }
    return outVal;
}

//----------------------------------------------------------------------------
/**
*/
float 
wxFloatSliderCtrl::ClampValue(float val) const
{
    if (val < this->minValue)
    {
        return this->minValue;
    }
    if (val > this->maxValue)
    {
        return this->maxValue;
    }
    return val;
}

//----------------------------------------------------------------------------
/**
    @brief Handle the scroll event of the wxSlider sub-control.

    The wxFloatCtrl sub-control will be updated to reflect the new position
    within the wxSlider sub-control.
*/
void 
wxFloatSliderCtrl::OnScroll(wxScrollEvent& event)
{
    if (this->floatCtrl)
    {
        this->curValue = this->ScaleIntToFloat(event.GetPosition());
        this->floatCtrl->SetFloatValue(this->curValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
wxSize 
wxFloatSliderCtrl::DoGetBestSize() const
{
    return this->bestSize;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
