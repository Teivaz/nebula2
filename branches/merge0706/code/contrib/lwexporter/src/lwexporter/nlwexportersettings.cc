//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwexportersettings.h"
#include "tinyxml/tinyxml.h"
#include "lwwrapper/nlwglobals.h"
#include "lwexporter/nlwexportnodetype.h"

nLWExporterSettings* nLWExporterSettings::singleton = 0;
const char nLWExporterSettings::SETTINGS_FILENAME[] = "N2LightwaveExporter.xml";

namespace
{
    //----------------------------------------------------------------------------
    /**
        @brief Convert a type/value string pair to an nArg.
        @return true if the conversion was successful, false otherwise.
    */
    bool
    StringToArg(const nString& argTypeStr, const nString& argValStr, nArg& arg)
    {
        arg.Delete();

        if ("Bool" == argTypeStr)
        {
            arg.SetB((argValStr.AsInt() == 0) ? false : true);
            return true;
        }
        else if ("Int" == argTypeStr)
        {
            arg.SetI(argValStr.AsInt());
            return true;
        }
        else if ("Float" == argTypeStr)
        {
            arg.SetF(argValStr.AsFloat());
            return true;
        }
        else if ("String" == argTypeStr)
        {
            arg.SetS(argValStr.Get());
            return true;
        }
        else
        {
            return false;
        }
    }

    //----------------------------------------------------------------------------
    /**
        @brief Convert an nArg to a type/value string pair.
        @return true if the conversion was successful, false otherwise.
    */
    bool
    ArgToString(const nArg& arg, nString& argTypeStr, nString& argValStr)
    {
        switch (arg.GetType())
        {
            case nArg::Bool:
                argTypeStr = "Bool";
                argValStr.SetInt(arg.GetB() ? 1 : 0);
                return true;

            case nArg::Int:
                argTypeStr = "Int";
                argValStr.SetInt(arg.GetI());
                return true;

            case nArg::Float:
                argTypeStr = "Float";
                argValStr.SetFloat(arg.GetF());
                return true;

            case nArg::String:
                argTypeStr = "String";
                argValStr = arg.GetS();
                return true;
        }
        return false;
    }
} // anonymous namespace

namespace GeneralSettings
{
    enum GeneralSetting
    {
        GeometryScale = 0,
        ScriptServer,
        PreviewRenderer,
        PreviewWidth,
        PreviewHeight,
        PreviewFullScreen,
        OutputBinarySceneNode,
        OutputBinaryMesh,
        OutputBinaryAnimation,
    };

    const char* const SETTING_NAME_ARRAY[] = {
        "geometryScale",
        "scriptServer",
        "previewRenderer",
        "previewWidth",
        "previewHeight",
        "previewFullScreen",
        "outputBinarySceneNode",
        "outputBinaryMesh",
        "outputBinaryAnimation"
    };

    const int SETTING_NAME_ARRAY_SIZE = sizeof(SETTING_NAME_ARRAY) / sizeof(const char*);

    //----------------------------------------------------------------------------
    /**
    */
    int
    SettingNameToArrayIndex(const nString& settingName)
    {
        for (int i = 0; i < SETTING_NAME_ARRAY_SIZE; i++)
        {
            if (SETTING_NAME_ARRAY[i] == settingName)
            {
                return i;
            }
        }
        return -1; // setting not found
    }

} // namespace GeneralSettings

//----------------------------------------------------------------------------
/**
*/
nLWExporterSettings::nLWExporterSettings() :
    settingsArray(0, 0)
{
    nLWExporterSettings::singleton = this;

    this->settingsArray.SetFixedSize(GeneralSettings::SETTING_NAME_ARRAY_SIZE);
    this->SetDefaults();
}

//----------------------------------------------------------------------------
/**
*/
nLWExporterSettings::~nLWExporterSettings()
{
    this->sceneNodeTypesArray.Clear();
    this->ClearExportNodeTypes();
    this->exportNodeTypesArray.Clear();
    this->settingsArray.Clear();
}

