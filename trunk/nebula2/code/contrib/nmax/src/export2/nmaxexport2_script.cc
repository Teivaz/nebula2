//-----------------------------------------------------------------------------
//  nmaxexport2_script.cc
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "maxscrpt/maxscrpt.h"
#include "maxscrpt/parser.h"
#include "maxscrpt/definsfn.h"

//-----------------------------------------------------------------------------
/**
*/
Value* NebulaExport_cf(Value** arg_list, int count)
{
    //Value* result;

    return &ok;
}

//-----------------------------------------------------------------------------
def_visible_primitive(NebulaExport, "GetScriptFromObject");
