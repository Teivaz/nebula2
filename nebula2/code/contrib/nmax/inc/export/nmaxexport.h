#ifndef N_MAXEXPORT_H
#define N_MAXEXPORT_H
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
    Sep 2004 Oleg Kreptul (Haron)
*/

#include <Max.h>
#include <decomp.h> //the decompose functions for matrix3

#include <IGame/IGame.h>
#include <IGame/IGameProperty.h>
#include <IGame/IGameModifier.h>

#include "base/nmaxdll.h"
#include "base/nmaxloghandler.h"

#include "export/nmaxexporttask.h"

#include "util/nstring.h"
#include "util/narray.h"

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "script/ntclserver.h"
#include "variable/nvariableserver.h"

#include "tools/nskinpartitioner.h"
#include "scene/nskinanimator.h"
#include "tools/nmeshbuilder.h"
#include "tools/nanimbuilder.h"

#include "scene/ntransformnode.h"
#include "scene/ntransformanimator.h"
#include "scene/nshapenode.h"
#include "scene/nskinshapenode.h"
#include "scene/nspotlightnode.h"

#define N_MAX_EXPORT_CLASS_ID	Class_ID(0x7bcf1e6d, 0xbaddf00d)

#define LONGDESC	"Nebula2 Scene Export"
#define SHORTDESC	"Nebula2 Export"
#define AUTHORNAME	"J. Kellner"
#define COPYRIGHT	"Nebula License"
#define MESSAGE1    "This file is licensed under the terms of the Nebula License"
#define MESSAGE2	"(c) 2004 J. Kellner"
#define NUM_EXT		1
#define EXTENSION   "n2"

#define VERSION			100				 // Version number * 100
#define CFGFILENAME		"nMaxExport.cfg" // Configuration file

//FIXME: create GUI options for this
#define N_MAXEXPORT_TIMECHANNELNAME     "time"

#define N_MAXEXPORT_SCRIPTSERVER        "ntclserver"

#define N_MAXEXPORT_IGAMEPROPERYIFILE   _T("plugcfg/IGameProp.xml")
#define N_MAXEXPORT_IGAMECOORDSYS       IGameConversionManager::IGAME_OGL //can be OGL/D3D or user defined (OLG == NEBULA)

class nMaxExportTask;
class nMaxExport : public SceneExport
{
public:
	nMaxExport();
	~nMaxExport();
	
	/// from max: do export
	int	DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
	/// from max: show the about dialog
	void ShowAbout(HWND hWnd) { return; } 
	/// from max: show file extensition 
	const TCHAR * Ext(int n);
	/// from max: number of extensitions
	int	ExtCount() { return NUM_EXT; }
	/// from max: options we are support FIXME: return the state of SCENE_EXPORT_SELECTED
	BOOL SupportsOptions(int /*ext*/, DWORD /*options*/) { return true; }
	/// from max: Version number * 100 (i.e. v3.01 = 301) 
	unsigned int Version()			{ return VERSION; }	
	
	/// from max: Long description
	const TCHAR * LongDesc() { return LONGDESC; }
	/// from max: Short description
	const TCHAR * ShortDesc() { return SHORTDESC; }
	/// from max: Author name
	const TCHAR * AuthorName() { return AUTHORNAME; }
	/// from max: Copyright message
	const TCHAR * CopyrightMessage() { return COPYRIGHT; }
	/// from max: Other message #1	 
	const TCHAR * OtherMessage1() { return MESSAGE1; }
	/// from max: Other message #2	
	const TCHAR * OtherMessage2() { return MESSAGE2; }
	
private:   
    nMaxExportTask* task;
    
    ///the internal scene node path used as root for the export
    nString nohBase;
	
    ///true if max request to show no prompts
    bool suppressPrompts;
    ///true if only the selected nodes should be exported
    bool exportSelected;
	
    /** @brief
    from max: Time is stored internally in 3ds max as an integer number of ticks.
    Each second of an animation is divided into 4800 ticks.
    This value is chosen in part because it is evenly divisible by the standard frame
    per second settings in common use (24 -- Film, 25 -- PAL, and 30 -- NTSC).
    tickTime = 1.0f / FramesPerSecond / TicksPerFrame;
    */
    float tickTime; 

