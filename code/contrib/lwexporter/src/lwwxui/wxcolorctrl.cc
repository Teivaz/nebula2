//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxcolorctrl.h"
#include "wx/colordlg.h"
#include "lwwxui/wxcustomevents.h"

IMPLEMENT_DYNAMIC_CLASS(wxColorCtrl, wxControl)

BEGIN_EVENT_TABLE(wxColorCtrl, wxControl)
    EVT_PAINT(wxColorCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxColorCtrl::OnEraseBackground)
    EVT_LEFT_UP(wxColorCtrl::OnLeftUp)
END_EVENT_TABLE() 

//----------------------------------------------------------------------------
/**
*/
wxColorCtrl::wxColorCtrl() :
    created(false)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxColorCtrl::wxColorCtrl(wxWindow* parent, wxWindowID id, 
                         const wxPoint& pos, const wxSize& size) :
    created(false)
{
    this->Create(parent, id, pos, size);
}

//----------------------------------------------------------------------------
/**
*/
bool 
wxColorCtrl::Create(wxWindow* parent, wxWindowID id, 
                    const wxPoint& pos, const wxSize& size,
                    long style, const wxValidator& val, const wxString& name)
{
    this->created = wxControl::Create(parent, id, pos, size, style, val, name);
    return this->created;
}

//----------------------------------------------------------------------------
/**
*/
wxColorCtrl::~wxColorCtrl()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
wxColorCtrl::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
    wxSize ctrlSize = this->GetClientSize();
    wxPen pen(this->curColor);
    wxBrush brush(this->curColor);
    dc.SetPen(pen);
    dc.SetBrush(brush);
    dc.DrawRectangle(0, 0, ctrlSize.GetWidth(), ctrlSize.GetHeight());
}

//----------------------------------------------------------------------------
/**
*/
void 
wxColorCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void 
wxColorCtrl::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetChooseFull(true);
    data.SetColour(this->curColor);
    
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
    {
        this->curColor = dialog.GetColourData().GetColour();
        // fire off a custom event to let anyone interested know the
        // color probably changed
        wxCommandEvent event(wxEVT_CUSTOM_CHANGE, this->GetId());
        event.SetEventObject(this);
        this->GetEventHandler()->ProcessEvent(event);
    }

    this->Refresh();
}

//----------------------------------------------------------------------------
/**
*/
wxSize 
wxColorCtrl::DoGetBestSize() const
{
    // some magic numbers, but they work for me :)
    return wxSize(20, 20);
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
