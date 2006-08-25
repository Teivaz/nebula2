//-----------------------------------------------------------------------------
//  nmaxmaterial_ui.cc
//
//  Generate UI part of script in rollout of 3dsmax scripted plug-in
//  to handle Nebula2 custom material.
//
//  (c)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------

#include "export2/nmax.h"
#include "util/nstring.h"
#include "export2/nmaxmaterial_ui.h"

//-----------------------------------------------------------------------------
/**
    Retrieves default value which descriped in 'def'.

    -14-Mar-06  kims  changed to get proper range for value of color picker.
*/
nString 
GetDefault(TiXmlElement* elemParam)
{
    nString paramType = elemParam->Attribute("type");
    nString defVal = elemParam->Attribute("def");

    nString defScript;

    if (paramType == "Int" || paramType == "Float")
    {
        defScript += defVal;
    }
    else
    if (paramType == "Bool")
    {
        int val = defVal.AsInt();

        if (val < 1)
            defScript += "false";
        else
            defScript += "true";
    }
    else
    if (paramType == "Color")
    {
        nArray<nString> tokens;
        int numValues = defVal.Tokenize(" ", tokens);

        defScript += "[";
        for (int i=0; i<numValues; i++)
        {
            // A Color value of 3dsmax are normally in the range 0.0 to 255.0 and 
            // Nebula uses 0.0 to 1.0 for its range.
            // so it needs to be converted to the proper range.
            nString tmp = tokens[i];
            float fval = tmp.AsFloat();
            tmp.Clear();
            tmp.SetFloat(fval * 255.0f);

            defScript += tmp;
            
            if (i < numValues - 1)
                defScript += ", ";
        }
        defScript += "]";
    }
    else
    if (paramType == "Texture" || 
        paramType == "BumpTexture" || 
        paramType == "CubeTexture" )
    {
    }
    else
    if (paramType == "Enum")
    {
        defScript += defVal;
    }
    else
    if (paramType == "Vector")
    {
        nArray<nString> tokens;
        int numValues = defVal.Tokenize(" ", tokens);

        defScript += "[";
        for (int i=0; i<numValues; i++)
        {
            defScript += tokens[i];
            if (i < numValues - 1)
                defScript += ", ";
        }
        defScript += "]";
    }
    else
    {
        // unknown. return an empty string.
    }

    return defScript;
}

//-----------------------------------------------------------------------------
/**
    Convert string of enum values to items of dropdownlist UI.

    @param enums   string of enum values. e.g. enum = "None=1:CW:CCW"
    @param values  to contain each enum values string.
*/
static
void 
EnumToItem(const nString &enums, nArray<nString> &values)
{
    nArray<nString> tokens;
    int count = enums.Tokenize(":", tokens);

    nString str;
    for (int i=0; i<tokens.Size(); i++)
    {
        str = tokens[i].GetFirstToken("=");
        values.Append(str);
    }
}

