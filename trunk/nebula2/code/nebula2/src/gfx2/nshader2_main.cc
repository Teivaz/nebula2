#define N_IMPLEMENTS nShader2
//------------------------------------------------------------------------------
//  nshader2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nshader2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nShader2, "nresource");

//------------------------------------------------------------------------------
/**
*/
nShader2::nShader2() :
    refVariableServer("/sys/servers/variable")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShader2::~nShader2()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetInt(nVariable::Handle h, int val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetIntArray(nVariable::Handle h, const int* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat(nVariable::Handle h, float val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloatArray(nVariable::Handle h, const float* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector(nVariable::Handle h, const float4& val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVectorArray(nVariable::Handle h, const float4* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrix(nVariable::Handle h, const matrix44& val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixArray(nVariable::Handle h, const matrix44* array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixPointerArray(nVariable::Handle h, const matrix44** array, int count)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetTexture(nVariable::Handle varHandle, nTexture2* val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nShader2::IsParameterUsed(nVariable::Handle varHandle)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
int
nShader2::Begin()
{
    // empty
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::Pass(int /*pass*/)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
nShader2::End()
{
    // empty
}

