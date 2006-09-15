#ifndef N_GLSHADER_H
#define N_GLSHADER_H
//------------------------------------------------------------------------------
/**
    @class nGLShader
    @ingroup OpenGL

    @brief Base class for all GL shader systems (GLSL, CgFX, GL/ES, etc.)

    10-Apr-2005  Haron  created
*/
#include "gfx2/nshader2.h"

//------------------------------------------------------------------------------
class nGLShader : public nShader2
{
public:
    /// constructor
    nGLShader();
    /// destructor
    virtual ~nGLShader();
	
    /// initialize gl shader subsystem (GLSL, CgFX, GL/ES, etc.)
    virtual bool InitShaderSys();
    /// release gl shader subsystem
    virtual bool ReleaseShaderSys();
    /// set int parameter
    bool IsInitialized();

protected:
    bool ss_initialized;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGLShader::IsInitialized() const
{
    return this->ss_initialized;
}

//------------------------------------------------------------------------------
#endif
