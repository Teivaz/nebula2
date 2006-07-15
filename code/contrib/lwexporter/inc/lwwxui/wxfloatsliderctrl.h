#ifndef N_WX_FLOAT_SLIDER_CONTROL_H
#define N_WX_FLOAT_SLIDER_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"
#include "lwwxui/wxfloatctrl.h"

//----------------------------------------------------------------------------
/**
    @class wxFloatSliderCtrl
    @brief A float only text-box with a slider.

    A combined control that consists of a slider and a text-box that only
    allows floats to be entered. The floats entered in the text-box will
    be clamped to be within the range of the slider.
*/
class wxFloatSliderCtrl : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxFloatSliderCtrl)

public:
    /// Default Constructor. Used during two-step construction.
    wxFloatSliderCtrl();
    /// Construct and initialize control in one step.
    wxFloatSliderCtrl(wxWindow* parent, wxWindowID id,
                      float minVal, float maxVal,
                      const wxPoint& pos = wxDefaultPosition, 
                      const wxSize& size = wxDefaultSize);
    /// Initialize the control during two-step construction.
    bool Create(wxWindow* parent, wxWindowID id,
                float minVal, float maxVal,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = "wxFloatSliderCtrl");
    /// Destructor.
    virtual ~wxFloatSliderCtrl();

    /// Set the current value of the control.
    void SetValue(float);
    /// Get the current value of the control.
    float GetValue();

protected:
    /// Get the absolute minimum size of the control.
    virtual wxSize DoGetBestSize() const;

private:
    int ScaleFloatToInt(float) const;
    float ScaleIntToFloat(int) const;
    float ClampValue(float) const;
    
    void OnScroll(wxScrollEvent& event);

    wxSlider* sliderCtrl;
    wxFloatCtrl* floatCtrl;
    float minValue;
    float maxValue;
    float curValue;
    wxSize bestSize;

    class TextCtrlEvtHandler;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
wxFloatSliderCtrl::SetValue(float val)
{
    this->curValue = val;
    if (this->sliderCtrl)
    {
        this->sliderCtrl->SetValue(this->ScaleFloatToInt(this->curValue));
    }
    if (this->floatCtrl)
    {
        this->floatCtrl->SetFloatValue(this->curValue);
    }
}

//----------------------------------------------------------------------------
/**
*/
inline
float
wxFloatSliderCtrl::GetValue()
{
    if (this->floatCtrl)
    {
        this->curValue = this->floatCtrl->GetFloatValue();
    }
    return this->curValue;
}

//----------------------------------------------------------------------------
#endif // N_WX_FLOAT_SLIDER_CONTROL_H
