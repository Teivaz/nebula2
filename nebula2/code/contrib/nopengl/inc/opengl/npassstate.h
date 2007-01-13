#ifndef N_PASSSTATE_H
#define N_PASSSTATE_H
//------------------------------------------------------------------------------
/**
    @namespace nPassState
    @ingroup OpenGL

    Shader pass state parameters and data types.

    (C) 2006 Oleg Khryptul (Haron)
*/
#include "kernel/ntypes.h"
#include "gfx2/nshaderstate.h"

namespace nPassState
{
    ////------------------------------------------------------------------------------
    ///**
    //    Shader pass parameter data types.
    //*/
    //enum Type
    //{
    //    Void,
    //    Bool,
    //    Int,
    //    Float,
    //    Float4,
    //    Matrix44,
    //    Texture,

    //    NumTypes,  // always keep this after all valid types!
    //    InvalidType
    //};

    //------------------------------------------------------------------------------
    /**
        Shader pass parameters.
    */
    enum Param
    {
#define DECL_PASS_PARAM(name, type) name,
#include "opengl/npassparams.h"
#undef DECL_PASS_PARAM

        NumParameters,                  // keep this always at the end!
        InvalidParameter,
    };

    /// convert parameter enum to string
    const char* ParamToString(Param p);
    /// convert parameter enum to type
    nShaderState::Type ParamToType(Param p);
    /// convert string to parameter
    Param StringToParam(const char* str);

} // namespace nPassState
//------------------------------------------------------------------------------
#endif