//----------------------------------------------------------------------------
/**
*/
nLWExporterSettings*
nLWExporterSettings::Instance()
{
    if (!nLWExporterSettings::singleton)
    {
        n_new(nLWExporterSettings);
    }
    n_assert(nLWExporterSettings::singleton);
    return nLWExporterSettings::singleton;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::FreeInstance()
{
    if (nLWExporterSettings::singleton)
    {
        n_delete(nLWExporterSettings::singleton);
        nLWExporterSettings::singleton = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetDefaults()
{
    this->SetGeometryScale(1.0f);
    this->SetScriptServer("ntclserver");
    this->SetPreviewRenderer("Default");
    this->SetPreviewWidth(640);
    this->SetPreviewHeight(480);
    this->SetPreviewFullScreen(false);
    this->SetOutputBinarySceneNode(false);
    this->SetOutputBinaryMesh(false);
    this->SetOutputBinaryAnimation(false);
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::Load()
{
    // the settings are stored in the Lightwave content directory
    nLWGlobals::DirInfoFunc dirInfo;
    nString settingsFileName(dirInfo.GetContentDir());
    settingsFileName.StripTrailingSlash();
    settingsFileName += "\\";
    settingsFileName += nLWExporterSettings::SETTINGS_FILENAME;

    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(settingsFileName.Get()))
    {
        n_printf("Failed to load %s.", settingsFileName.Get());
        return;
    }

    TiXmlHandle xmlhSettings = xmlDoc.FirstChild("NebulaExporterSettings");

    // load general settings
    TiXmlElement* settingElem = 0;
    settingElem = xmlhSettings.FirstChild("General").FirstChild("setting").Element();
    for ( ; settingElem; settingElem = settingElem->NextSiblingElement())
    {
        int idx = GeneralSettings::SettingNameToArrayIndex(settingElem->Attribute("name"));
        if (idx != -1)
        {
            nArg arg;
            if (StringToArg(settingElem->Attribute("type"), settingElem->Attribute("value"), arg))
            {
                this->settingsArray[idx] = arg;
            }
            else
            {
                n_assert2(false, "Failed to construct nArg!");
            }
        }
        else
        {
            n_assert2(false, "Unknown setting!");
        }
    }

    // load preview renderers
    settingElem = xmlhSettings.FirstChild("Preview").FirstChild("Renderers").FirstChild("Renderer").Element();
    for ( ; settingElem; settingElem = settingElem->NextSiblingElement())
    {
        nString renderer = settingElem->Attribute("name");
        if (!renderer.IsEmpty())
        {
            this->rendererNameArray.PushBack(renderer);
            this->rendererFeatureSetArray.PushBack(settingElem->Attribute("featureset"));
        }
    }

    // load preview sizes
    settingElem = xmlhSettings.FirstChild("Preview").FirstChild("Sizes").FirstChild("Size").Element();
    for ( ; settingElem; settingElem = settingElem->NextSiblingElement())
    {
        nString size = settingElem->Attribute("name");
        if (!size.IsEmpty())
        {
            this->previewSizeArray.PushBack(size);
        }
    }

    // load scene node types
    settingElem = xmlhSettings.FirstChild("SceneNodeTypes").FirstChild("SceneNodeType").Element();
    for ( ; settingElem; settingElem = settingElem->NextSiblingElement())
    {
        nString sceneNodeType = settingElem->Attribute("name");
        if (!sceneNodeType.IsEmpty())
        {
            this->sceneNodeTypesArray.PushBack(sceneNodeType);
        }
    }

    // load export node types
    settingElem = xmlhSettings.FirstChild("ExportNodeTypes").FirstChild("ExportNodeType").Element();
    for ( ; settingElem; settingElem = settingElem->NextSiblingElement())
    {
        nLWExportNodeType* exportNodeType = n_new(nLWExportNodeType);
        if (exportNodeType)
        {
            if (exportNodeType->LoadFromXML(settingElem))
            {
                this->exportNodeTypesArray.PushBack(exportNodeType);
            }
            else
            {
                n_delete(exportNodeType);
            }
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::Save()
{
    // the settings are stored in the Lightwave content directory
    nLWGlobals::DirInfoFunc dirInfo;
    nString settingsFileName(dirInfo.GetContentDir());
    settingsFileName.StripTrailingSlash();
    settingsFileName += "\\";
    settingsFileName += nLWExporterSettings::SETTINGS_FILENAME;

    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(settingsFileName.Get()))
    {
        n_printf("Failed to load %s.", settingsFileName.Get());
        return;
    }

    TiXmlHandle xmlhSettings(xmlDoc.FirstChild("NebulaExporterSettings"));

    // general settings
    TiXmlElement* categoryElem = xmlhSettings.FirstChild("General").Element();
    n_assert(categoryElem);
    if (categoryElem)
    {
        // discard all general settings currently in the xml file
        categoryElem->Clear();

        // write out the current general settings
        for (int i = 0; i < this->settingsArray.Size(); i++)
        {
            nString argTypeStr;
            nString argValStr;
            if (ArgToString(this->settingsArray[i], argTypeStr, argValStr))
            {
                TiXmlElement* settingElem = n_new(TiXmlElement("setting"));
                if (settingElem)
                {
                    settingElem->SetAttribute("name", GeneralSettings::SETTING_NAME_ARRAY[i]);
                    settingElem->SetAttribute("type", argTypeStr.Get());
                    settingElem->SetAttribute("value", argValStr.Get());
                    categoryElem->LinkEndChild(settingElem);
                }
            }
        }
    }

    // preview renderers
    // can't change this via the UI yet, so nothing to do here

    // preview sizes
    // can't change this via the UI yet, so nothing to do here

    // scene node types
    categoryElem = xmlhSettings.FirstChild("SceneNodeTypes").Element();
    n_assert(categoryElem);
    if (categoryElem)
    {
        // discard all scene node types currently in the xml file
        categoryElem->Clear();

        // write out the current scene node types
        for (int i = 0; i < this->sceneNodeTypesArray.Size(); i++)
        {
            TiXmlElement* sceneNodeTypeElem = n_new(TiXmlElement("SceneNodeType"));
            if (sceneNodeTypeElem)
            {
                sceneNodeTypeElem->SetAttribute("name", this->sceneNodeTypesArray[i].Get());
                categoryElem->LinkEndChild(sceneNodeTypeElem);
            }
        }
    }

    // export node types
    categoryElem = xmlhSettings.FirstChild("ExportNodeTypes").Element();
    n_assert(categoryElem);
    if (categoryElem)
    {
        // discard all export node types currently in the xml file
        categoryElem->Clear();

        // write out the current export node types
        for (int i = 0; i < this->exportNodeTypesArray.Size(); i++)
        {
            TiXmlElement* exportNodeTypeElem = n_new(TiXmlElement("ExportNodeType"));
            if (exportNodeTypeElem)
            {
                if (this->exportNodeTypesArray[i]->SaveToXML(exportNodeTypeElem))
                {
                    categoryElem->LinkEndChild(exportNodeTypeElem);
                }
                else
                {
                    n_delete(exportNodeTypeElem);
                }
            }
        }
    }

    xmlDoc.SaveFile();
}

//----------------------------------------------------------------------------
/**
*/
nString
nLWExporterSettings::GetProjectDir() const
{
    nLWGlobals::DirInfoFunc dirInfo;
    // the Lightwave content dir should be set to proj:work/
    nString contentDir = dirInfo.GetContentDir();
    // strip off the last dir in the path (work) to get the proj dir
    contentDir.StripTrailingSlash();
    return contentDir.ExtractToLastSlash();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetGeometryScale(float factor)
{
    this->settingsArray[(int)GeneralSettings::GeometryScale].SetF(factor);
}

//----------------------------------------------------------------------------
/**
*/
float
nLWExporterSettings::GetGeometryScale() const
{
    return this->settingsArray[(int)GeneralSettings::GeometryScale].GetF();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetScriptServer(const nString& scriptServer)
{
    this->settingsArray[(int)GeneralSettings::ScriptServer].SetS(scriptServer.Get());
}

//----------------------------------------------------------------------------
/**
*/
nString
nLWExporterSettings::GetScriptServer() const
{
    return this->settingsArray[(int)GeneralSettings::ScriptServer].GetS();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetPreviewRenderer(const nString& renderer)
{
    this->settingsArray[(int)GeneralSettings::PreviewRenderer].SetS(renderer.Get());
}

//----------------------------------------------------------------------------
/**
*/
nString
nLWExporterSettings::GetPreviewRenderer() const
{
    return this->settingsArray[(int)GeneralSettings::PreviewRenderer].GetS();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetPreviewWidth(int w)
{
    this->settingsArray[(int)GeneralSettings::PreviewWidth].SetI(w);
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterSettings::GetPreviewWidth() const
{
    return this->settingsArray[(int)GeneralSettings::PreviewWidth].GetI();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetPreviewHeight(int h)
{
    this->settingsArray[(int)GeneralSettings::PreviewHeight].SetI(h);
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterSettings::GetPreviewHeight() const
{
    return this->settingsArray[(int)GeneralSettings::PreviewHeight].GetI();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetPreviewFullScreen(bool fullScreen)
{
    this->settingsArray[(int)GeneralSettings::PreviewFullScreen].SetB(fullScreen);
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWExporterSettings::GetPreviewFullScreen() const
{
    return this->settingsArray[(int)GeneralSettings::PreviewFullScreen].GetB();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetOutputBinarySceneNode(bool binaryOutput)
{
    this->settingsArray[(int)GeneralSettings::OutputBinarySceneNode].SetB(binaryOutput);
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWExporterSettings::GetOutputBinarySceneNode() const
{
    return this->settingsArray[(int)GeneralSettings::OutputBinarySceneNode].GetB();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetOutputBinaryMesh(bool binaryOutput)
{
    this->settingsArray[(int)GeneralSettings::OutputBinaryMesh].SetB(binaryOutput);
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWExporterSettings::GetOutputBinaryMesh() const
{
    return this->settingsArray[(int)GeneralSettings::OutputBinaryMesh].GetB();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetOutputBinaryAnimation(bool binaryOutput)
{
    this->settingsArray[(int)GeneralSettings::OutputBinaryAnimation].SetB(binaryOutput);
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWExporterSettings::GetOutputBinaryAnimation() const
{
    return this->settingsArray[(int)GeneralSettings::OutputBinaryAnimation].GetB();
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterSettings::GetNumPreviewRenderers() const
{
    return this->rendererNameArray.Size();
}

//----------------------------------------------------------------------------
/**
*/
const nString&
nLWExporterSettings::GetPreviewRendererName(int index) const
{
    return this->rendererNameArray[index];
}

//----------------------------------------------------------------------------
/**
*/
const nString&
nLWExporterSettings::GetPreviewRendererFeatureSet(int index) const
{
    return this->rendererFeatureSetArray[index];
}

//----------------------------------------------------------------------------
/**
*/
const nString&
nLWExporterSettings::GetPreviewRendererFeatureSet(const nString& renderer)
{
    for (int i = 0; i < this->rendererNameArray.Size(); i++)
        if (this->rendererNameArray[i] == renderer)
            return this->rendererFeatureSetArray[i];

    n_assert(false); // should never get here
    static nString emptyStr;
    return emptyStr;
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterSettings::GetNumPreviewSizes() const
{
    return this->previewSizeArray.Size();
}

//----------------------------------------------------------------------------
/**
*/
const nString&
nLWExporterSettings::GetPreviewSize(int index) const
{
    return this->previewSizeArray[index];
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterSettings::GetNumExportNodeTypes() const
{
    return this->exportNodeTypesArray.Size();
}

//----------------------------------------------------------------------------
/**
*/
nLWExportNodeType*
nLWExporterSettings::GetExportNodeType(int index) const
{
    return this->exportNodeTypesArray[index];
}

//----------------------------------------------------------------------------
/**
*/
nLWExportNodeType*
nLWExporterSettings::GetExportNodeType(const nString& nodeType) const
{
    for (int i = 0; i < this->exportNodeTypesArray.Size(); i++)
    {
        if (this->exportNodeTypesArray[i]->GetName() == nodeType)
        {
            return this->exportNodeTypesArray[i];
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::ClearExportNodeTypes()
{
    for (int i = 0; i < this->exportNodeTypesArray.Size(); i++)
    {
        n_delete(this->exportNodeTypesArray[i]);
    }
    this->exportNodeTypesArray.Clear();
}

//----------------------------------------------------------------------------
/**
*/
void
nLWExporterSettings::SetExportNodeTypes(nArray<nLWExportNodeType*>& array)
{
    this->ClearExportNodeTypes();
    this->exportNodeTypesArray.Reallocate(array.Size(), 0);
    for (int i = 0; i < array.Size(); i++)
    {
        this->exportNodeTypesArray.PushBack(n_new(nLWExportNodeType(*(array[i]))));
    }
}

//----------------------------------------------------------------------------
/**
*/
int
nLWExporterSettings::GetNumSceneNodeTypes() const
{
    return this->sceneNodeTypesArray.Size();
}

//----------------------------------------------------------------------------
/**
*/
const nString&
nLWExporterSettings::GetSceneNodeType(int index) const
{
    return this->sceneNodeTypesArray[index];
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
