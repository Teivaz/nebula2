//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwwxui/wxfloatctrl.h"
#include "wx/colordlg.h"

IMPLEMENT_DYNAMIC_CLASS(wxFloatCtrl, wxTextCtrl)

BEGIN_EVENT_TABLE(wxFloatCtrl, wxTextCtrl)
    EVT_CHAR(wxFloatCtrl::OnChar)
    EVT_KILL_FOCUS(wxFloatCtrl::OnKillFocus)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
/**
*/
wxFloatCtrl::wxFloatCtrl() :
    created(false)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
wxFloatCtrl::wxFloatCtrl(wxWindow* parent, wxWindowID id,
                         const wxPoint& pos, const wxSize& size) :
    created(false)
{
    this->Create(parent, id, pos, size);
}

//----------------------------------------------------------------------------
/**
*/
bool
wxFloatCtrl::Create(wxWindow* parent, wxWindowID id,
                    const wxPoint& pos, const wxSize& size,
                    long style, const wxString& name)
{
    this->created = wxTextCtrl::Create(parent, id, wxEmptyString, pos, size,
                                       style, wxDefaultValidator, name);
    return this->created;
}

//----------------------------------------------------------------------------
/**
*/
wxFloatCtrl::~wxFloatCtrl()
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
void
wxFloatCtrl::OnChar(wxKeyEvent& event)
{
    // Only allow numbers and dots to be entered in the control
    int keyCode = event.GetKeyCode();
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
        case '.':
        case WXK_DELETE:
            // let them through
            break;

        default:
            wxBell();
            // eat the event so no else gets it!
            return;
        }
    }
    event.Skip();
}

//----------------------------------------------------------------------------
/**
*/
void
wxFloatCtrl::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    nString tempStr(wxTextCtrl::GetValue().c_str());
    this->SetFloatValue(tempStr.AsFloat());
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
