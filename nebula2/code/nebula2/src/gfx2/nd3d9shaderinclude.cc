//------------------------------------------------------------------------------
//  nd3d9shaderinclude.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "gfx2/nd3d9shaderinclude.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
/**
*/
nD3D9ShaderInclude::nD3D9ShaderInclude(const nString& sDir)
{
    this->shaderDir = sDir + "/";
}

//------------------------------------------------------------------------------
/**
*/
HRESULT
nD3D9ShaderInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
    nFile* file = nKernelServer::Instance()->GetFileServer()->NewFileObject();
    if (!file)
    {
        return E_FAIL;
    }

    // open the file
    // try absolute path first
    if (!file->Open(pName, "r"))
    {
        // try in shader dir
        nString filePath = this->shaderDir + pName;
        if (!file->Open(filePath, "r"))
        {
            n_printf("nD3D9Shader: could not open include file '%s' nor '%s'!\n", pName, filePath.Get());
            file->Release();
            return E_FAIL;
        }
    }

    // allocate data for file and read it
    int fileSize = file->GetSize();
    void* buffer = n_malloc(fileSize);
    if (!buffer)
    {
        file->Release();
        return E_FAIL;
    }
    file->Read(buffer, fileSize);

    *ppData = buffer;
    *pBytes = fileSize;

    file->Close();
    file->Release();

    return S_OK;
}

//------------------------------------------------------------------------------
/**
*/
HRESULT
nD3D9ShaderInclude::Close(LPCVOID pData)
{
    n_free((void*)pData);
    return S_OK;
}
