#ifndef N_WX_COLOR_CONTROL_H
#define N_WX_COLOR_CONTROL_H
//----------------------------------------------------------------------------
#include "wx/wx.h"

//----------------------------------------------------------------------------
/**
    @class wxColorCtrl
    @brief A color selector.

    A simple color box that can be clicked on to display the OS color picker,
    once the user picks a color the color box will be redrawn with that
    color.
*/
class wxColorCtrl : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxColorCtrl)

public:
    /// Default Constructor. Used during two-step construction.
    wxColorCtrl();
    /// Construct and initialize control in one step.
    wxColorCtrl(wxWindow* parent, wxWindowID id, 
                const wxPoint& pos = wxDefaultPosition, 
                const wxSize& size = wxDefaultSize);
    /// Initialize the control during two-step construction.
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = "wxColorCtrl");
    /// Destructor.
    virtual ~wxColorCtrl();

    /// Set the current color of the control.
    void SetValue(wxColour& color);
    /// Get the current color in the control.
    const wxColour& GetValue() const;

protected:
    /// Get the absolute minimum size of the control.
    virtual wxSize DoGetBestSize() const;

private:
    virtual void OnPaint(wxPaintEvent& event);
    virtual void OnEraseBackground(wxEraseEvent& event);
    virtual void OnLeftUp(wxMouseEvent& event);

    wxColour curColor;
    bool created;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
wxColorCtrl::SetValue(wxColour& color)
{
    this->curColor = color;
    if (this->created)
        this->Refresh();
}

//----------------------------------------------------------------------------
/**
*/
inline
const wxColour& 
wxColorCtrl::GetValue() const
{
    return this->curColor;
}

//----------------------------------------------------------------------------
#endif // N_WX_COLOR_CONTROL_H