//-----------------------------------------------------------------------------
/**
    Add spinner UI script to rollout clause.

    Example:
    @verbatim
    spinner RenderPri "Render Priority" align:#left fieldwidth:36 range:[-100,100,0]
    @endverbatim
*/
nString 
AddSpinner(TiXmlElement* elemParam)
{
    nString paramName  = elemParam->Attribute("name");  // UI control name.
    nString caption    = elemParam->Attribute("label"); // UI caption 
    nString defaultVal = elemParam->Attribute("def");   // default value of the ui.

    nString uiScript;
    uiScript += "\t\t";
    uiScript += "spinner";
    uiScript += " ";
    uiScript += paramName;
    uiScript += " ";
    uiScript += "\"" + caption + "\"";
    uiScript += " ";
    uiScript += "align:#left";
    uiScript += " ";

    uiScript += "fieldwidth:36";
    uiScript += " ";

    // if the element has spinner UI, there's min/max attributes in xml element.
    nString min = elemParam->Attribute("min");
    nString max = elemParam->Attribute("max");

    uiScript += "range:[";
    uiScript += min;
    uiScript += ",";
    uiScript += max.Substitute("+", ""); // max value has '+' sign, so we substitute it to empty char.
    uiScript += ",";
    
    uiScript += GetDefault(elemParam);
    
    uiScript += "]";
    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add checkbox UI script to rollout clause.

    Example:
    @verbatim
    checkbox LockViewer "Lock To Viewer" align:#left checked:false
    @endverbatim
*/
nString 
AddCheckBox(TiXmlElement* elemParam)
{
    nString paramName = elemParam->Attribute("name");
    nString caption = elemParam->Attribute("label");
    nString defaultVal = elemParam->Attribute("def");

    nString uiScript;

    uiScript += "\t\t";
    uiScript += "checkbox";
    uiScript += " ";
    uiScript += paramName;
    uiScript += " ";
    uiScript += "\"" + caption + "\"";
    uiScript += " ";
    uiScript += "align:#left";
    uiScript += " ";

    uiScript += "checked:";
    uiScript += GetDefault(elemParam);

    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add dropdownlist UI script to rollout clause.

    Example:
    @verbatim
    dropdownlist CullMode "Cull Mode" align:#left width:100 items:#("None","CW","CCW") selection:2
    @endverbatim
*/
nString 
AddDropdownList(TiXmlElement* elemParam)
{
    nString paramName = elemParam->Attribute("name");
    nString caption = elemParam->Attribute("label");

    nString uiScript;

    uiScript += "\t\t";
    uiScript += "dropdownlist";
    uiScript += " ";
    uiScript += paramName;
    uiScript += " ";
    uiScript += "\"" + caption + "\"";
    uiScript += " ";
    uiScript += "align:#left";
    uiScript += " ";

    uiScript += "width:100";
    uiScript += " ";

    // we assume that if we have dropdownlist, the type of the element is 'Enum'.
    // if the ui is 'dropdownlist', it needs items.

    nString enums = elemParam->Attribute("enum");

    nArray<nString> values;

    EnumToItem(enums, values);

    uiScript += "items:#(";

    for (int i=0;i<values.Size(); i++)
    {
        uiScript += "\"";
        uiScript += values[i];
        uiScript += "\"";

        if (i < values.Size() - 1)
            uiScript += ",";
    }

    uiScript += ")";
    uiScript += " ";
    
    // add default value. 
    uiScript += "selection:";
    uiScript += GetDefault(elemParam);
    uiScript += " ";

    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add colorpicker UI script to rollout clause.

    Example:
    @verbatim
    colorpicker MatDiffuse "Diffuse Color" align:#left alpha:true color:[1.0, 1.0, 1.0, 1.0]
    @endverbatim
*/
nString 
AddColorPicker(TiXmlElement* elemParam)
{
    nString paramName = elemParam->Attribute("name");
    nString caption = elemParam->Attribute("label");

    nString uiScript;

    uiScript += "\t\t";
    uiScript += "colorpicker";
    uiScript += " ";
    uiScript += paramName;
    uiScript += " ";
    uiScript += "\"" + caption + "\"";
    uiScript += " ";
    uiScript += "align:#left";
    uiScript += " ";

#if MAX_RELEASE >= 6000
    uiScript += "alpha:true";
    uiScript += " ";
#endif

    // default value.
    uiScript += "color:";
    uiScript += GetDefault(elemParam);
    uiScript += " ";

    // 3dsmax5 orlower versions does not support specifyng alpha in ColorPicker.
    // So additional spinner UI for handling alpha value.
#if MAX_RELEASE < 6000
    uiScript += "across:2 ";

    uiScript += "\n";
    uiScript += "spinner ";
    nString uiName; 
    uiName.Format("%sAlpha",paramName.Get());
    uiScript += uiName;
    uiScript += " ";
    uiScript += "Alpha"; //caption
    uiScript += " ";
    uiScript += "type:#float"; //always float type for alpha
    uiScript += " ";
    uiScript += "align:#left";
    uiScript += " ";
    uiScript += "fieldwidth:36";
#endif

    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add mapbutton UI script to rollout clause.

    Example:
    @verbatim
    mapbutton DiffMap0 "Texture Map" align:#left width:150
    @endverbatim
*/
nString 
AddMapButton(TiXmlElement* elemParam, nString defaultVal)
{
    nString paramName = elemParam->Attribute("name");
    nString caption = elemParam->Attribute("label");

    nString uiScript;

    nString dummyLabel = paramName;
    dummyLabel += "_";
    uiScript += AddLabel(dummyLabel, caption, 2, true);

    uiScript += "\t\t";
    uiScript += "mapbutton";
    uiScript += " ";
    uiScript += paramName;
    uiScript += " ";
    uiScript += "\"" + caption + "\"";
    uiScript += " ";
    // add default map property if the default value is given.
    if (!defaultVal.IsEmpty())
    {
        uiScript += "map:";
        uiScript += "\"" + defaultVal + "\"";
        uiScript += " ";
    }
    uiScript += "align:#left";
    uiScript += " ";
    uiScript += "width:150";
    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add four spinner UI script to rollout clause for 'Vector' type.

    Example:
    @verbatim
    label TexGenS "Texture Scale " align:#left across:5
    spinner TexGenS0 align:#left fieldwidth:36
    spinner TexGenS01 align:#left fieldwidth:36
    spinner TexGenS012 align:#left fieldwidth:36
    spinner TexGenS0123 align:#left fieldwidth:36
    @endverbatim
*/
nString 
AddVector4Spinner(TiXmlElement* elemParam)
{
    nString paramName  = elemParam->Attribute("name");  // UI control name.
    nString caption    = elemParam->Attribute("label"); // UI caption 

    nString uiScript;

    // insert dummy label which has parameters name for its UI name.
    caption += " ";
    uiScript += AddLabel(paramName, caption, 5);

    nString index, label;

    // create four spinner for vector type.
    for (int i=0; i<4; i++)
    {
        index.AppendInt(i);
        
        //switch(i)
        //{
        //case 0: label = "x:"; break;
        //case 1: label = "y:"; break;
        //case 2: label = "z:"; break;
        //case 3: label = "w:"; break;
        //}

        uiScript += "\t\t";
        uiScript += "spinner";
        uiScript += " ";
        uiScript += paramName + index;
        uiScript += " ";
        //uiScript += "\"" + label + "\"";
        //uiScript += " ";
        uiScript += "align:#left";
        uiScript += " ";
        uiScript += "fieldwidth:36";
        uiScript += "\n";

        //if (i==1)
        //    uiScript += "across:4";
    }

    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add label UI script to rollout clause.

    Example:
    @verbatim
    label dummy "" align:#left across:1
    @endverbatim
*/
nString 
AddLabel(const nString &uiname, const nString &caption, int across, bool addDot)
{
    nString uiScript;

    uiScript += "\t\t";
    uiScript += "label";
    uiScript += " ";
    uiScript += uiname;
    uiScript += " ";
    uiScript += "\"";
    uiScript += caption ;
    if (addDot)
    {
        uiScript += "..................................................";
    }
    uiScript += "\"";
    uiScript += " ";

    uiScript += "align:#left";
    uiScript += " ";

    uiScript += "across:";

    nString strAcross;
    strAcross.AppendInt(across);
    uiScript += strAcross;

    uiScript += "\n";

    return uiScript;
}

//-----------------------------------------------------------------------------
/**
    Add texture directory setting button.

    Example:
    @verbatim
	button btnFldBumpMap0 "texture:"align:#right width:150
	on btnFBumpMap0 pressed do
	(
		mapPath = getSavePath caption:"Select path of Wave Bump Map"
		if mapPath != undefined then 
		(
			dirSettingBumpMap0  = mapPath
            edtFldBumpMap0.text = mapPath
		)
	)
    @endverbatim
*/
nString
AddSetDirDlg(TiXmlElement* elemParam)
{
    nString uiname  = elemParam->Attribute("name");
    nString caption = elemParam->Attribute("label");

    nString uiScript;
    nString uiPrefix = "Fld";

    // script code for 'edittext' control
    uiScript += "\t\t";
    uiScript += "edittext";
    uiScript += " ";
    uiScript += "edt";
    uiScript += uiPrefix + uiname;
    uiScript += " ";
    uiScript += "\"";
    uiScript += "Dest Folder : ";
    uiScript += "\"";
    uiScript += " align:#left fieldWidth:180 across:2 readonly:true \n";

    // script code for 'button' control
    uiScript += "\t\t";
    uiScript += "button";
    uiScript += " ";
    uiScript += "btn";
    uiScript += uiPrefix + uiname;
    uiScript += " ";

    //button name.
    uiScript += " ";
    uiScript += "\"";
    uiScript += "<<";
    uiScript += "\"";
    uiScript += " ";

    uiScript += "align:#right";
    uiScript += " ";
    uiScript += "width:40 height:18";
    uiScript += "\n";

    // button handler scrpt code
    uiScript += "\t\t";
    uiScript += "on ";
    uiScript += "btn";
    uiScript += uiPrefix + uiname;
    uiScript += " ";
    uiScript += "pressed do\n";
    uiScript += "\t\t";
    uiScript += "(\n";
    
    uiScript += "\t\t\t";

    uiScript += "mapPath = getSavePath";
    uiScript += " ";
    uiScript += "caption:";
    uiScript += "\"";
    uiScript += "Select a directory where the ";
    uiScript += caption;
    uiScript += " to be exported.";
    uiScript += "\"";
    uiScript += "\n";
    
    uiScript += "\t\t\t";
    uiScript += "if mapPath != undefined then \n";
    uiScript += "\t\t\t";
    uiScript += "(\n";

    //HACK: the string 'dirSetting' should be same with the string
    //      which can be found in GenerateScript() function and
    //      nMaxMaterial::GetNebulaMaterial() function.
    //      code : dirSettngDiffmap0 = mapPath
    uiScript += "\t\t\t\t"; 
    uiScript += "dirSetting" + uiname;
    uiScript += " = mapPath";
    uiScript += "\n";

    // code : edtFldDiffMap0.text = mapPath
    uiScript += "\t\t\t\t";
    uiScript += "edt";
    uiScript += uiPrefix + uiname;
    uiScript += ".text";
    uiScript += " = mapPath";
    uiScript += "\n";
    
    uiScript += "\t\t\t";
    uiScript += ")\n";

    uiScript += "\t\t";
    uiScript += ")\n";

    return uiScript;
}
