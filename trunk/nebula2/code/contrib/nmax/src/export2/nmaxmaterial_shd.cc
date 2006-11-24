//-----------------------------------------------------------------------------
//  nmaxmaterial_shd.cc
//
//  Generate 3dsmax scripted plug-in to handle Nebula2 custom material.
//
//  (c)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "base/nmaxlistener.h"
#include "export2/nmaxscriptcall.h"
#include "export2/nmaxutil.h"
#include "util/nstring.h"
#include "export2/nmaxmaterial_ui.h"
#include "export2/nmaxoptions.h"

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "misc/niniprefserver.h"
#include "util/nstring.h"
#include "tinyxml/tinyxml.h"
#include "gfx2/nshaderstate.h"

/// contains shader name which to be filtered out.
static nArray<nString> shaderFilterArray;

//-----------------------------------------------------------------------------
/**
    Find the full path of shaders.xml file.

    The reason we do not directly call nMaxOption::GetHomePath() is that 
    nMaxOption is mostly used for exporting. 
    It is singleton class so calling that here makes hard to handle and destroy 
    its instance.

    @param file file to retrieve the path
*/
static
nString GetShaderXmlPath(nString file)
{
    nString shdxml;

    nString iniFilename;
    iniFilename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
    iniFilename += "\\";
    iniFilename += N_MAXEXPORT_INIFILE;

    nFileServer2* fileServer = nFileServer2::Instance();

    // check the .ini file exist in 3dsmax plugin directory.
    if (!fileServer->FileExists(iniFilename.Get()))
    {
        // .ini file does not exist in '/plugcfg' directory.
        n_message("%s file does not exist in '$3dsmax/plugcfg' directory.\n", 
            N_MAXEXPORT_INIFILE);

        // return a empty string.
        return shdxml;
    }

    nString homeDir;

    // read 'home' path from existing .ini flle in '$3dsmax/plugcfg' directory.
    nIniPrefServer* iniFile = (nIniPrefServer*)nKernelServer::Instance()->New("niniprefserver", "/iniprefsrv");
    iniFile->SetFileName(iniFilename);
    iniFile->SetSection("GeneralSettings");
    homeDir = iniFile->ReadString("HomeDir");
    iniFile->Release();

    if (homeDir.IsEmpty())
    {
        n_message("Home directory does not exist in %s file.\n", iniFilename.Get());

        // return a empty string.
        return shdxml;
    }

    shdxml += homeDir;
    shdxml += "\\";
    //shdxml += "data\\shaders\\shaders.xml";
    shdxml += "data\\shaders\\";
    shdxml += file;

    if (!nFileServer2::Instance()->FileExists(shdxml.Get()))
    {
        n_message("File %s does not exist.\n", shdxml.Get());

        // make the string to be empty then return.
        shdxml += "";
        return shdxml;
    }

    return shdxml;
}

//-----------------------------------------------------------------------------
/**
    Retrieves parameter type based on the given shader type.

    -02-Nov-06  kims  Replaced point4 to floattab to retrieve valid values from 3dsmax control.
                      Thank ZHANG Zikai for the patch.

    @param shdType shader type which described in xml file.
    @return string which represent parameter type. 
*/
static
nString GetParameterType(const nString &shdType)
{
    if (shdType == "Int")
        return "#integer";
    else
    if (shdType == "Bool")
        return "#boolean";
    else
    if (shdType == "Float")
        return "#float";
    else
    if (shdType == "String")
        return "#string";
    else
    if (shdType == "Enum")
        return "#integer";
    else
    if (shdType == "Color")
#if MAX_RELEASE >= 6000
        return "#frgba"; // we use rgba, for 3dsmax6 or higher
#else
        return "#color"; // 3dsmax5 or lower version does not support alpha in colorpicker.
#endif
    else
    if (shdType == "Vector")
//#if MAX_RELEASE >= 6000
//        return "#point4";
//#else
        return "#floatTab tabSize:4 tabSizeVariable:false";
//#endif
    else
    if (shdType == "Texture")
        return "#texturemap";
    else
    if (shdType == "BumpTexture")
        return "#texturemap";
    else
    if (shdType == "CubeTexture")
        return "#texturemap";
    else
    if (shdType == "EnvelopeCurve")
        //return "#maxObject";
        return "#floatTab tabSize:9 tabSizeVariable:false";
    else
    if (shdType == "ColorEnvelopeCurve" )
        return "#floatTab tabSize:14 tabSizeVariable:false";
    else
    {
        return "<<unknown>>";
    }
}

