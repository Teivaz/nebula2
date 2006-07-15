#ifndef N_GLFONT_H
#define N_GLFONT_H
//------------------------------------------------------------------------------
/**
    @class nGLFont
    @ingroup OpenGL

    An OpenGL specific subclass of nFont2.
*/
#include "gfx2/nfont2.h"
#include "kernel/nautoref.h"

//------------------------------------------------------------------------------
class nGLFont : public nFont2
{
public:
    /// constructor
    nGLFont();
    /// destructor
    virtual ~nGLFont();

protected:
    /// load resource
    virtual bool LoadResource();
    /// unload resource
    virtual void UnloadResource();
};

//------------------------------------------------------------------------------
#endif
