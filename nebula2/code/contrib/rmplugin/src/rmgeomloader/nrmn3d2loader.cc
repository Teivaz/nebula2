//-----------------------------------------------------------------------------
//  nrmn3d2loader.cc
//
//  Geometry loader plug-in implementation
//
//  (C) 2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "rmgeomloader/nrmgeomloader_pch.h"
#include "rmgeomloader/nrmn3d2loader.h"
#include "rmgeomloader/nrmgeomloader_main.h"
#include "Shlwapi.h"
#include "rmgeomloader/nrmmeshbuilder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
/**
*/
nRMN3d2Loader::nRMN3d2Loader() : IRmGeometryLoaderPlugIn()
{
    // This plug-in's type is a geometry plug-in, we must explicitly specify it:
    this->plugInDesc.plugInType = RM_PLUGINTYPE_GEOMETRY_LOADER;

    RmPlugInID plugInID = { 0x52e55508, 0x1c5f, 0x4c0a, { 0x84, 0x6c, 0xc3, 0xd0, 0x63, 0x84, 0x71, 0xfb } };
    this->plugInDesc.plugInID   = plugInID;

    // There is only one type of node that this plug-in is associated with by the main application:
    this->plugInDesc.nNumSupportedNodeTypes = 1;
    this->plugInDesc.supportedNodeTypes[0] = RM_MODEL_DATA_TYPE; // Model data node association

    // This plug-in is implemented using 1.0 Wizard version:
    this->plugInDesc.nMajorSDKVersion = RENDERMONKEY_SDK_CURRENT_VERSION_MAJOR;
    this->plugInDesc.nMinorSDKVersion = RENDERMONKEY_SDK_CURRENT_VERSION_MINOR;

    // This plug-in in can be used in effects of all types and thus doesn't care about the
    // graphics API version:
    RmStrCopyT(this->plugInDesc.strRenderAPIVersion, RM_API_AGNOSTIC);

    // The name that the user will see for this plug-in
    RmStrCopyT(this->plugInDesc.strName, _T( "Nebula2GeometryLoader"));

    if (0 == nKernelServer::Instance())
    {
        new nKernelServer();
    }
}

//-----------------------------------------------------------------------------
/**
*/
nRMN3d2Loader::~nRMN3d2Loader()
{
    nKernelServer* ks = nKernelServer::Instance();
    n_delete(ks);
}

//-----------------------------------------------------------------------------
/**
    Initializes this particular plug-in
*/
bool nRMN3d2Loader::Init()
{
    return true;
}

//-----------------------------------------------------------------------------
/**
    Performs uninitialization of the plug-in specific data
*/
void nRMN3d2Loader::Uninitialize()
{
}

//-----------------------------------------------------------------------------
/**
    RenderMonkey main message handling entry point. RenderMonkey communicates
    all of its events with the loaded plug-ins via messages. See
    Wizard/Include/Core/RmDefines.h for more details on each message.

    @return RM_OK if the message was successfully handled and RM_EFAIL otherwise.
*/
int nRMN3d2Loader::MessageHandler(int nMessageID,
                                  int nMessageData,
                                  int nMessageParameter /*=0*/,
                                  const RmPlugInID* pRmPlugInID /*=NULL*/)
{
    return RM_OK;
}

//-----------------------------------------------------------------------------
/**
    This method returns a list of file extensions that are supported by this geometry
    loader plug-in for importing geometry from. These values are used to control the
    display of files in the FileOpen dialog for model load. Note that number of entries
    for 'formatDescriptions' and 'formatExtensions' must match.

    Eg. For X file, extension = "*.X"
                    text      = "X Studio File (*.X)"
*/
void nRMN3d2Loader::GetSupportedExtensions(RmLinkedList<RmStringT> &formatDescriptions,
                                           RmLinkedList<RmStringT> &formatExtensions)
{
    formatDescriptions.push_back(_T("N2 File (*.n3d2)"));
    formatExtensions.push_back(_T("*.n3d2"));
}

//-----------------------------------------------------------------------------
/**
    Method checks whether this plug-in can load geometry in the specified file
    based on its own criteria. Returns true if it can load the file and false
    otherwise.
*/
bool nRMN3d2Loader::CanLoadGeometry(const RM_TCHAR *filename)
{
    // We want to use RenderMonkey specific file open methods to ensure that
    // we are always opening file using either unicode or ascii-specific
    // methods using the application settings automatically:
    FILE *file = RmFileOpenT(filename, _T("r"));
    if (file == NULL)
    {
        return false;
    }

    // Read the first line of this file
    bool result = false;
    RM_CHAR line[1024];

    while (fread(&line, sizeof(line), 1, file))
    {
        char* keyword = strtok(line, N_WHITESPACE);
        if (0 == keyword)
        {
            continue;
        }
        else
        if (0 == strcmp(keyword, "type"))
        {
            const char* typeString = strtok(0, N_WHITESPACE);
            n_assert(typeString);
            if (0 != strcmp(typeString, "n3d2"))
            {
                n_message("CanLoadGeometry: In file '%s' invalid type '%s', must be 'n3d2'\n", filename, typeString);
                RmFileClose(file);
                return false;
            }
            // it can be loaded, so no need to read more lines.
            result = true;
            break;
        }
    }

    RmFileClose(file);
    return result;
}


//-----------------------------------------------------------------------------
/**
    This method gets called by the main application to read in geometry data from
    a particular file ('strFileName') into the specified model data container
    ('pModelContainer')
*/
bool nRMN3d2Loader::LoadGeometry(const RM_TCHAR       *strFileName,
                                       RmMeshModelContainer *modelContainer)
{
    // Actual loading code goes here
    RmUtilWaitCursor waitCursor;

    if (!ConstructScene(strFileName, modelContainer))
        return false;

    return true;
}


