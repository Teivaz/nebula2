#ifndef N_SHADERLIST_H
#define N_SHADERLIST_H
//------------------------------------------------------------------------------
/**
    @class nShaderList
    @ingroup NebulaGraphicsSystem
    
    Loads the file "shaders:shaderlist.txt" which contains the global list
    of shader files sorted by their render priority (which is used
    as the shader bucket index in the scene server).

    NOTE:

    The gfx server initializes a global instance of nShaderList
    at OpenDisplay(), and exposes shader indices with the
    nGfxServer::GetShaderIndex() method.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/narray.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nShaderList
{
public:
    /// constructor
    nShaderList();
    /// destructor
    ~nShaderList();
    /// load the shader list
    bool Load(const char* filename);
    /// clear the shader list
    void Clear();
    /// return true if initialized
    bool IsLoaded() const;
    /// get shader index by name
    int GetShaderIndex(const char* shaderName) const;

private:
    nArray<nString> shaders;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShaderList::IsLoaded() const
{
    return (this->shaders.Size() > 0);
}

//------------------------------------------------------------------------------
#endif    


