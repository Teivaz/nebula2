//-----------------------------------------------------------------------------
//  nmaxexport2_script.cc
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include <maxscrpt/maxscrpt.h>
#include <maxscrpt/parser.h>
#include <maxscrpt/definsfn.h>

#include "export2/nmax.h"
#include "base/nmaxlistener.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxdirdlg.h"

#include "export2/nmaxoptionscriptext.h"

//-----------------------------------------------------------------------------
/**
    max script which exports the given node.

      - 1st in-arg - filename to save the node.
      - 2nd in-arg - node to export.
*/
Value* nExportNode_cf(Value** arg_list, int count)
{
    check_arg_count(nExportScene, 2, count);

    // filename which to used for saving a scene.
    char* filename = arg_list[0]->to_string();
    if (!filename)
    {
        n_listener("Invalid filename.\n");
        return &undefined;
    }

    // node which to export
    INode* node = arg_list[1]->to_node();
    if (!node)
    {
        n_listener("Invalid node.\n");
        return &undefined;
    }

    Interface* intf = GetCOREInterface();

    if (!ExportScene(filename, intf, node))
    {
        n_listener("Failed to export scene.\n");
        return &undefined;
    }

    return &ok;
}

//-----------------------------------------------------------------------------
def_visible_primitive(nExportNode, "nExportNode");

//-----------------------------------------------------------------------------
/**
    max script interface to export a scene. (export whole scene)
*/
Value* nExportScene_cf(Value** arg_list, int count)
{
    check_arg_count(nExportScene, 1, count);

    char* filename = arg_list[0]->to_string();

    Interface* intf = GetCOREInterface();

    if (!ExportScene(filename, intf, 0))
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
    function registering macro for nShowDirSettingDlg_cf.
*/
def_visible_primitive(nShowDirSettingDlg, "nShowDirSettingDlg");

//-----------------------------------------------------------------------------
/**
    Retrieves project directory name.
*/
Value* nGetProjDir_cf(Value** arg_list, int count)
{
    one_typed_value_local(Value* result);
    Value* result = &undefined;

    const char* projDir = GetProjDir();

    int len;
    if (len = strlen(projDir))
    {
        char* buf = new char[len + 1];
        sprintf(buf, "%s", projDir);
        result = new String(buf);
    }
    else
    {
        result = &undefined;
    }

    return_value(result);
}

//-----------------------------------------------------------------------------
/**
    function registering macro for nGetPorjDir_cf.
*/
def_visible_primitive(nGetProjDir, "nGetProjDir");
