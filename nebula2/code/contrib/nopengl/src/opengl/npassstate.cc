//------------------------------------------------------------------------------
//  npassstate.cc
//  (C) 2006 Oleg Khryptul (Haron)
//------------------------------------------------------------------------------

#include "opengl/npassstate.h"
#include <string.h>

/// the shader parameter name string table
static const char* ParamTable[nPassState::NumParameters] =
{
    "ZWriteEnable",
    "ZEnable",
    "ZFunc",
    "ColorWriteEnable",
    "AlphaBlendEnable",
    "SrcBlend",
    "DestBlend",
    "AlphaTestEnable",
    "StencilEnable",
    "CullMode",
};

//------------------------------------------------------------------------------
/**
*/
const char*
nPassState::ParamToString(nPassState::Param p)
{
    n_assert((p >= 0) && (p < nPassState::NumParameters));
    return ParamTable[p];
}

//------------------------------------------------------------------------------
/**
*/
nPassState::Param
nPassState::StringToParam(const char* str)
{
    n_assert(str);
    int i;
    for (i = 0; i < nPassState::NumParameters; i++)
    {
        if (0 == strcmp(str, ParamTable[i]))
        {
            return (nPassState::Param) i;
        }
    }
    // fallthrough: state not found
    return nPassState::InvalidParameter;
}
