#ifndef N_D3D9SHADERINCLUDE_H
#define N_D3D9SHADERINCLUDE_H
//------------------------------------------------------------------------------
/**
    Override default include handling in D3DX FX files.

    (C) 2004 RadonLabs GmbH
*/
#include "util/npathstring.h"
#include <d3dx9.h>

//------------------------------------------------------------------------------
class nD3D9ShaderInclude : public ID3DXInclude
{
public:
    /// constructor
    nD3D9ShaderInclude(const nPathString& sDir);
    /// open an include file and read its contents
    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    /// close an include file
    STDMETHOD(Close)(LPCVOID pData);

private:
    nPathString shaderDir;
};
//------------------------------------------------------------------------------
#endif