//-----------------------------------------------------------------------------
//  nmaxoptions.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXOPTIONS_H
#define N_MAXOPTIONS_H
//-----------------------------------------------------------------------------
/**
    @class nMaxOptions
    @ingroup NebulaMaxExport2Contrib

    @brief A class for global options of the exporter.

*/
#include "util/nstring.h"

//-----------------------------------------------------------------------------
class nMaxOptions
{
public:
    /// the destructor.
    virtual ~nMaxOptions();

    static nMaxOptions* Instance();

    bool Initialize();
    bool LoadUtilityOptions();
    void SaveUtilityOptions();

    void SetSaveFileName(const nString&);
    nString GetSaveFileName() const;
    const nString& GetSaveFilePath() const;
    bool GroupMeshes();

    // @name Mesh options
    // @{
    void SetExportNormals(bool status);
    void SetExportColors(bool status);
    void SetExportUvs(bool status);
    void SetExportTangents(bool status);
    
    bool ExportNormals();
    bool ExportColors();
    bool ExportUvs();
    bool ExportTangents();
    /// @}

    // @name assign and path access functions.
    // @{
    const nString& GetHomePath() const;
    const nString& GetAnimAssign() const;
    const nString& GetAnimPath() const;

    const nString& GetGfxLibAssign() const;
    const nString& GetGfxLibPath() const;

    const nString& GetMeshesAssign() const;
    const nString& GetMeshesPath() const;

    const nString& GetTextureAssign() const;
    const nString& GetTexturePath() const;
    /// @}

    /// @name Global utility panel options
    /// @{
    void SetGeomScaleValue(float val);
    float GetGeomScaleValue() const;

    void SetSampleRate(int val);
    int GetSampleRate() const;
    void SetAddJointName(bool flag);
    bool HasJointName() const;

    void SetMeshFileType(const char* type);
    const nString& GetMeshFileType() const;
    void SetAnimFileType(const char* type);
    const nString& GetAnimFileType() const;

    void SetExportHiddenNodes(bool status);
    bool ExportHiddenNodes();

    void SetMaxJointPaletteSize(int size);
    int GetMaxJointPaletteSize();
    void SetWeightThreshold(float val);
    float GetWeightThreshold() const;

    void SetSaveScriptServer(const char* serverName);
    const nString& GetSaveScriptServer();
    /// @}

    bool RunViewer();

    const char* GetViewerDir() const;

    bool UseIndivisualMesh() const;

    int GetVerboseLevel() const;
    void SetVerboseLevel(int level);

    bool OverwriteExistTexture() const;

    void SetPreviewMode(bool on);
    bool UsePreviewMode() const;

    bool UseDefaultViewer() const;
    void SetUseDefaultViewer(bool use);

protected:
    /// @name export option variables.
    /// @{
    bool exportNormal;
    bool exportVertColor;
    bool exportUvs;
    bool exportTangent;

    bool exportHiddenNodes;
    bool exportAnimations;
    bool groupMeshes;

    /// mesh file extension '.n3d2' or 'nvx2'
    nString meshFileExtension;
    /// animation file extension '.nanim2' or 'nax2'
    nString animFileExtension;

    nString sceneFilename;

    /// animation sample rate.
    int sampleRate;
    /// maximum number of joint palette.
    int maxJointPaletteSize;
    /// weight threshold.
    float weightThreshold;
    /// flag for adding joint name.
    bool addJointName;
    /// geometry scaling value.
    float geomScale;
    /// @}

    /// @name assign and path variables.
    /// @{
    nString homeDir;
    nString projDir;
    nString binaryPath;

    nString animsAssign;
    nString gfxlibAssign;
    nString guiAssign;
    nString lightsAssign;
    nString meshesAssign;
    nString shadersAssign;
    nString texturesAssign;

    nString animsPath;
    nString gfxlibPath;
    nString guiPath;
    nString lightsPath;
    nString meshesPath;
    nString shadersPath;
    nString texturesPath;
    /// @}

    /// the script server class which should be used for saving.
    nString saveScriptServer; 
    /// 
    bool useIndivisualMesh;
    /// launch specified viewer.
    bool runViewer;
    /// preview mode. It ueses ram file to view exported model.
    bool previewMode;
    /// default viewer.
    bool useDefaultViewer;
    /// verbose level of log message. 0: errors only, 1:warning, 2:low, 3:midium, 4:high
    int verboseLevel; 
    /// If it is present, overwrite any existing texture.
    bool overwriteExistTexture;

private:
    nMaxOptions();
    nMaxOptions(const nMaxOptions&) {};

