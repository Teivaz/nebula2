//-----------------------------------------------------------------------------
//  nmaxdlgscript.cc
//
//  (C)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include <maxscrpt/maxscrpt.h>
#include <maxscrpt/definsfn.h>

#include "export2/nmax.h"
#include "base/nmaxlistener.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxdirdlg.h"

//-----------------------------------------------------------------------------
/**
    max script interface to show directory setting dialog up with macro script.
*/
Value* nShowDirSettingDlg_cf(Value** arg_list, int count)
{
    // defined in nmaxdirdlg.cc
    ShowDirSettingDlg();

    return &ok;
}

//-----------------------------------------------------------------------------
/**
    function registering macro for nShowDirSettingDlg_cf
*/
def_visible_primitive(nShowDirSettingDlg, "nShowDirSettingDlg");

//-----------------------------------------------------------------------------
/**
    max script interface to export scene.
*/
Value* nExportScene_cf(Value** arg_list, int count)
{
    check_arg_count(nExportScene, 1, count);

    char* filename = arg_list[0]->to_string();

    Interface* intf = GetCOREInterface();

    if (!ExportScene(filename, intf))
    {
        n_listener("Failed to export scene.\n");
        return &undefined;
    }

    return &ok;
}

//-----------------------------------------------------------------------------
/**
    function registering macro for nExportScene_cf
*/
def_visible_primitive(nExportScene, "nExportScene");
