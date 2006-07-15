#ifndef N_WX_FLOAT_CONTROL_H
#define N_WX_FLOAT_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"
#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class wxFloatCtrl

    A text control that only accepts floating point numbers.
*/
class wxFloatCtrl : public wxTextCtrl
{
    DECLARE_DYNAMIC_CLASS(wxFloatCtrl)

public:
    /// Default Constructor. Used during two-step construction.
    wxFloatCtrl();
    /// Construct and initialize control in one step.
    wxFloatCtrl(wxWindow* parent, wxWindowID id, 
                const wxPoint& pos = wxDefaultPosition, 
                const wxSize& size = wxDefaultSize);
    /// Initialize the control during two-step construction.
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = "wxFloatCtrl");
    /// Destructor.
    virtual ~wxFloatCtrl();

    /// Set the current value of the control.
    void SetFloatValue(float);
    /// Get the current value in the control.
    float GetFloatValue() const;

private:
    virtual void OnChar(wxKeyEvent& event);
    virtual void OnKillFocus(wxFocusEvent& event);

    bool created;

    class TextCtrlEvtHandler;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
wxFloatCtrl::SetFloatValue(float val)
{
    n_assert(this->created);
    if (this->created)
    {
        wxTextCtrl::SetValue(nString::FromFloat(val).Get());
    }
}

//----------------------------------------------------------------------------
/**
*/
inline
float 
wxFloatCtrl::GetFloatValue() const
{
    n_assert(this->created);
    if (this->created)
    {
        nString tempStr(wxTextCtrl::GetValue().c_str());
        return tempStr.AsFloat();
    }
    return 0.0f;
}

//----------------------------------------------------------------------------
#endif // N_WX_FLOAT_CONTROL_H
