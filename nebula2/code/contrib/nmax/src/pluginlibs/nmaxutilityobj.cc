//------------------------------------------------------------------------------
//  nmaxutilityobj.cc
//
//  (c)2004 Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "pluginlibs/nmaxutilityobj.h"
#include "pluginlibs/nmaxoptionparammapdlg.h"
#include "../res/nmaxtoolbox.h"

IParamMap* nMaxUtilityObj::optionParamMap = 0;

//------------------------------------------------------------------------------
/**
*/
nMaxUtilityObj::nMaxUtilityObj()
{
    flags = ROLLUP_EXPORTOPTIONS_OPEN;
}

//------------------------------------------------------------------------------
/**
*/
nMaxUtilityObj::~nMaxUtilityObj()
{
}

//------------------------------------------------------------------------------
/**
*/
void nMaxUtilityObj::BeginEditParams(Interface* intf, IUtil* iutil)
{
    // create nebula utility panel dialog.
    optionParamMap = CreateCPParamMap(
        NULL, 
        0,//1,
        this, 
        intf, 
        maxUtilityObjInterfaceClassDesc2.HInstance(), 
        MAKEINTRESOURCE(IDD_UTILITY_OPTION),
        "Nebula Export Options",
        (flags & ROLLUP_EXPORTOPTIONS_OPEN) ? 0 : APPENDROLL_CLOSED);

    if (optionParamMap)
    {
        optionParamMap->SetUserDlgProc(new nMaxOptionParamMapDlg);
    }
}

//------------------------------------------------------------------------------
/**
*/
void nMaxUtilityObj::EndEditParams(Interface* intf, IUtil* iutil)
{
    if (optionParamMap)
    {
        // destroy nebula utility panel dialog.
        if (IsRollupPanelOpen(optionParamMap->GetHWnd()))
            flags |= ROLLUP_EXPORTOPTIONS_OPEN;
        DestroyCPParamMap(optionParamMap); 
        optionParamMap = NULL;
    }
}
