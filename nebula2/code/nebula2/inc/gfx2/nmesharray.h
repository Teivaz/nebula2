#ifndef N_MESHARRAY_H
#define N_MESHARRAY_H
//------------------------------------------------------------------------------
/**
    @class nMeshArray
    @ingroup NebulaGraphicsSystem

    Holds an array of up to nGfxServer2::MaxVertexStreams meshes.
    Can be posted to the nGfxServer to assign all streams to the graphics
    device.
    
    nMeshArray is normally a superclass for Gfx API specific derived classes, like
    Direct3D or OpenGL.

    (C) 2004 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"

class nVariableServer;

//------------------------------------------------------------------------------
class nMeshArray : public nResource
{
public:
    /// constructor
    nMeshArray();
    /// destructor
    virtual ~nMeshArray();

    /// set absolute path to resource file for index
    virtual void SetFilenameAt(int index, const nString& path);
    /// get absolute path to resource file for index
    const nString& GetFilenameAt(int index) const;
    
    ///get the mesh object at index
    nMesh2* GetMeshAt(int index) const;
    /// set the mesh object at index
    virtual void SetMeshAt(int index, nMesh2* mesh);
    
    /// set the mesh use type
    void SetUsageAt(int index, int useFlags);
    /// get the mesh use type
    int GetUsageAt(int index) const;

protected:
    /// override in subclasse to perform actual resource loading
    virtual bool LoadResource();
    /// override in subclass to perform actual resource unloading
    virtual void UnloadResource();

    nArray<int> usages;
    nArray<nRef<nMesh2> >refMeshes;
    nArray<nString> filenames;
};

//------------------------------------------------------------------------------
/**
    Set the mesh filename for the specified stream.

    @param  index index of stream the mesh shall be used for
    @param  filename    the absolute path to the resource file
*/
inline
void
nMeshArray::SetFilenameAt(int index, const nString& path)
{
    this->filenames[index] = path;
    this->SetValid(false);
}

//------------------------------------------------------------------------------
/**
    Get the mesh for the specified stream.

    @param  streamIndex index of stream the mesh shall be used for
    @return             current mesh on that stream
*/
inline
const nString&
nMeshArray::GetFilenameAt(int index) const
{
    return this->filenames[index];
}

//------------------------------------------------------------------------------
/**
    only valid inbetween of Load-/UnloadResource(),
*/
inline
nMesh2*
nMeshArray::GetMeshAt(int index) const
{
    if (this->refMeshes[index].isvalid())
    {
        return this->refMeshes[index].get();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshArray::SetUsageAt(int index, int useFlags)
{
    this->usages[index] = useFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshArray::GetUsageAt(int index) const
{
    return this->usages[index];
}
//------------------------------------------------------------------------------
#endif
