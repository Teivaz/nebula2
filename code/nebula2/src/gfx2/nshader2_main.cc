//------------------------------------------------------------------------------
//  nshader2_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gfx2/nshader2.h"
#include "kernel/nkernelserver.h"

nNebulaClass(nShader2, "resource::nresource");

//------------------------------------------------------------------------------
/**
*/
nShader2::nShader2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShader2::~nShader2()
{
    if (!this->IsUnloaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Create or update the instance stream declaration for this shader.
    Stream components will be appended, unless they already exist in the
    declaration. Returns the number of components appended.
    Override this method in a subclass.
*/
int
nShader2::UpdateInstanceStreamDecl(nInstanceStream::Declaration& /*decl*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShader2::IsParameterUsed(nShaderState::Param /*p*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetBool(nShaderState::Param /*p*/, bool /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetBoolArray(nShaderState::Param /*p*/, const bool* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetInt(nShaderState::Param /*p*/, int /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetIntArray(nShaderState::Param /*p*/, const int* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat(nShaderState::Param /*p*/, float /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloatArray(nShaderState::Param /*p*/, const float* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector4(nShaderState::Param /*p*/, const vector4& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector3(nShaderState::Param /*p*/, const vector3& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat4(nShaderState::Param /*p*/, const nFloat4& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetFloat4Array(nShaderState::Param /*p*/, const nFloat4* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetVector4Array(nShaderState::Param /*p*/, const vector4* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrix(nShaderState::Param /*p*/, const matrix44& /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixArray(nShaderState::Param /*p*/, const matrix44* /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetMatrixPointerArray(nShaderState::Param /*p*/, const matrix44** /*array*/, int /*count*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetTexture(nShaderState::Param /*p*/, nTexture2* /*val*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::SetParams(const nShaderParams& /*params*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
nShader2::Begin(bool /*saveState*/)
{
    // empty, implement this method in a subclass
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::BeginPass(int /*pass*/)
{
    // empty, implement this method in a subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::CommitChanges()
{
    // empty, implement this method in a subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::EndPass()
{
    // empty, implement this method in a subclass
}

//------------------------------------------------------------------------------
/**
*/
void
nShader2::End()
{
    // empty, implement this method in a subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
nShader2::HasTechnique(const char* /*t*/) const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShader2::SetTechnique(const char* /*t*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nShader2::GetTechnique() const
{
    return 0;
}