//-----------------------------------------------------------------------------
/**
    Retrieves UI from the given type.
    @note 
        EnvelopeCurve and ColorEnvelopeCurve are ignored.

    The followings are parameters type and its corresponding UI control.

      -# Int - spinner
      -# Float - spinner
      -# color - ColorPicker
      -# Enum - dropdownlist
      -# Texture - mapbutton
      -# BumpTexture - mapbutton
      -# CubeTexture - mapbutton
      -# Vector - spinner
      -# EnvelopeCurve - nmaxenvelopecurve custom control
      -# ColorEnvelopeCurve - nmaxenvelopecurve custom control
      -# Unknown - label

    @param type 'type' value of xml element.

    -17-Aug-06  kims Changed to add texutre directory setting button.
*/
static
nString GetUIFromType(TiXmlElement* elemParam, const nString &shdName, const nString &type)
{
    if (type == "Int")
        return AddSpinner(elemParam);
    else
    if (type == "Bool")
        return AddCheckBox(elemParam);
    else
    if (type == "Float")
        return AddSpinner(elemParam);
    else
    if (type == "Color")
        return AddColorPicker(elemParam);
    else
    if (type == "Enum")
        return AddDropdownList(elemParam);
    else
    if (type == "Texture")
    {
        nString ret;

        // Caution: The plugin max script code is always regenerated and
        //          executed at the start time of 3dsmax so it is useless to 
        //          specifying any initial value to a control or parameter block.
        ret += AddMapButton(elemParam);
        ret += AddSetDirDlg(elemParam);
        return ret;
    }
    else
    if (type == "BumpTexture")
    {
        nString ret;

        // Caution: The plugin max script code is always regenerated and
        //          executed at the start time of 3dsmax so it is useless to 
        //          specifying any initial value to a control or parameter block.
        ret += AddMapButton(elemParam);
        ret += AddSetDirDlg(elemParam);
        return ret;
    }
    else
    if (type == "CubeTexture")
    {
        nString ret;

        // Caution: The plugin max script code is always regenerated and
        //          executed at the start time of 3dsmax so it is useless to 
        //          specifying any initial value to a control or parameter block.
        ret += AddMapButton(elemParam);
        ret += AddSetDirDlg(elemParam);
        return ret;
    }
    else
    if (type == "Vector")
        return AddVector4Spinner(elemParam);
    else
    if (type == "EnvelopeCurve" || type == "ColorEnvelopeCurve")
        return AddEnvelopeCurve(shdName, elemParam);
    else
    {
        nString uiScript;
        uiScript += "\t\t";
        uiScript += "label Unknown \"Unknown\"\n";

        return uiScript;
    }
}

//-----------------------------------------------------------------------------
/**
    The followinig maxscript is generated:
    @verbatim
    param = "" 
    param += (val.r/255.0) as string 
    param += " "
    param += (val.g/255.0) as string
    param += " "
    param += (val.b/255.0) as string
    param += " "
    param += (val.a/255.0) as string
    param += " "
    nChangeShaderParameter "Standard" "common" "MatSpecular" param
    @endverbatim
*/
static
nString GetStringForColorPicker(const nString &shdName, const nString &type, const nString &paramName)
{
    nString str;

    nString indent = "\t\t\t\t";

    str += indent + "param = \"\" \n";
    str += indent + "param += (val.r/255.0) as string \n";
    str += indent + "param += \" \"\n";
    str += indent + "param += (val.g/255.0) as string\n";
    str += indent + "param += \" \"\n";
    str += indent + "param += (val.b/255.0) as string\n";
    str += indent + "param += \" \"\n";
    str += indent + "param += (val.a/255.0) as string\n";
    str += indent + "param += \" \"\n";

    str += indent + "nChangeShaderParameter ";
    str += "\""; str += shdName; str += "\""; //e.g. "Standard"
    str += " ";
    str += "\""; str += type; str += "\"";
    str += " ";
    str += "\""; str += paramName; str += "\""; // e.g. "MatDiffuse"
    str += " ";
    str += "param\n";

    return str;
}

