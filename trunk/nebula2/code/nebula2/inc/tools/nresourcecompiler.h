#ifndef N_RESOURCECOMPILER_H
#define N_RESOURCECOMPILER_H
//------------------------------------------------------------------------------
/**
    @class nResourceCompiler
    @ingroup Tools

    A resource compiler class. Takes a number of .N2 files as input
    and generates one big .N2 file and an associated resource file which
    contains the meshes, animations and textures.

    Resource Bundle fileformat (.nrb extension):

    uint magic = 'NRB0'
    uint numTocEntries

    TocEntry
    {
        uint type = 'MESH' | 'TXTR' | 'MANI' | 'SANI'   (Mesh, Texture, MemoryAnim, StreamingAnim)
        char name[32];      // pre-mangled resource name (see nResourceServer::GetResourceId())
        uint start;         // data offset from beginning of file
        uint length;        // number of bytes in data block
        uint flags;         // type specific flags
    }

    DataBlock
    {
        ... collection of nvx2, nax2 and dds files ...
    }

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "util/narray.h"
#include "util/nstring.h"
#include "resource/nresource.h"
#include "kernel/nfile.h"
#include "resource/nresourceserver.h"
#include <stdarg.h>

//------------------------------------------------------------------------------
class nResourceCompiler
{
public:
    /// path identifiers
    enum PathIdentifier
    {
        BaseFilename = 0,
        ProjectDirectory,
        ScratchDirectory,

        NumPathIdentifiers,
    };

    /// constructor
    nResourceCompiler();
    /// destructor
    ~nResourceCompiler();
    /// set a path
    void SetPath(PathIdentifier pathId, const nString& name);
    /// get a path
    const char* GetPath(PathIdentifier pathId) const;
    /// set binary save flag
    void SetBinaryFlag(bool b);
    /// get binary save flag
    bool GetBinaryFlag() const;
    /// initialize the resource compiler (setup Nebula, etc...)
    bool Open(nKernelServer* ks);
    /// do the resource compilation, takes an array of Nebula object filenames)
    bool Compile(const nArray<nString>& objs);
    /// close the resource compiler
    void Close();
    /// get size of mesh data in bytes
    int GetMeshDataSize() const;
    /// get size of anim data in bytes
    int GetAnimDataSize() const;
    /// get size of texture data in bytes
    int GetTextureDataSize() const;
    /// get the last error
    const char* GetError() const;

private:
    /// a resource entry class
    class ResEntry
    {
    public:
        /// constructor
        ResEntry();
        /// constructor with name
        ResEntry(nResource::Type t, const char* name);
        /// destructor
        ~ResEntry();
        /// set resource type
        void SetType(nResource::Type t);
        /// get resource type
        nResource::Type GetType() const;
        /// set resource name
        void SetName(const char* name);
        /// get resource name
        const char* GetName() const;
        /// set resource data start offset
        void SetDataOffset(int i);
        /// get resource data start offset
        int GetDataOffset() const;
        /// set resource data length
        void SetDataLength(int i);
        /// get resource data length
        int GetDataLength() const;
        /// set flags field
        void SetFlags(int f);
        /// get flags field
        int GetFlags() const;
        /// operator ==
        bool operator==(const ResEntry& rhs);

    private:
        nResource::Type type;
        nString name;
        int dataOffset;
        int dataLength;
        int flags;
    };

    /// set error string
    void __cdecl SetError(const char* error, ...);
    /// load Nebula2 objects
    bool LoadObjects(const nArray<nString>& objs);
    /// extract resource names from loaded N2 objects
    void ExtractResourceAttrs(nRoot* curNode);
    /// save Nebula2 object hierarchy
    bool SaveObjectHierarchy();
    /// process mesh resources
    bool ProcessMeshes();
    /// process anim resources
    bool ProcessAnims();
    /// process texture resources
    bool ProcessTextures();
    /// write a single resource entry to the resource bundle file
    bool WriteResEntry(nFile* file, const ResEntry& resEntry, int dataBlockOffset);
    /// save the resulting resource bundle file
    bool SaveResourceBundle();

    nKernelServer* kernelServer;
    nAutoRef<nResourceServer> refResourceServer;
    nString pathArray[NumPathIdentifiers];
    bool isOpen;
    nArray<ResEntry> meshes;        // mesh resource names
    nArray<ResEntry> textures;      // texture resource names
    nArray<ResEntry> anims;         // anim resource names
    nRoot* rootNode;                // pointer to resource root node
    nClass* shapeNodeClass;
    nClass* shaderNodeClass;
    nClass* skinAnimatorClass;
    bool binaryFlag;
    nString dataFilePath;
    nFile* dataFile;                // temporary data file
    nString error;
};

//------------------------------------------------------------------------------
/**
*/
inline
nResourceCompiler::ResEntry::ResEntry() :
    type(nResource::InvalidResourceType),
    dataOffset(0),
    dataLength(0),
    flags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nResourceCompiler::ResEntry::ResEntry(nResource::Type t, const char* n) :
    type(t),
    name(n),
    dataOffset(0),
    dataLength(0),
    flags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nResourceCompiler::ResEntry::~ResEntry()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::ResEntry::SetType(nResource::Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nResource::Type
nResourceCompiler::ResEntry::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::ResEntry::SetName(const char* n)
{
    n_assert(n);
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nResourceCompiler::ResEntry::GetName() const
{
    return this->name.IsEmpty() ? 0 : this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::ResEntry::SetDataOffset(int i)
{
    this->dataOffset = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nResourceCompiler::ResEntry::GetDataOffset() const
{
    return this->dataOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::ResEntry::SetDataLength(int i)
{
    this->dataLength = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nResourceCompiler::ResEntry::GetDataLength() const
{
    return this->dataLength;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResourceCompiler::ResEntry::operator==(const nResourceCompiler::ResEntry& rhs)
{
    return (this->name == rhs.name);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::ResEntry::SetFlags(int f)
{
    this->flags = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nResourceCompiler::ResEntry::GetFlags() const
{
    return this->flags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::SetPath(PathIdentifier pathId, const nString& name)
{
    n_assert(pathId < NumPathIdentifiers);
    this->pathArray[pathId] = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nResourceCompiler::GetPath(PathIdentifier pathId) const
{
    return this->pathArray[pathId].IsEmpty() ? 0 : this->pathArray[pathId].Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nResourceCompiler::SetBinaryFlag(bool b)
{
    this->binaryFlag = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nResourceCompiler::GetBinaryFlag() const
{
    return this->binaryFlag;
}

//------------------------------------------------------------------------------
/**
*/
inline
void __cdecl
nResourceCompiler::SetError(const char* err, ...)
{
    va_list argList;
    va_start(argList, err);
    char msg[1024];
    vsnprintf(msg, sizeof(msg), err, argList);
    this->error = msg;
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nResourceCompiler::GetError() const
{
    return this->error.Get();
}

//------------------------------------------------------------------------------
#endif

