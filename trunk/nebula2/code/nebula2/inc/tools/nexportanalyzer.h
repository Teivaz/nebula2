#ifndef N_EXPORTANALYZER_H
#define N_EXPORTANALYZER_H
//------------------------------------------------------------------------------
/**
    @class nExportAnalyzer

    Analyzes the exported data (.n2 objects, textures, anim, mesh resources,
    Mangalore levels) and builds comma-separated-value files. The object
    expects a completely setup Nebula2 runtime and the proj: assign set
    to the project directory.

    (C) 2006 Radon Labs GmbH
*/
#include "util/nstring.h"
#include "util/narray.h"
#include "gfx2/ntexture2.h"

//------------------------------------------------------------------------------
class nExportAnalyzer
{
public:
    /// constructor
    nExportAnalyzer();
    /// destructor
    ~nExportAnalyzer();
    /// run the entire export
    bool Run();
    /// set error string
    void SetError(const nString& e);
    /// get error string
    const nString& GetError() const;

private:
    /// return category/filename from complete path
    nString ExtractCategoryFilename(const nString& path);
    /// list category files by pattern
    nArray<nString> ListFiles(const nString& rootDir, const nString& pattern, nArray<nString>* gatheredFiles=0);
    /// parse texture infos
    bool ParseTextures();
    /// parse mesh infos
    bool ParseMeshes();
    /// parse anims
    bool ParseAnims();
    /// parse objects
    bool ParseObjects();
    /// parse Mangalore levels
    bool ParseLevels();
    /// read info for a single texture
    bool ReadTextureInfo(const nString& filename);
    /// read info for a single mesh
    bool ReadMeshInfo(const nString& filename);
    /// read info for a single anim
    bool ReadAnimInfo(const nString& filename);
    /// read info for a single N2 object
    bool ReadObjectInfo(const nString& filename);
    /// read game object info
    void ReadGameObjectInfo(const nString& levelName, const nString& graphicsResource);
    /// write texture info csv file
    void WriteTextureInfo();
    /// write mesh info csv file
    void WriteMeshInfo();
    /// write anim info csv file
    void WriteAnimInfo();
    /// write object info csv file
    void WriteObjectInfo();
    /// write level info csv file
    void WriteLevelInfo();
    /// append to a summay file
    void WriteSummary();
    /// recursively count Nebula2 objects which match given class
    int RecurseCountByClass(nClass* cl, nRoot* root, int inputCount = 0);
    /// recursively check if textures exist for a Nebula2 hierarchy
    bool RecurseCheckTexturesExist(nRoot* root, nArray<nString>& failedArray);
    /// find texture info index by name
    int FindUniqueTextureInfo(const nString& name, const nArray<int>& indices);
    /// find mesh info index  by name
    int FindUniqueMeshInfo(const nString& name, const nArray<int>& indices);
    /// find anim info index by name
    int FindUniqueAnimInfo(const nString& name, const nArray<int>& indices);
    /// find object info by name
    int FindUniqueObjectInfo(const nString& name, const nArray<int>& indices);
    /// find level info by name
    int FindLevelInfo(const nString& name);
    /// get concatenated string of all texture names
    nString ConcatTextureNames(const nArray<int>& indices);
    /// get concatenated string of all mesh names
    nString ConcatMeshNames(const nArray<int>& indices);
    /// get concatenated string of all anim names
    nString ConcatAnimNames(const nArray<int>& indices);
    /// get concatenated string of all object names
    nString ConcatObjectNames(const nArray<int>& indices);

    /// hold information about a texture
    struct TextureInfo
    {
        nString filename;
        nString name;
        int fileSize;
        int ramSize;
        nTexture2::Type type;
        nTexture2::Format format;
        int width;
        int height;
        int numMipLevels;
        int useCount;
    };
    /// hold information about a mesh
    struct MeshInfo
    {
        nString filename;
        nString name;
        int fileSize;
        int ramSize;
        int numVertices;
        int numIndices;
        int numEdges;
        int vertexComponents;
        int numGroups;
        int useCount;
    };
    /// hold information about anims
    struct AnimInfo
    {
        nString filename;
        nString name;
        int fileSize;
        int ramSize;
        int numGroups;
        int numKeys;
        int useCount;
    };

    /// hold information about a Nebula2 object
    struct ObjectInfo
    {
        nString filename;
        nString name;
        int fileSize;
        int numMaterials;
        int textureSize;
        int meshSize;
        int animSize;
        int useCount;
        bool error;
        nArray<nString> missingTextures;
        nArray<int> textureIndices;
        nArray<int> meshIndices;
        nArray<int> animIndices;
    };
    /// hold information about a level
    struct LevelInfo
    {
        nString name;
        bool error;
        int numObjects;
        int numUniqueObjects;
        int textureSize;
        int meshSize;
        int animSize;
        nArray<int> objectIndices;
        nArray<int> textureIndices;
        nArray<int> meshIndices;
        nArray<int> animIndices;
        nArray<nString> missingObjects;
    };

    nString error;
    nString dateAndTime;
    nString rootDir;
    nArray<TextureInfo> textures;
    nArray<MeshInfo> meshes;
    nArray<AnimInfo> anims;
    nArray<ObjectInfo> objects;
    nArray<LevelInfo> levels;
};
//------------------------------------------------------------------------------
#endif