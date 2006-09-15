#ifndef N_GLSHADERINCLUDE_H
#define N_GLSHADERINCLUDE_H
//------------------------------------------------------------------------------
/**
	@class nGLShaderInclude
	@ingroup OpenGL

    '#include' statement resolver for GL shaders

    2004 Oleg Kreptul (Haron)
    10-Apr-2005 Haron Converted from nCgFXShaderInclude to nGLShaderInclude
*/
#include "util/nstring.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nGLShaderInclude
{
public:
    /// constructor
    nGLShaderInclude();
    /// destructor
    virtual ~nGLShaderInclude();
	
	/// resolve all includes recursively
    bool Begin(const nString& sourceFile);
    /// return temporary file name
    nString& GetFileName();
    /// get source
    void GetSource(nString& src);
    /// clean
    void End();

protected:
	/// equivalent for nD3D9ShaderInclude class
	bool Include(nFile* dstfile, const nString& srcfile, nArray<nString>& includes);

    nString tmpFilePath;
    nFile* tmpFile;
};

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nGLShaderInclude::GetFileName()
{
    return this->tmpFilePath;
}

//------------------------------------------------------------------------------
#endif