//-----------------------------------------------------------------------------
/**
    The followinig maxscript is generated:
    @verbatim
    nChangeShaderParameter "Standard" "common" "DiffMap0" val.filename
    @endverbatim
*/
static
nString GetStringForMapButton(const nString &shdName, const nString &type, const nString &paramName)
{
    nString str;

    str += "\t\t\t\tnChangeShaderParameter ";
    str += "\""; str += shdName; str += "\"";
    str += " ";
    str += "\""; str += type; str += "\"";
    str += " ";
    str += "\""; str += paramName; str += "\"";
    str += " ";
    str += "val.filename";
    str += "\n";

    return str;
}

//-----------------------------------------------------------------------------
/**
    The followinig maxscript is generated:
    @verbatim
    param = "" 
    param += val as string 
    nChangeShaderParameter "Standard" "common" "MatEmissiveIntensity" param
    @endverbatim
*/
static
nString GetStringForDefault(const nString &shdName, const nString &type, const nString &paramName)
{
    nString str;

    nString indent = "\t\t\t\t";

    str += indent + "param = \"\" \n";
    str += indent + "param += val as string \n";

    str += "\t\t\t\tnChangeShaderParameter ";
    str += "\""; str += shdName; str += "\"";
    str += " ";
    str += "\""; str += type; str += "\"";
    str += " ";
    str += "\""; str += paramName; str += "\"";
    str += " ";
    str += "param";
    str += "\n";

    return str;
}

//-----------------------------------------------------------------------------
/**
    Generate max script code for a event which occurred a value of a control is
    changed.

    The following maxscript is one of an example which might be generated:
    @verbatim
    on RenderPri changed val do 
    (
        nChangeShaderParameter "Standard", "common", "RenderPri", "1.0 1.0 1.0 1.0"
    )
    @endverbatim

    -17-Aug-06  kims Added color picker to support MatAmbient.
*/
static
nString GetIpcEventHandler(const nString &shdName, const nString &paramName)
{
    nString handler;

    handler += "\t\t\tif nIsConnectedIpc() do\n";
    handler += "\t\t\t(\n";

    //color picker
    if (paramName == "MatDiffuse"  ||
        paramName == "MatEmissive" ||
        paramName == "MatSpecular" ||
        paramName == "MatAmbient") 
    {
        handler += GetStringForColorPicker(shdName, "common", paramName);
    }
    else
    if (paramName == "MatEmissiveIntensity" ||
        paramName == "MatSpecularPower"     ||
        paramName == "BumpScale")
    {
        handler += GetStringForDefault(shdName, "common", paramName);
    }
    else
    if (paramName == "AlphaSrcBlend" ||
        paramName == "AlphaDstBlend")
    {
        handler += GetStringForDefault(shdName, "common", paramName);
    }
    //texture map
    else
    if (strstr(paramName.Get(), "DiffMap") ||
        strstr(paramName.Get(), "BumpMap") ||
        strstr(paramName.Get(), "CubeMap") )
    {
        handler += GetStringForMapButton(shdName, "common", paramName);
    }
    else
    {
        // print "Unknown material type: shader name: xxx parameter name: yyy"
        handler += "\t\t\t\tprint ";
        handler += "\"Unknown material type: shader name: ";
        handler += shdName.Get();
        handler += " ";
        handler += "parameter name: ";
        handler += paramName.Get();
        handler += "\"\n";
    }

    handler += "\t\t\t)\n";

    return handler;
}

