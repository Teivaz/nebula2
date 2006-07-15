//----------------------------------------------------------------------------
// (c) 2005    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwlayoutgenericlaunchers.h"

//----------------------------------------------------------------------------
XCALL_(int) 
Activate_LayoutToolkitMainMenu(long version, GlobalFunc* global, 
							   LWLayoutGeneric* local, void* serverData)
{
	if (version != LWINTERFACE_VERSION)
		return AFUNC_BADVERSION;

	

	return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
