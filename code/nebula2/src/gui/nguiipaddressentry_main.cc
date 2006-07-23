//------------------------------------------------------------------------------
//  nguiipaddressentry_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiipaddressentry.h"

nNebulaClass(nGuiIpAddressEntry, "gui::nguitextentry");

//------------------------------------------------------------------------------
/**
*/
nGuiIpAddressEntry::nGuiIpAddressEntry()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiIpAddressEntry::~nGuiIpAddressEntry()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiIpAddressEntry::OnChar(uchar charCode)
{
    if (this->active)
    {
        if ((charCode == '.') || (isdigit(charCode) != 0))
        {
            this->lineEditor->InsertChar(charCode);
        }
    }
}