//-----------------------------------------------------------------------------
/**
    Retrieves event handler for each parameters.

    @note
        Because we use custom attribute for plugin's parameters,
        we cannot directly access its parameters of superclass with 'delegate'
        local variable. Instead of doing that, we get current material in the
        material editor then access its delegate like this:
    @verbatim
    on MapAmbient set val do
    (
        -- get the current material in the material editor.
        -- this is the material which currently selected material in the slot.
        curMaterial = medit.GetCurMtl()

        -- now, we can access the ambient value of the superclass.
        curMaterial.delegate.ambient = val
    )
    @endverbatim

    @param paramName parameter name which in parameter block.
    @return string which to be appended for parameter's event handler.
*/
static
nString GetEventHandler(const nString &shdName, const nString &paramName)
{
    bool validParam = true;
    nString handler;

    handler += "\t\ton ";
    handler += paramName;
    handler += " set val do \n";
    handler += "\t\t(\n";
    nShaderState::Param param = nShaderState::StringToParam(paramName.Get());
    switch(param)
    {
    case nShaderState::MatAmbient:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    case nShaderState::MatDiffuse:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    case nShaderState::MatSpecular:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    case nShaderState::MatEmissive:
        break;
    case nShaderState::MatEmissiveIntensity:
        break;
    case nShaderState::MatSpecularPower:
        break;
    case nShaderState::AlphaSrcBlend:
    case nShaderState::AlphaDstBlend:
        break;

    case nShaderState::DiffMap0:
    case nShaderState::DiffMap1:
    case nShaderState::DiffMap2:
    case nShaderState::DiffMap3:
    case nShaderState::DiffMap4:
    case nShaderState::DiffMap5:
    case nShaderState::DiffMap6:
    case nShaderState::DiffMap7:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    case nShaderState::BumpMap0:
    case nShaderState::BumpMap1:
    case nShaderState::BumpMap2:
    case nShaderState::BumpMap3:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    case nShaderState::CubeMap0:
    case nShaderState::CubeMap1:
    case nShaderState::CubeMap2:
    case nShaderState::CubeMap3:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    case nShaderState::SpecMap0:
    case nShaderState::SpecMap1:
    case nShaderState::SpecMap2:
    case nShaderState::SpecMap3:
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        break;

    default:
    case nShaderState::InvalidParameter:
        // put empty handler.
        validParam = false;
        break;
    }

    if (validParam)
    {
        handler += GetIpcEventHandler(shdName, paramName);
    }

    handler += "\t\t) \n";

    return handler;
}

//-----------------------------------------------------------------------------
/**
    Generate script plug-in script.(part of 'param' element in xml)
    
    -02-Nov-06  kims  Changd to 'Vector' type correctly. Thank ZHANG Zikai for the patch.
                      
    @param name 'name' attribute in param element
                 The followings are known names:
                 'name', 'label', 'type', 'gui', 'export', 'min', 'max', 'def', 'enum'
*/
static
void GenerateScript(TiXmlElement* elemParam, nString& shdName,
                    nString& strParamBlock, nString &strRollout)
{
    // start to generate script for param block clause.

    // parameter name in parameter block.
    nString paramName = elemParam->Attribute("name");
    strParamBlock += "\t\t";
    strParamBlock += paramName;
    strParamBlock += " ";

    // parameter type
    nString paramType = elemParam->Attribute("type");
    strParamBlock += "type:";
    strParamBlock += GetParameterType(paramType);
    strParamBlock += " ";

    // if the type is one of any texture types, specifies false to this parameter.
    // default setting is 'true' on animatable parameter.
    if (strstr(paramType.Get(), "Texture"))
    {
        //strParamBlock += "animatable:false";
        strParamBlock += "animatable:true";
        strParamBlock += " ";
    }

    // specify default value of the parameter.
    if (paramType == "Vector" || 
       (paramType == "EnvelopeCurve" || paramType == "ColorEnvelopeCurve"))
    {
        // we don't need default values for 'vector' and 'envelopecurve' type 
        // those are types uses floattab.
        ;
    }
    else
    {
        nString defaultValue = GetDefault(elemParam);
        if (!defaultValue.IsEmpty())
        {
            strParamBlock += "default:";
            strParamBlock += defaultValue;
            strParamBlock += " ";
        }
    }

    // if there's gui, we create ui in the given rollout.
    int hasGui;
    elemParam->Attribute("gui", &hasGui);
    if (hasGui)
    {
        // we do not combine ui if the parameter is envelopecurve or colorenvelopecurve due to 
        // it is hard to bind those control type with parameters block.
        if (!(paramType == "EnvelopeCurve" || paramType == "ColorEnvelopeCurve"))
        {
            // 'ui' name should be same as parameter name.
            strParamBlock += "ui:";
            // combine the 4 spinner into a parameter
            if (paramType == "Vector")
            {
                // names should be same as the spinners
                // see AddVector4Spinner
                nString name = paramName;
                strParamBlock += "(";
                name.AppendInt(0);
                strParamBlock += name;
                strParamBlock += ", ";
                name.AppendInt(1);
                strParamBlock += name;
                strParamBlock += ", ";
                name.AppendInt(2);
                strParamBlock += name;
                strParamBlock += ", ";
                name.AppendInt(3);
                strParamBlock += name;
                strParamBlock += ")";
            }
            else
            {
                strParamBlock += paramName;
            }
        }
        strParamBlock += " \n";

        strRollout += GetUIFromType(elemParam, shdName, paramType);
    }

    // append event handler.
    strParamBlock += GetEventHandler (shdName, paramName);

    // if it is texture type, add destination directory setting value.
    if (strstr(paramType.Get(), "Texture") && hasGui)
    {
        //HACK: 'dirSetting' is used again in AddSetDirDlg() function.
        //      and nMaxMaterial::GetNebulaMaterial() function.
        //      So, if you change the string, the string in AddSetDirDlg() 
        //      also should be changed.
        nString prefix = "dirSetting";

        strParamBlock += "\t\t";
        strParamBlock += prefix;
        strParamBlock += paramName;
        strParamBlock += " ";

        strParamBlock += "type:#string ";

        // The directory parameter has "" for default string. It is absolutely necessary in Max6.
        // Without that, the exporter is not usable as the panels that have those controls in them don't work.
        strParamBlock += "default:\"\" ";

        strParamBlock += "ui:";
        strParamBlock += "edtFld";
        strParamBlock += paramName;
        strParamBlock += "\n";
    }
}

