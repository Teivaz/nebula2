#ifndef N_WX_COLOR_ALPHA_CONTROL_H
#define N_WX_COLOR_ALPHA_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"
#include "lwwxui/wxcolorctrl.h"
#include "lwwxui/wxfloatsliderctrl.h"
#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class wxColorAlphaCtrl
    @brief A color selector with an alpha value.

    A combined control that consists of a color box that can be clicked on to 
    pick an RGB color and a float-box to enter an alpha value in the 
    range 0.0 to 1.0.
*/
class wxColorAlphaCtrl : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxColorAlphaCtrl)

public:
    /// Default Constructor. Used during two-step construction.
    wxColorAlphaCtrl();
    /// Construct and initialize control in one step.
    wxColorAlphaCtrl(wxWindow* parent, wxWindowID id, 
                     const wxPoint& pos = wxDefaultPosition, 
                     const wxSize& size = wxDefaultSize);
    /// Initialize the control during two-step construction.
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = "wxColorAlphaCtrl");
    /// Destructor.
    virtual ~wxColorAlphaCtrl();

    /// Set the current color of the control.
    void SetValue(int r, int g, int b, float a);
    /// Get the current color in the control.
    void GetValue(int* r, int* g, int* b, float* a) const;

protected:
    /// Get the absolute minimum size of the control.
    virtual wxSize DoGetBestSize() const;

private:
    wxColorCtrl* colorCtrl;
    wxFloatSliderCtrl* alphaCtrl;
    wxSize bestSize;
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
wxColorAlphaCtrl::SetValue(int r, int g, int b, float a)
{
    if (this->colorCtrl)
        this->colorCtrl->SetValue(wxColour(r, g, b));

    if (this->alphaCtrl)
        this->alphaCtrl->SetValue(a);
}

//----------------------------------------------------------------------------
/**
*/
inline
void
wxColorAlphaCtrl::GetValue(int* r, int* g, int* b, float* a) const
{
    if (this->colorCtrl)
    {
        wxColour color = this->colorCtrl->GetValue();
        
        if (r)
            *r = color.Red();

        if (g)
            *g = color.Green();

        if (b)
            *b = color.Blue();
    }

    if (this->alphaCtrl && a)
        *a = this->alphaCtrl->GetValue();
}

//----------------------------------------------------------------------------
#endif // N_WX_COLOR_ALPHA_CONTROL_H
