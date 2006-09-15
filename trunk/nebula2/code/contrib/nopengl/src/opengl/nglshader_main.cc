//------------------------------------------------------------------------------
//  nglslshader_main.cc
//  23-Mar-2005 Haron
//------------------------------------------------------------------------------
#include "opengl/nglshader.h"

nNebulaClass(nGLShader, "nshader2");

//------------------------------------------------------------------------------
/**
*/
nGLShader::nGLShader() :
    ss_initialized(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGLShader::~nGLShader()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    initialize gl shader subsystem
*/
bool
nGLShader::InitShaderSys()
{
    n_error("nGLShader::InitShaderSys(): Pure virtual function called!");
    return false;
}

//------------------------------------------------------------------------------
/**
    release gl shader subsystem
*/
bool
nGLShader::ReleaseShaderSys()
{
    n_error("nGLShader::ReleaseShaderSys(): Pure virtual function called!");
    return false;
}