//-----------------------------------------------------------------------------
/**
    Parse each of the elements in xml and generate parameter block and rollout.

    @param elemShader element of the given xml
    @param strParamBlock parameter block script which to be generated.
    @param strRollout rollout script which to be generated.
*/
static
void ParseParams(TiXmlElement* elemShader, nString& shdName, nString& strParamBlock, nString &strRollout)
{
    nString shdType = elemShader->Attribute("shaderType");
    nString meshType = elemShader->Attribute("meshType");

    // Get .shader name.
    nString effectFileName = elemShader->Attribute("file");

    // 'Shader' parameter in parameter block.
    strParamBlock += "\t\t";
    strParamBlock += "Shader type:#string ";
    strParamBlock += "default:\"";
    strParamBlock += effectFileName;
    strParamBlock += "\" animatable:false";
    strParamBlock += "\n";

    TiXmlElement* elemParam = elemShader->FirstChild("param")->ToElement();
    for (elemParam; elemParam; elemParam = elemParam->NextSiblingElement())
    {
        GenerateScript(elemParam, shdName, strParamBlock, strRollout);
    }
}

//-----------------------------------------------------------------------------
/**
    Filter of the shader in filter array.
*/
static
bool DoFilter(const nString &str)
{
    for (int i=0; i<shaderFilterArray.Size(); i++)
    {
        if (shaderFilterArray[i] == str)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Add script plugin event handlers.
*/
static
nString AddPluginEventHandlers()
{
    nString script;

    // 'create' plugin event handlers
    script += "\ton create do\n";
    script += "\t(\n";
    script += "\t\tOnPluginCreate()\n";
    script += "\t)\n";

    // 'postCreate' plugin event handlers
    script += "\ton postCreate do\n";
    script += "\t(\n";
    script += "\t\tOnPluginPostCreate()\n";
    script += "\t)\n";

    // 'load' plugin event handlers
    script += "\ton load do\n";
    script += "\t(\n";
    script += "\t\tOnPluginLoad()\n";
    script += "\t)\n";

    // 'postLoad' plugin event handlers
    script += "\ton postLoad do\n";
    script += "\t(\n";
    script += "\t\tOnPluginPostLoad()\n";
    script += "\t)\n";

    return script;
}

//-----------------------------------------------------------------------------
/**
    Retrieve all shader names from xml file (e.g. shader.xml)
*/
TiXmlHandle GetShaderFromDataBase(TiXmlHandle& xmlHandle, const nString &shader)
{
    // retrieves all shader names in xml file.
    TiXmlElement* child = xmlHandle.FirstChild("NebulaShaderDatabase").FirstChild("shader").Element();
    for (child; child; child=child->NextSiblingElement())
    {
        // get shader name.
        nString name = child->Attribute("file");
        if (name == shader)
        {
            return child;
        }
    }
    return TiXmlHandle(0);
}

//-----------------------------------------------------------------------------
/**
    Retrieve parameter from given shader.
*/
TiXmlHandle GetParamFromShader(TiXmlHandle& shader, const nString &param)
{
    TiXmlElement* child = shader.FirstChild("param").Element();
    for (child; child; child=child->NextSiblingElement())
    {
        nString name = child->Attribute("name");
        if (name == param)
            return TiXmlHandle(child);
    }
    return TiXmlHandle(0);
}

//-----------------------------------------------------------------------------
/**
    Retrieve shader name which to be filtered out.
    The shader names are listed in the filter.xml file which can be found 
    under the same directory with shader.xml file.
*/
bool GetShaderFilter()
{
    // Get the full path of '$nebula/data/shaders/filter.xml' file.
    nString filterXmlFilePath = GetShaderXmlPath("filter.xml");
    if (filterXmlFilePath.IsEmpty())
    {
        n_listener("Cannot find filter.xml file.\n");
        return false;
    }

    TiXmlDocument xmlDoc;

    // Load the shaders.xml file
    if (!xmlDoc.LoadFile(filterXmlFilePath.Get()))
    {
        n_listener("Filed to load %s.", filterXmlFilePath.Get());
        return false;
    }

    TiXmlHandle xmlHandle(&xmlDoc);
    TiXmlElement* child;

    shaderFilterArray.Clear();

    child = xmlHandle.FirstChild("NebulaShaderFilter").FirstChild("shader").Element();
    for (child; child; child=child->NextSiblingElement())
    {
        // get shader name.
        nString name = child->Attribute("name");

        shaderFilterArray.Append(name);
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    max script expose function which to generate material script plugin based on 
    '$nebula2/data/shaders/shaders.xml' file.

    The following max script generate nebula2 custom material plugin. 
    @verbatim
    nCreateCustomMaterialPlugin() 
    @endverbatim

    @note
    Do not call this script explicitly when 3DS Max running.
    This function should be called at once when 3DS Max start up.
    If not, all previously specified materials in the slot will be reset.

    The generated file, "n2materialplugin.ms" can be found in the 
    '$3dsmax/scripts/' directory. Use it for the purpose of test or debugging.
    It is only available on the debug version of plugin.

    @return true if we success to generate scripted plug-in.
*/
bool EvalCustomMaterialPlugin()
{
    // Get shader name which to be filted out.
    if (!GetShaderFilter())
    {
        n_listener("Cannot find filter.xml file.\n");
        shaderFilterArray.Clear();
    }

    // Get the full path of '$nebula/data/shaders/shaders.xml' file.
    nString shdXmlFilePath = GetShaderXmlPath(nString("shaders.xml"));
    if (shdXmlFilePath.IsEmpty())
    {
        n_listener("Cannot find shaders.xml file.\n");
        return false;
    }

    TiXmlDocument xmlDoc;

    // Load the shaders.xml file
    if (!xmlDoc.LoadFile(shdXmlFilePath.Get()))
    {
        n_listener("Filed to load %s.", shdXmlFilePath.Get());
        return false;
    }

    TiXmlHandle xmlHandle(&xmlDoc);

    TiXmlElement* child;

    // array for shader name of shader element.
    nArray<nString> shaderArray;
    shaderArray.Append("None");
   
    nArray<nString> custAttribNameArray;
    custAttribNameArray.Append("undefined");

    nArray<nString> effectFileArray;
    effectFileArray.Append("'None'");

    // array for containing generated custom attribute script.
    nArray<nString> custAttribArray;

    // retrieves all shader names in xml file.
    child = xmlHandle.FirstChild("NebulaShaderDatabase").FirstChild("shader").Element();
    for (child; child; child=child->NextSiblingElement())
    {
        // get shader name.
        nString name = child->Attribute("name");

        // if the given shader name is found in the shader filter,
        // the plugin does not generate script code for the shader.
        if (DoFilter(name))
            continue;

		nString caName = name;

        // get shader name.
        nString effectFile;
        effectFile += "'";
        effectFile += child->Attribute("file");
        effectFile += "'";
        effectFileArray.Append(effectFile);

        shaderArray.Append(name);

        nString custattrib, custattribName;
        
        // we append 'ca' prefix to avoid name confliction.
        // e.g. "Standard' and "Ocean' are already defined keyword in max script.
        custattribName +="ca";
        custattribName += nMaxUtil::CorrectName(name);
        custAttribNameArray.Append(custattribName);

        custattrib += custattribName;
        custattrib += " = ";
        custattrib += "attributes";
        custattrib += " ";
        custattrib += "\"";
        custattrib += caName;//name;
        custattrib += "\"";
        custattrib += "\n";
        custattrib += "(\n";

        // parameter block and rollout clause.
        nString paramBlock, rollout;

        nString shdName = nMaxUtil::CorrectName(name);

        nString rolloutName = "r" + shdName; //e.g. "rStandard"

        paramBlock += "\t";
        paramBlock += "parameters ";
        paramBlock += shdName;//nMaxUtil::CorrectName(shdname);
        paramBlock += " ";
        paramBlock += "rollout:";
        paramBlock += rolloutName;
        paramBlock += "\n";
        paramBlock += "\t(\n";

        // begin rollout
        rollout += "\t";
        rollout += "rollout ";
        rollout += rolloutName;
        rollout += " ";

        //specify rollout name.
        rollout += "\"";
        rollout += name;
        rollout += " ";
        rollout += "Parameters";
        rollout += "\"";
        rollout += "\n";

        rollout += "\t(\n";

        // parse each param elements in shader element.
        ParseParams(child, caName, paramBlock, rollout);

        paramBlock += "\t)\n";
        rollout += "\t)\n";;

        custattrib += paramBlock;
        custattrib += rollout;

        custattrib += "\n)\n";

        custAttribArray.Append(custattrib);
    }

    int i;
    nString script;

    script += "--------------------------------------------------------------------------------\n";
    script += "--  The Nebula2 3dsmax Toolkit MaxScript code \n";
    script += "--  This file was automatically generated and executed when 3dsmax started. \n";
    script += "--------------------------------------------------------------------------------\n";

    // define 'materialhandlers.ms' including script.
    script += "include \"nebula2\\materialhandlers.ms\"\n\n";

    // add custom attributes.
    for (i=0; i<custAttribArray.Size(); i++)
    {
        script += custAttribArray[i];
        script += "\n";
    }

    // script plugin header
    const int version = 100;

    nString strVer;
    strVer.AppendInt(version);

    script += "plugin material Nebula2Material\n";
    script += "\tname:\"Nebula2\"";
    script += "\n";
    script += "\tclassID:#(";
    script += "0x6b5ae640";
    script += ", ";
    script += "0x319f53b8";
    script += ")\n";
    script += "\textends:Standard\n";
    script += "\treplaceUI:true\n";
    script += "\tversion:";
    script += strVer; // version
    script += "\n(\n";

    // script plugin body.

    // local variable : array of customAttributes
    script += "\tlocal customAttributes = #(";
    for (i=0; i<custAttribNameArray.Size(); i++)
    {
        script += custAttribNameArray[i];

        if (i < custAttribNameArray.Size() - 1)
            script += ",";
    }
    script += ")\n";

    // local variable : array of shader types
    script += "\tlocal effectFileNames = #(";
    for (i=0; i<effectFileArray.Size(); i++)
    {
        script += "\"";
        script += effectFileArray[i];
        script += "\"";
        if (i < effectFileArray.Size() - 1)
            script += ",";
    }
    script += ")\n";

    // parameter block
    script += "\tparameters SelectShader rollout:main \n"; 
    script += "\t(\n"; 
    script += "\t\tparamCurShader type:#integer default:1 animatable:false \n"; 
    script += "\t\tparamEffectFile type:#string default:\"'None'\" animatable:false ui:effectFileName \n"; 
    script += "\t)\n";

    nString mainRollout;

    mainRollout += "\trollout main ";
    mainRollout += "\"Nebula2 Shader Types\"\n";
    mainRollout += "\t(\n";

    // insert dropdownlist script for shader selection to rollout clause.
    nString uiShdName = "Shader";
    mainRollout += "\t\t";
    mainRollout += "dropdownlist ";
    mainRollout += uiShdName;
    mainRollout += " ";

    mainRollout += "\"Select Shader\"";
    mainRollout += " ";

    mainRollout += "width:160";
    mainRollout += " ";

    mainRollout += "items:#(";
    for (i=0; i<shaderArray.Size(); i++)
    {
        mainRollout += "\"";
        mainRollout += shaderArray[i];
        mainRollout += "\"";

        if (i < shaderArray.Size() - 1)
            mainRollout += ",";
    }
    mainRollout += ") ";
    mainRollout += "selection:1";
    mainRollout += "\n";
    // end of dropdownlist script for shader.

    mainRollout += "\t\tlabel effectFileName \"Effect File: 'None'\" align:#left \n";

    mainRollout += "\t\ton " + uiShdName + " " + "selected i do \n";
    mainRollout += "\t\t(\n";
    mainRollout += "\t\t\tcurMaterial = medit.GetCurMtl() \n";

    mainRollout += "\t\t\tif customAttributes[paramCurShader] != undefine do \n";
    mainRollout += "\t\t\t(\n";
    mainRollout += "\t\t\t\tnumCustAttr = custAttributes.count curMaterial \n";
    mainRollout += "\t\t\t\tfor i = 1 to numCustAttr do\n";
    mainRollout += "\t\t\t\t(\n";
    mainRollout += "\t\t\t\t\tattrDef = custAttributes.getdef curMaterial i \n";
    mainRollout += "\t\t\t\t\tif attrDef != undefined and attrDef.name == customAttributes[paramCurShader].name do \n";
    mainRollout += "\t\t\t\t\t(\n";
    mainRollout += "\t\t\t\t\t\tcustAttributes.delete curMaterial i \n";
    mainRollout += "\t\t\t\t\t)\n";
    mainRollout += "\t\t\t\t)\n"; // end of for
    mainRollout += "\t\t\t)\n";

    mainRollout += "\t\t\tif customAttributes[i] != undefined then \n";
    mainRollout += "\t\t\t(\n";
    mainRollout += "\t\t\t\tcustAttributes.add curMaterial customAttributes[i] \n";
    mainRollout += "\t\t\t)\n";
    mainRollout += "\t\t\tShader.selection = i \n";
    mainRollout += "\t\t\tparamCurShader = i \n";
    mainRollout += "\t\t\teffectFileName.caption = \"Effect File: \" + effectFileNames[i] \n";
    mainRollout += "\t\t\tparamEffectFile = effectFileNames[i]\n";
    mainRollout += "\t\t)\n";

    mainRollout += "\t\ton main open do \n";
    mainRollout += "\t\t(\n";
    mainRollout += "\t\t\tShader.selection = paramCurShader \n";
    mainRollout += "\t\t\teffectFileName.caption = \"Effect File: \" + paramEffectFile\n";
    mainRollout += "\t\t)\n";

    mainRollout += "\t)\n";

    script += mainRollout;

    script += AddPluginEventHandlers();

    script += "\n)";

    nString scriptsPath;
    scriptsPath += GetCOREInterface()->GetDir(APP_SCRIPTS_DIR);
#ifdef _DEBUG
    nString sampleFile;
    sampleFile += scriptsPath;
    sampleFile += "\\";
    sampleFile += "n2materialplugin.ms";

    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open(sampleFile.Get(), "w"))
    {
        file->PutS(script);
        file->Close();
        file->Release();
    }
#endif

    // execute the generated script.
    if (!nMaxScriptCall(script.Get()))
    {
        n_listener("Failed to evaluate the generated script.\n");
        n_listener("See '$3dsmax/scripts/materialplugintemp.ms' for generated script.\n");

        // if we failed to evaluate the given script, put it to 3dsmax script directory

        nString debugFilePath;
        debugFilePath += scriptsPath;
        debugFilePath += "\\";
        debugFilePath += "materialplugintemp.ms";

        nFile* file = nFileServer2::Instance()->NewFileObject();
        if (file->Open(debugFilePath.Get(), "w"))
        {
            file->PutS(script);
            file->Close();
            file->Release();
        }

        // open 3dsmax script editor with the script which failed to be evaluated
        // for the purpose of debugging it.
        n_openeditor(debugFilePath.Get());

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Retrieve default value which is defined in shader database file(shader.xml)
    It is needed when the toolkit set default value if the parameter has not any value.

    e.g) The toolkit specifies "nobump.dds" if there is no bump map is specified 
    in the material editor when it exports.
*/
bool GetDefaultValueFromDataBase(const nString &shader, const nString &param, nString &outvalue)
{
    // Get the full path of '$nebula/data/shaders/shaders.xml' file.
    nString shdXmlFilePath = GetShaderXmlPath("shaders.xml");
    if (shdXmlFilePath.IsEmpty())
    {
        n_listener("Cannot find shaders.xml file.\n");
        return false;
    }

    TiXmlDocument xmlDoc;

    // Load the shaders.xml file
    if (!xmlDoc.LoadFile(shdXmlFilePath.Get()))
    {
        n_listener("Filed to load %s.", shdXmlFilePath.Get());
        return false;
    }

    TiXmlHandle xmlHandle(&xmlDoc);
    TiXmlElement* pElem = GetParamFromShader(GetShaderFromDataBase(xmlHandle, shader), param).Element();
    if (pElem == 0)
        return false;

    const char *attr = pElem->Attribute("def");
    if (attr == 0)
        return false;
    
    outvalue = attr;

    return true;
}
