#ifndef N_GUIIPADDRESSENTRY_H
#define N_GUIIPADDRESSENTRY_H
//------------------------------------------------------------------------------
/**
    @class nGuiIpAddressEntry
    @ingroup Gui
    @brief An text entry gadget which restricts input to ip addresses.

    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguitextentry.h"

//------------------------------------------------------------------------------
class nGuiIpAddressEntry : public nGuiTextEntry
{
public:
    /// constructor
    nGuiIpAddressEntry();
    /// destructor
    virtual ~nGuiIpAddressEntry();
    /// handle character code entry
    virtual void OnChar(uchar charCode);
};
//------------------------------------------------------------------------------
#endif

