#ifndef N_WX_INT_SLIDER_CONTROL_H
#define N_WX_INT_SLIDER_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"
#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class wxIntSliderCtrl
    @brief An integer only text-box with a slider.

    A combined control that consists of a slider and a text-box that only
    allows integers to be entered. The integers entered in the text-box will
    be clamped to be within the range of the slider.
*/
class wxIntSliderCtrl : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxIntSliderCtrl)

public:
    /// Default Constructor. Used during two-step construction.
    wxIntSliderCtrl();
    /// Construct and initialize control in one step.
    wxIntSliderCtrl(wxWindow* parent, wxWindowID id,
                    int minVal, int maxVal,
                    const wxPoint& pos = wxDefaultPosition, 
                    const wxSize& size = wxDefaultSize);
    /// Initialize the control during two-step construction.
    bool Create(wxWindow* parent, wxWindowID id,
                int minVal, int maxVal,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = "wxFloatSliderCtrl");
    /// Destructor.
    virtual ~wxIntSliderCtrl();

    /// Set the current value of the control.
    void SetValue(int);
    /// Get the current value of the control.
    int GetValue();

protected:
    /// Get the absolute minimum size of the control.
    virtual wxSize DoGetBestSize() const;

private:
    int ClampValue(int) const;

    void OnScroll(wxScrollEvent& event);

    wxSlider* sliderCtrl;
    wxTextCtrl* textCtrl;
    wxSize bestSize;

    class TextCtrlEvtHandler;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
wxIntSliderCtrl::SetValue(int val)
{
    if (this->sliderCtrl)
    {
        this->sliderCtrl->SetValue(val);
    }
    if (this->textCtrl)
    {
        this->textCtrl->SetValue(nString::FromInt(val).Get());
    }
}

//----------------------------------------------------------------------------
/**
*/
inline
int
wxIntSliderCtrl::GetValue()
{
    if (this->textCtrl)
    {
        nString tempStr(this->textCtrl->GetValue().c_str());
        return tempStr.AsInt();
    }
    return 0;
}

//----------------------------------------------------------------------------
#endif // N_WX_INT_SLIDER_CONTROL_H