    static nMaxOptions* Singleton;

};
//-----------------------------------------------------------------------------
/**
    DO NOT FORGET TO CALL 'n_delete' for intance of nMaxOption 
    when application exit.
*/
inline
nMaxOptions*
nMaxOptions::Instance()
{
    // nMaxOption is not nRoot derived class, so we make its instance 
    // directly with n_new if it is not created before.
    if (0 == Singleton)
    {
        n_new(nMaxOptions);
    }

    return Singleton;
}
//-----------------------------------------------------------------------------
inline
void 
nMaxOptions::SetSaveFileName(const nString& saveFileName)
{
    this->sceneFilename = saveFileName;
}
//-----------------------------------------------------------------------------
/**
    c:/nebula2/export/mywork.n2 -> mywork
*/
inline
nString 
nMaxOptions::GetSaveFileName() const
{
    nString str;

    str = this->sceneFilename;
    str = sceneFilename.ExtractFileName();
    str.StripExtension();

    return str;
}
//-----------------------------------------------------------------------------
inline
const nString& 
nMaxOptions::GetSaveFilePath() const
{
    return this->sceneFilename;
}
//-----------------------------------------------------------------------------
/**
    Group meshes to one master mesh file.
*/
inline
bool nMaxOptions::GroupMeshes()
{
    return this->groupMeshes;
}
//-----------------------------------------------------------------------------
inline
void 
nMaxOptions::SetSaveScriptServer(const char* serverName)
{
    this->saveScriptServer = serverName;
}
//-----------------------------------------------------------------------------
inline
const nString& 
nMaxOptions::GetSaveScriptServer()
{
    return this->saveScriptServer;
}
//-----------------------------------------------------------------------------
inline
int nMaxOptions::GetVerboseLevel() const
{
    return this->verboseLevel;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetVerboseLevel(int level)
{
    this->verboseLevel = level;
}
//-----------------------------------------------------------------------------
inline
bool nMaxOptions::UseIndivisualMesh() const
{
    return this->useIndivisualMesh;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetMeshFileType(const char* type)
{
    n_assert(type);
    this->meshFileExtension = type;
}
//-----------------------------------------------------------------------------
/**
    Return mesh file extension ".n3d2" or ".nvx2"
*/
inline
const nString& nMaxOptions::GetMeshFileType() const
{
    return this->meshFileExtension;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetAnimFileType(const char* type)
{
    n_assert(type);
     this->animFileExtension = type;
}
//-----------------------------------------------------------------------------
/**
    Return animation file extension ".nanim2" or ".nax2"
*/
inline
const nString& nMaxOptions::GetAnimFileType() const
{
    return this->animFileExtension;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetMaxJointPaletteSize(int size)
{
    this->maxJointPaletteSize = size;
}
//-----------------------------------------------------------------------------
inline
int nMaxOptions::GetMaxJointPaletteSize()
{
    return this->maxJointPaletteSize;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetWeightThreshold(float val)
{
    this->weightThreshold = val;
}
//-----------------------------------------------------------------------------
inline
float nMaxOptions::GetWeightThreshold() const
{
    return this->weightThreshold;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetAddJointName(bool flag)
{
    this->addJointName = flag;
}
//-----------------------------------------------------------------------------
inline
bool nMaxOptions::HasJointName() const
{
    return this->addJointName;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetGeomScaleValue(float val)
{
    this->geomScale = val;
}
//-----------------------------------------------------------------------------
inline
float nMaxOptions::GetGeomScaleValue() const
{
    return this->geomScale;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetPreviewMode(bool on)
{
    this->previewMode = on;
}
//-----------------------------------------------------------------------------
inline
bool nMaxOptions::UsePreviewMode() const
{
    return this->previewMode;
}
//-----------------------------------------------------------------------------
inline
bool nMaxOptions::UseDefaultViewer() const
{
    return this->useDefaultViewer;
}
//-----------------------------------------------------------------------------
inline
void nMaxOptions::SetUseDefaultViewer(bool use)
{
    this->useDefaultViewer = use;
}
//-----------------------------------------------------------------------------
#endif
