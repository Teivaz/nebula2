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

    @brief
*/
class nString;

//-----------------------------------------------------------------------------
class nMaxOptions
{
public:
    virtual ~nMaxOptions();

    static nMaxOptions* Instance();

    bool Initialize();

    void SetSaveFileName(const nString&);
    nString GetSaveFileName() const;
    const nString& GetSaveFilePath() const;
    bool GroupMeshes();

    /// @name mesh options access functions.
    /// @{
    void SetExportNormals(bool status);
    void SetExportColors(bool status);
    void SetExportUvs(bool status);
    void SetExportTangents(bool status);
    void SetExportHiddenNodes(bool status);

    bool ExportNormals();
    bool ExportColors();
    bool ExportUvs();
    bool ExportTangents();
    bool ExportHiddenNodes();
    /// @}

    /// @name assign and path access functions.
    /// @{
    const nString& GetHomePath() const;
    const nString& GetAnimAssgin() const;
    const nString& GetAnimPath() const;

    const nString& GetGfxLibAssign() const;
    const nString& GetGfxLibPath() const;

    const nString& GetMeshesAssign() const;
    const nString& GetMeshesPath() const;

    const nString& GetTextureAssign() const;
    const nString& GetTexturePath() const;
    /// @}

    void SetMeshFileType(const char* type);
    const nString& GetMeshFileType() const;
    void SetAnimFileType(const char* type);
    const nString& GetAnimFileType() const;

    bool RunViewer();

    const char* GetViewerDir() const;

    void SetSaveScriptServer(const char* serverName);
    const nString& GetSaveScriptServer();

    bool UseIndivisualMesh() const;

    int GetVerboseLevel();

    bool OverwriteExistTexture() const;

    void SetMaxJointPaletteSize(int size);
    int GetMaxJointPaletteSize();
    void SetWeightTrashold(float val);
    float GetWeightTrashold() const;

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

    nString meshFileExtension;
    nString animFileExtension;

    nString sceneFilename;

    int sampleRate;
    int maxJointPaletteSize;
    int weightTrashHold;
    /// @}

    /// @nanme assign and path variables.
    /// @{
    nString homeDir;
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

    bool useIndivisualMesh;

    bool runViewer;

    int verboseLevel; /// -1: errors only, 0:low, 1:midium, 2:high

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
/**
*/
inline
int nMaxOptions::GetVerboseLevel()
{
    return this->verboseLevel;
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
void nMaxOptions::SetWeightTrashold(float val)
{
    this->weightTrashHold = val;
}
//-----------------------------------------------------------------------------
inline
float nMaxOptions::GetWeightTrashold() const
{
    return this->weightTrashHold;
}
#endif
