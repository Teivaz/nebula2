//------------------------------------------------------------------------------
//  nglfont_main.cc
//------------------------------------------------------------------------------
#include "opengl/nglfont.h"

nNebulaClass(nGLFont, "gfx2::nfont2");

//------------------------------------------------------------------------------
/**
*/
nGLFont::nGLFont()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGLFont::~nGLFont()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nGLFont::LoadResource()
{
    this->SetValid(true);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGLFont::UnloadResource()
{
    this->SetValid(false);
}

