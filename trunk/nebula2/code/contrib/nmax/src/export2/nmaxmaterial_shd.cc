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

//-----------------------------------------------------------------------------
/**
    Find the full path of shaders.xml file.

    The reason we do not directly call nMaxOption::GetHomePath() is that 
    nMaxOption is mostly used for exporting. 
    It is singleton class so calling that here makes hard to handle and destroy 
    its instance.
*/
static
nString GetShaderXmlPath()
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
    shdxml += "data\\shaders\\shaders.xml";

    if (!nFileServer2::Instance()->FileExists(shdxml.Get()))
    {
        //n_listener("File %s does not exist.\n", shdxml.Get());
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
#if MAX_RELEASE >= 6000
        return "#point4";
#else
        return "#floatTab";
#endif
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
        return "<<unknown>>";
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
      -# Unknown - label

    @param type 'type' value of xml element.

    -17-Aug-06  kims Changed to add texutre directory setting button.
*/
static
nString GetUIFromType(TiXmlElement* elemParam, const nString &type)
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
        return "label Unknown \"Unknown\"\n";
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
    nString handler;

    handler += "\t\ton ";
    handler += paramName;
    handler += " set val do \n";
    handler += "\t\t(\n";

    if (paramName == "MatAmbient")
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "\t\t\tcurMaterial.delegate.ambient = val\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (paramName == "MatDiffuse")
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "\t\t\tcurMaterial.delegate.diffuse = val\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (paramName == "MatSpecular")
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "\t\t\tcurMaterial.delegate.specular = val\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (paramName == "MatEmissive")
    {
        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (paramName == "MatEmissiveIntensity")
    {
        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (paramName == "MatSpecularPower")
    {
        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (paramName == "AlphaSrcBlend" ||
        paramName == "AlphaDstBlend")
    {
        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (strstr(paramName.Get(), "DiffMap"))
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";

        //FIXME: should assign map channel to delegate.

        // standard material support only one map channel.

        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "\t\t\tcurMaterial.delegate.diffuseMap = val\n";
        //handler += "\t\t\tif curMaterial.delegate.diffuseMap != undefined do\n";
        //handler += "\t\t\t\tcurMaterial.delegate.diffuseMapEnable = true\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (strstr(paramName.Get(), "BumpMap"))
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        
        //FIXME: should assign map channel to delegate.

        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "\t\t\tcurMaterial.delegate.bumpMap = val\n";
        //handler += "\t\t\tif curMaterial.delegate.bumpMap != undefined do\n";
        //handler += "\t\t\t\tcurMaterial.delegate.bumpMapEnable = true\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (strstr(paramName.Get(), "CubeMap"))
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";

        //FIXME: should assign map channel to delegate.

        //FIXME: need find proper way to hanlde cubemap in 3dsmax.
        //       cubemap is only available when 3dsmax uses hardware shader.
        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "delegate.diffuseMap = val\n";
        //handler += "if delegate.diffuseMap != undefined do\n";
        //handler += "\t\t\t\t";
        //handler += "delegate.diffuseMapEnable = true\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    if (strstr(paramName.Get(), "SpecMap"))
    {
        handler += "\t\t\tOn" + paramName + "Changed val\n";
        //FIXME: should assign map channel to delegate.

        // specular map.
        //handler += "\t\t\tcurMaterial = medit.GetCurMtl()\n";
        //handler += "\t\t\tcurMaterial.delegate.specularMap  = val\n";
        //handler += "\t\t\tif curMaterial.delegate.specularMap != undefined do\n";
        //handler += "\t\t\t\tcurMaterial.delegate.specularMapEnable = true\n";

        handler += GetIpcEventHandler(shdName, paramName);
    }
    else
    {
        // put empty handler.
        ;
    }

    handler += "\t\t) \n";

    return handler;
}

//-----------------------------------------------------------------------------
/**
    Generate script plug-in script.(part of 'param' element in xml)
    
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
    nString defaultValue = GetDefault(elemParam);
    if (!defaultValue.IsEmpty())
    {
        strParamBlock += "default:";
        strParamBlock += defaultValue;
        strParamBlock += " ";
    }

    // if there's gui, we create ui in the given rollout.
    int hasGui;
    elemParam->Attribute("gui", &hasGui);
    if (hasGui)
    {
        // 'ui' name should be same as parameter name.
        strParamBlock += "ui:";
        strParamBlock += paramName;
        strParamBlock += " \n";

        strRollout += GetUIFromType(elemParam, paramType);
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

//FIXME: filtering mechanism is temporal at the moment.
//       some shaders like particle does not fit well with 3dsmax.
//       the particle shader needs curved UI controls like enveloped color etc.
//       using curved UI control does not difficult but place it to
//       material editor does not seems to be a right way.
//       So, just filter it up until we find the proper way.

// fileter array.
const int FILETER_SIZE = 3;
static const char* filter[FILETER_SIZE] = {
    "Particle",
    "Particle (Additive)",
    "Particle (Alpha Blended)"
};

//-----------------------------------------------------------------------------
/**
    Filter of the shader in filter array.
*/
static
bool DoFilter(const nString &str)
{
    for (int i=0; i<FILETER_SIZE; i++)
    {
        if (strcmp(str.Get(), filter[i]) == 0)
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
    // Get the full path of '$nebula/data/shaders/shaders.xml' file.
    nString shdXmlFilePath = GetShaderXmlPath();
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
    mainRollout += "\t\t\t\tcustAttributes.delete curMaterial customAttributes[paramCurShader] \n";
    mainRollout += "\t\t\t)\n";
    mainRollout += "\t\t\tif customAttributes[i] != undefined then \n";
    mainRollout += "\t\t\t(\n";
    mainRollout += "\t\t\t\tcustAttributes.add curMaterial customAttributes[i] \n";
    mainRollout += "\t\t\t)\n";
    mainRollout += "\t\t\tShader.selection = i \n";
    mainRollout += "\t\t\tparamCurShader = i \n";
    mainRollout += "\t\t\teffectFileName.caption = \"Effect Fils: \" + effectFileNames[i] \n";
    mainRollout += "\t\t\tparamEffectFile = effectFileNames[i]\n";
    mainRollout += "\t\t)\n";

    mainRollout += "\t\ton main open do \n";
    mainRollout += "\t\t(\n";
    mainRollout += "\t\t\tShader.selection = paramCurShader \n";
    mainRollout += "\t\t\teffectFileName.caption = \"Effect Fils: \" + paramEffectFile\n";
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
        file->PutS(script.Get());
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
            file->PutS(script.Get());
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