	///pointer to the basic max interface
	Interface* maxInterface;
    ///pointer to the max export interface
	ExpInterface* expInterface;
	
	///the property file used for IGame
	TCHAR* propertyFile;
    ///the IGameScene pointer
	IGameScene* iGameScene;

    /// 
    int ticksPerFrame;
    int startFrame;
    int endFrame;
	
    ///the pointer to the scriptserver
    nScriptServer* scriptServer;
    ///the pointer to the max loghandler
	nMaxLogHandler* logHandler;
    ///the pointer to the variableserver
    nVariableServer* varServer;
	
    enum
    {
        MAX_NUM_BONES = 4 ///< don't change! the data is exported through a vector4 so a change won't work.
    };

    ///a struct to store the used materials of a mesh in the exporters material pool
    struct Material
    {
        IGameMaterial* igMaterial;  ///< the IGame material node
    };

    ///a struct to store informations that defines what groups of a nMeshBuilder are from what IGameNode mesh object.
    struct MeshObject
    {
        nArray<int> groupIDs;                   ///< the groupID(s) into the meshBuilder of the PoolEntry this MeshObject is atteched to
        nArray< nArray<int> > jointPalettes;    ///< if skinned than the result of the nSkinPartioner is stored there
        nRoot* nNode;                           ///< the nebula node that is the root node for this mesh
        nString nNodeName;                      ///< the NOH path of the root node for this mesh
        IGameNode* igNode;                      ///< the IGameNode where the mesh data is from
    };

    ///only meshObjects with the same attributes are stored together in one pool
    struct PoolEntry
    {
        nPathString meshFileName;       ///< sceneName + (int)vertexComponets + (int) vertexUsage
        nMeshBuilder* meshBuilder;      ///< storage of mesh data
        nArray<MeshObject> meshObjects; ///< array of meshObjects that are stored in this pool/in the meshBuilder

        bool skinned;                   ///< define wheather mesh skinned or not
        nPathString animFileName;       ///< animation file name
		nString animNodePath;           ///< the NOH path
        nAnimBuilder* animBuilder;      ///< storage of animation data
        nArray<int> boneIDs;            ///< bones ID
    };

    ///progess 
    float progressOnePercent;
    ///current progess value
    int progressCount;
    ///value that represent 100% of the current progress operation
    int progressNumTotal;

    ///update the progress bar
    void progressUpdate(int percent, nString msg);

    ///get config path
    void GetCfgFilename(nPathString& fileName);
    ///init 'home', 'anims', 'gfxlib', 'meshes', 'shaders', 'textures' assigns
    void InitAssigns();

    ///array of all materials used by the exported objects
    nArray<Material>  materials;
    ///array of the mesh pools that are created(divided by used vertexcomponents) to export the mesh objects
    nArray<PoolEntry> meshPool;  
    
    Matrix3 GetNodeTM(IGameNode *igNode, int time, int type = 0); // type: 0 - NodeTM, 1 - LocalTM, 2 - WorldTM
    void GetPRS(Matrix3 m, int time, vector4& pos, quaternion& rot, vector4& scale);
    void GetPRS(IGameNode *igNode, int time, vector4& pos, quaternion& rot, vector4& scale, int type = 0);
    void GetPRSBoneSpace(IGameNode *igNode, int time, vector4& pos, quaternion& rot, vector4& scale, int type = 0);

    //---main loop---
    ///export a IGameNode, check the type and do the needed nodeTypeExport (mesh/light...) - recurse to subnodes - must return true if ok.
    bool exportNode(IGameNode* igNode, nString node);
      
    //---node handling---
    /// export a material - setup the nebula material node     
    void exportMaterial(nMaterialNode* materialNode, nString nodeName, IGameMaterial* material, bool skinned);
   
    ///@todo: export a light node
    bool exportLight(nString nodeName, IGameNode* igNode);

