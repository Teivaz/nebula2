//------------------------------------------------------------------------------
/**
    nmaxlistener.cc

    (C) 2004 Kim, Hyoun Woo
*/

#include "base/nmaxlistener.h"

//
// There's conflicts between some of nebula functions and maxscript defines.
// (n_error, n_message and n_free and so on)
// So, if both of 'ntypes.h' and 'maxscrpt.h' files are included on same file
// there will be errors.
// To prevent those errors, 'maxscrpt.h' should be included separately.
//
#include "MAXScrpt\MAXScrpt.h"
#include "MAXScrpt\MAXObj.h"
#include "MAXScrpt\Listener.h"

//-----------------------------------------------------------------------------
/**
    Output message to 3dsmax maxscript listener.

    @param msg string to put on maxscript listener.
*/
void n_listener (char* msg, ...)
{
    char message [MAX_PATH];

    va_list argList;
    va_start(argList, msg);
    vsprintf (message, msg, argList);
    va_end(argList);
    
    the_listener->edit_stream->printf (message);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
