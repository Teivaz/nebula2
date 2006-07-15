#ifndef N_WX_CUSTOM_EVENTS_H
#define N_WX_CUSTOM_EVENTS_H
//----------------------------------------------------------------------------
#include "wx/wx.h"

// custom event that gets sent out by some custom controls when their value
// is changed by the user
DECLARE_EVENT_TYPE(wxEVT_CUSTOM_CHANGE, -1)

//----------------------------------------------------------------------------
#endif // N_WX_CUSTOM_EVENTS_H
