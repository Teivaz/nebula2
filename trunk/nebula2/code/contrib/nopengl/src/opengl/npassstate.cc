//------------------------------------------------------------------------------
//  npassstate.cc
//  (C) 2006 Oleg Khryptul (Haron)
//------------------------------------------------------------------------------

#include "opengl/npassstate.h"
#include <string.h>

struct ParamInfo
{
    const char* name;
    nShaderState::Type type;
};

/// the shader parameter name string table
static ParamInfo ParamTable[nPassState::NumParameters] =
{
#define DECL_PASS_PARAM(name, type) { #name, nShaderState::type},
#include "opengl/npassparams.h"
#undef DECL_PASS_PARAM
};

//------------------------------------------------------------------------------
/**
*/
const char*
nPassState::ParamToString(nPassState::Param p)
{
    n_assert((p >= 0) && (p < nPassState::NumParameters));
    return ParamTable[p].name;
}

//------------------------------------------------------------------------------
/**
*/
nShaderState::Type
nPassState::ParamToType(nPassState::Param p)
{
    n_assert((p >= 0) && (p < nPassState::NumParameters));
    return ParamTable[p].type;
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
        if (0 == strcmp(str, ParamTable[i].name))
        {
            return (nPassState::Param) i;
        }
    }
    // fallthrough: state not found
    return nPassState::InvalidParameter;
}