    //TODO: add other(skeleton,camera,...) needed node export functions here
    
    //---mesh export---
    ///export the mesh data 
    void exportMesh(IGameNode* igNode, /*nSceneNode* nNode = 0,*/ const nString nodeName = ""/*, matrix44* transform = 0*/);     
    ///export the faces of a mesh
    void exportFaces(Tab<FaceEx*> matFaces, const int matID, MeshObject &meshObject, nMeshBuilder* meshBuilder, bool skinned);
    ///save the meshPools to disk and create the needed nShape-/nSkinShape- Nodes
    void storeDataPools();
    ///append a meshObeject and the meshBuilder with the data of this object to the meshPools (finds a fitting pool, or create a new one)
    void appendDataToPool(MeshObject &meshObject, nMeshBuilder* meshBuilder, nAnimBuilder *animBuilder, bool skinned);

    //---animation export---
    void exportSkinnedAnim(IGameNode *igNode, nString nodeName, nAnimBuilder *animBuilder);
    nAnimBuilder::Group createAnimGroup(int numFrames, nAnimBuilder::Group::LoopType loopType);
    bool writeCurves(IGameNode *igNode, nAnimBuilder::Group& animGroup, int& numUncompressedCurves);
    int getCurve(IGameControl *igControl, IGameControlType type, nAnimBuilder::Curve &curve);
    void getBipedCurves(IGameNode *igNode, nAnimBuilder::Curve &posCurve, nAnimBuilder::Curve &rotCurve, nAnimBuilder::Curve &scaleCurve);
	int GetBoneByID(int id, bool insert = true);
    void findBones(IGameSkin *igSkin, int vertNum);
    void traceBonesTree(IGameNode *igBone, nArray<int>& tmpIDs);
	nArray<int> boneIDs;


    //---tranformations---
    ///create the matrix for this node to convert the data from world back to nodes local system.
    matrix44* buildInverseModelWorldMatrix(IGameNode* igNode);

    ///export the local position of the igNode, create a animator if required.
    void exportPosition(nTransformNode* nNode, nString nodeName, IGameNode* igNode);
    ///export the local rotation of the igNode, create a animator if required.
    void exportRotation(nTransformNode* nNode, nString nodeName, IGameNode* igNode);
    ///export the local scale of the igNode, create a animator if required.
    void exportScale(nTransformNode* nNode, nString nodeName, IGameNode* igNode);

    ///extract the position component from a matrix
    vector3 PositionComponent(Matrix3 m);
    ///extract the position component from a decomposed matrix
    vector3 PositionComponent(const AffineParts &parts);
    
    ///extract the rotation component from a matrix
    quaternion RotationComponent(Matrix3 m);
    ///extract the rotation component from a decomposed matrix
    quaternion RotationComponent(const AffineParts &parts);
    
    ///extract the scale component form a matrix
    vector3 ScaleComponent(Matrix3 m, const TCHAR* nodeName = 0);
    ///extract the scale component form a decomposed matrix
    vector3 ScaleComponent(const AffineParts &parts, const TCHAR* nodeName = 0);
    ///extract the scale component form a scaleAxis quaternion and a Point3 scale value
    vector3 ScaleComponent(const Quat &u, const Point3 &k, const TCHAR* nodeName = 0);
	
    //---helpers---
    /// removes bad chars from a string
	char* checkChars(char* string);
    /// removes bad chars from a string
    char* checkChars(const char* string);
    /// removes bad chars from a string
	nString checkChars(nString &string);

    ///cleanup data
    void CleanupData();
};

///create the autoregistered classDescription to make this class public to max
static tRegisteredClassDesc<nMaxExport> nMaxExportClassDescInstance("nMaxExport", N_MAX_EXPORT_CLASS_ID, SCENE_EXPORT_CLASS_ID, 1);

//------------------------------------------------------------------------------
/**
    return the file extension that is used in the export filetype dropdown
*/
inline const TCHAR *
nMaxExport::Ext(int n)
{ 
	switch (n)
	{
		case 0:
			return EXTENSION;
		break;
	}
	return "";
}

#endif
