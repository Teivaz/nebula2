#ifndef N_D3D9FONT_H
#define N_D3D9FONT_H
//------------------------------------------------------------------------------
/**
    @class nD3D9Font

    A D3D9 specific subclass of nFont2.
    
    (C) 2003 RadonLabs GmbH
*/
#include <d3dx9.h>
#include "gfx2/nfont2.h"
#include "kernel/nautoref.h"

class nD3D9Server;

//------------------------------------------------------------------------------
class nD3D9Font : public nFont2
{
public:
    /// constructor
    nD3D9Font();
    /// destructor
    virtual ~nD3D9Font();
    /// access to the internal CD3DFont object.
    CD3DFont9* GetD3DFont() const;

    static nKernelServer* kernelServer;

protected:
    /// load resource
    virtual bool LoadResource();
    /// unload resource
    virtual void UnloadResource();

private:
    nAutoRef<nD3D9Server> refD3D9Server;
    CD3DFont9* d3dFont;
};

//------------------------------------------------------------------------------
/**
*/
inline
CD3DFont9*
nD3D9Font::GetD3DFont() const
{
    n_assert(this->d3dFont);
    return this->d3dFont;
}

//------------------------------------------------------------------------------
#endif
