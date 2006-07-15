#ifndef N_CGFXSHADERINCLUDE_H
#define N_CGFXSHADERINCLUDE_H
//------------------------------------------------------------------------------
/**
	@class nCgFXShaderInclude
	@ingroup OpenGL

    '#include' statement resolver for CgFX shader

    2004 Oleg Kreptul (Haron)
*/
#include "util/nstring.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nCgFXShaderInclude
{
public:
    /// constructor
    nCgFXShaderInclude();
    /// destructor
    virtual ~nCgFXShaderInclude();
	
	/// 
    bool Begin(const nString& sourceFile);
    ///
    nString& GetFileName();
    /// 
    void End();

protected:
	/// equivalent for nD3D9ShaderInclude class
	bool Include(nFile* dstfile, const nString& srcfile, nArray<nString>& includes);

    nString tmpFilePath;
};

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nCgFXShaderInclude::GetFileName()
{
    return this->tmpFilePath;
}

//------------------------------------------------------------------------------
#endif

