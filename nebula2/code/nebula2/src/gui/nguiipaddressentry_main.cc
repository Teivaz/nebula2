//------------------------------------------------------------------------------
//  nguiipaddressentry_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiipaddressentry.h"

nNebulaClass(nGuiIpAddressEntry, "nguitextentry");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguiipaddressentry

    @cppclass
    nGuiIpAddressEntry
    
    @superclass
    nguitextentry
    
    @classinfo
    Docs needed.
*/

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
