#ifndef N_MAXEXPORTTASK_H
#define N_MAXEXPORTTASK_H
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
    Aug 2004 Oleg Kreptul (Haron)
*/

#include "util/npathstring.h"

#define N_MAXEXPORT_BINARY_PATH    "home:bin/win32d/"

#define N_MAXEXPORT_ANIMS_ASSIGN    "home:export/anims/"
#define N_MAXEXPORT_GFXLIB_ASSIGN   "home:export/gfxlib/"
#define N_MAXEXPORT_GUI_ASSIGN      "home:export/"
#define N_MAXEXPORT_LIGHTS_ASSIGN   "home:export/"
#define N_MAXEXPORT_MESHES_ASSIGN   "home:export/meshes/"
#define N_MAXEXPORT_SHADERS_ASSIGN  "home:data/shaders/"
#define N_MAXEXPORT_TEXTURES_ASSIGN "home:export/textures/"

#define N_MAXEXPORT_ANIMS_PATH    "anims:examples/"
#define N_MAXEXPORT_GFXLIB_PATH   "gfxlib:examples/"
#define N_MAXEXPORT_GUI_PATH      "gui:"
#define N_MAXEXPORT_LIGHTS_PATH   "lights:"
#define N_MAXEXPORT_MESHES_PATH   "meshes:examples/"
#define N_MAXEXPORT_SHADERS_PATH  "shaders:"
#define N_MAXEXPORT_TEXTURES_PATH "textures:examples/"

//BOOL CALLBACK Nebula2ExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

class nMaxExportTask
{
public:
    nMaxExportTask();
    ~nMaxExportTask();
    bool UseWeightedNormals() { return this->useWeightedNormals; }

    ///export nodes that are hidden?
    bool exportHiddenNodes;
    ///export tranformation or vector animations?
    bool exportAnimations;
    /// When set to true, the scale of meshes will be set to 1/(length of longest dimension).
    bool normalizeMeshScale;
    ///group the meshes by source object (increases the meshsize, but makes it possible to work with the original node, not only the materials used in the scene)
    bool groupMeshBySourceObject;

    ///the sample rate used for the transform and vector animation samplings (only used when no keyframe export is possible)
    int sampleRate;

    ///use the IGame option to create wighted normals? (requires that every face has min 1 smoth group assigned)
    bool useWeightedNormals;
    
    ///baseName of the meshfile
    nPathString meshFileName;

    ///extension of the mesh file (.n3d2|.nvx2)
    nString meshFileExtension;

    ///basename of the scenefile
    nPathString sceneFileName;

    ///the maxium joint palette size (see nSkinPartioner for more)
    int maxJointPaletteSize;
    
    ///when this is not 0, than only these vertexcomponents will be exported.
    int forcedVextexComponents;

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

    ///set config file path
    void SetCfgFileName(nPathString& cfn);
    ///read config file
    bool ReadConfig();
    ///write config file
    bool WriteConfig();

    ///get home dir
    nString GetHomeDir();
    ///set home dir (to set 'home' assign)
    void SetHomeDir(nString hd);
private:
    nPathString cfgFileName;
    nPathString homeDir;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaxExportTask::SetCfgFileName(nPathString& cfn)
{
    this->cfgFileName = cfn;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nMaxExportTask::GetHomeDir()
{
    //this->homeDir.ConvertSlashes();
    return this->homeDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaxExportTask::SetHomeDir(nString hd)
{
    this->homeDir = hd.Get();
    this->homeDir.ConvertBackslashes();
    this->homeDir.StripTrailingSlash();
}

#endif
