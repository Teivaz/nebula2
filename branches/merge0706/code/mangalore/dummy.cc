//------------------------------------------------------------------------------
//  dummy.cc
//
//  This files should be compiled into each static link lib. It simply makes
//  sure that platform specific link libs don't compile into nothing on
//  other platform, which may confuse the linker.
//
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
/*
*/
static
void
dummy()
{
    // empty
}
