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

namespace nPassState
{
    //------------------------------------------------------------------------------
    /**
        Shader pass parameters.
        NOTE: don't forget to update the string table in npassstate.cc after
        adding or removing shader states!
    */
    enum Param
    {
        ZWriteEnable = 0,               //
        ZEnable,                        //
        ZFunc,                          //
        ColorWriteEnable,               //
        AlphaBlendEnable,               //
        SrcBlend,                       //
        DestBlend,                      //
        AlphaTestEnable,                //
        StencilEnable,                  //
        CullMode,                       //

        NumParameters,                  // keep this always at the end!
        InvalidParameter,
    };

    /// convert parameter enum to string
    const char* ParamToString(Param p);
    /// convert string to parameter
    Param StringToParam(const char* str);

} // namespace nPassState
//------------------------------------------------------------------------------
#endif
