//------------------------------------------------------------------------------
//  nshaderlist.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "gfx2/nshaderlist.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
/**
*/
nShaderList::nShaderList()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShaderList::~nShaderList()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Initialize the shader list from an ascii shader list file (which
    contains shaders sorted by their priority):

    # a comment
    shaders:default.fx
    shaders:default_skinned.fx
    ...

    @param  filename    filename of a shader list file
    @return             true if successful
*/
bool
nShaderList::Load(const char* filename)
{
    n_assert(filename);
    n_assert(this->shaders.Empty());
    nKernelServer* kernelServer = nKernelServer::Instance();
    nFileServer2* fileServer = kernelServer->GetFileServer();
    bool retval = false;

    // open the file
    nFile* file = fileServer->NewFileObject();
    if (file->Open(filename, "r"))
    {
        char line[1024];
        while (file->GetS(line, sizeof(line)))
        {
            // tokenize the current line
            nString str = line;
            str.Strip("\n\r");
            nArray<nString> tokens;
            str.Tokenize(N_WHITESPACE, tokens);
            if (tokens.Size() > 0)
            {
                if (tokens[0].Get()[0] != '#')
                {
                    this->shaders.Append(tokens[0]);
                    retval = true;
                }
            }
        }
        file->Close();
    }
    file->Release();
    file = 0;
    return retval;
}

//------------------------------------------------------------------------------
/**
    Searches shader array list for shader filenames (must be "shaders:xxx.fx")
    and returns the shader's render priority index.

    @param  shaderName  filename of shader
    @return             shader index, or -1 if shader doesn't exist
*/  
int
nShaderList::GetShaderIndex(const char* shaderName) const
{
    n_assert(shaderName);
    return this->shaders.FindIndex(shaderName);
}

//------------------------------------------------------------------------------
/**
    Clear shader list.
*/
void
nShaderList::Clear()
{
    this->shaders.Clear();
}
