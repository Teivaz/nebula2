#ifndef N_CMDARGTYPES_H
#define N_CMDARGTYPES_H
//------------------------------------------------------------------------------
/**
    @file ncmdargtypes.h
    @ingroup Kernel

    Command argument templates for each type.

    Normal types like bool, int, float, char *, nObject * (void *)
    are handled, and their respective const versions.
    Their pointer versions are not handled, there is no enough information
    to decide if handle them as a single value or as an array of values (and
    with which dimension). Anyway, it is not very usual to define types that
    way for functions aimed for scripting. In this case, it is safer to do
    the scripting wrapper manually (as before).

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ncmd.h"

//------------------------------------------------------------------------------
class nObject;
class nRoot;

//------------------------------------------------------------------------------
#define N_CMDARGTYPE_NEW_TYPE(TYPE, SIGNATURE, GETCMD, SETCMD)   \
    N_CMDARGTYPE_NEW_SIGNATURE(TYPE, SIGNATURE)                  \
    N_CMDARGTYPE_NEW_GETCMD(TYPE, GETCMD)                        \
    N_CMDARGTYPE_NEW_SETCMD(TYPE, SETCMD)

    //N_CMDARGTYPE_NEW_SIGNATURE(const TYPE, SIGNATURE)
/*
    This compiles in VS8, but not in VS7.1 (error C2912) for basic types:
    N_CMDARGTYPE_NEW_SETCMD(const TYPE, SETCMD)

*/

#define N_CMDARGTYPE_NEW_SIGNATURE(TYPE, SIGNATURE) \
template <>                                         \
inline                                              \
const char * nGetSignatureStr<TYPE>()               \
{                                                   \
    return SIGNATURE;                               \
};

#define N_CMDARGTYPE_NEW_GETCMD(TYPE, GETCMD)                       \
template <>                                                         \
inline                                                              \
void                                                                \
nGetCmdArg<TYPE>( nCmd * cmd, n_remove_const<TYPE>::Type & value )  \
{                                                                   \
    (GETCMD);                                                       \
};

#define N_CMDARGTYPE_NEW_SETCMD(TYPE, SETCMD)                           \
template <>                                                             \
inline                                                                  \
void                                                                    \
nSetCmdArg<TYPE>( nCmd * cmd, n_remove_const<TYPE>::Type & value )    \
{                                                                       \
    (SETCMD);                                                           \
};

//------------------------------------------------------------------------------
template <typename T>
struct n_remove_const
{
    typedef T Type;
};

template <typename T>
struct n_remove_const<const T>
{
    typedef T Type;
};

template <typename T>
struct n_remove_const<const T *>
{
    typedef T * Type;
};

template <typename T>
struct n_remove_pointer
{
    typedef T Type;
};

template <typename T>
struct n_remove_pointer<T *>
{
    typedef T Type;
};

template <>
struct n_remove_pointer<void *>
{
    typedef int Type;
};

template <typename T>
struct n_remove_reference
{
    typedef T Type;
};

template <typename T>
struct n_remove_reference<T &>
{
    typedef T Type;
};

//------------------------------------------------------------------------------

template <typename T>
struct n_space_for_pointers
{
    n_space_for_pointers(T & /* inarg */) {}
    ~n_space_for_pointers() {}
};

template <>
struct n_space_for_pointers<void *>
{
    n_space_for_pointers(void * & /*inarg*/) {}
    ~n_space_for_pointers() {}
};

template <>
struct n_space_for_pointers<nObject *>
{
    n_space_for_pointers(nObject * & /*inarg*/) {}
    ~n_space_for_pointers() {}
};

template <>
struct n_space_for_pointers<nRoot *>
{
    n_space_for_pointers(nRoot * & /*inarg*/) {}
    ~n_space_for_pointers() {}
};

template <typename T>
struct n_space_for_pointers<T &>
{
    n_space_for_pointers(T & /*inarg*/) {};
    ~n_space_for_pointers() {};
};

template <typename T>
struct n_space_for_pointers<T *>
{
    n_space_for_pointers(T * & inarg) { inarg = &space; };
    ~n_space_for_pointers() {};
    T space;
};

//------------------------------------------------------------------------------
template <typename T>
inline
const char * nGetSignatureStr()
{
    n_assert2(0, "nGetSignatureStr: Please define the type in ncmdargtypes.h");
    return NULL;
};

template <typename T>
inline
void
nGetCmdArg( nCmd * cmd, T & ret )
{
    n_assert2(0, "nGetCmdArg: Please define the type in ncmdargtypes.h");
};

template <typename T>
inline
void
nSetCmdArg( nCmd * cmd, T & val )
{
    n_assert2(0, "nSetCmdArg: Please define the type in ncmdargtypes.h");
};

//------------------------------------------------------------------------------

// Special case for void
N_CMDARGTYPE_NEW_SIGNATURE(void, "v");

// Basic standard types
N_CMDARGTYPE_NEW_TYPE(bool, "b", ( value = (bool) (cmd->In()->GetB()) ), ( cmd->Out()->SetB(value)) );
N_CMDARGTYPE_NEW_TYPE(int, "i", (value = (int) cmd->In()->GetI()), (cmd->Out()->SetI(value))  );
N_CMDARGTYPE_NEW_TYPE(float, "f", (value = (float) cmd->In()->GetF()), (cmd->Out()->SetF(value)) );
N_CMDARGTYPE_NEW_TYPE(char *, "s", (value = (char *) cmd->In()->GetS()), (cmd->Out()->SetS(value)) );
N_CMDARGTYPE_NEW_TYPE(void *, "o", (value = (void *) cmd->In()->GetO()), (cmd->Out()->SetO((void *) value)) );
N_CMDARGTYPE_NEW_TYPE(nObject *, "o", (value = (nObject *) cmd->In()->GetO()), (cmd->Out()->SetO((nObject *) value)) );
N_CMDARGTYPE_NEW_TYPE(nRoot *, "o", (value = (nRoot *) cmd->In()->GetO()), (cmd->Out()->SetO((nRoot *) value)) );

//------------------------------------------------------------------------------
/**
    Declare new user types: vector2, vector3, vector4
*/

N_CMDARGTYPE_NEW_TYPE(nString, "s",
    (value.Set( (char *) cmd->In()->GetS()) ),
    (cmd->Out()->SetS( value.Get() ))
);

N_CMDARGTYPE_NEW_TYPE(nString *, "s",
    (value->Set( cmd->In()->GetS())),
    (cmd->Out()->SetS( value->Get()))
);

N_CMDARGTYPE_NEW_TYPE(vector2, "ff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF() ),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ) )
);

N_CMDARGTYPE_NEW_TYPE(vector2 *, "ff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF() ),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ) )
);

N_CMDARGTYPE_NEW_TYPE(vector3, "fff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF(), value.z = cmd->In()->GetF()),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ), cmd->Out()->SetF( value.z ))
);

N_CMDARGTYPE_NEW_TYPE(vector3 *, "fff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF(), value->z = cmd->In()->GetF()),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ), cmd->Out()->SetF( value->z ))
);

N_CMDARGTYPE_NEW_TYPE(vector4, "ffff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF(), value.z = cmd->In()->GetF(), value.w = cmd->In()->GetF()),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ), cmd->Out()->SetF( value.z ), cmd->Out()->SetF( value.w ))
);

N_CMDARGTYPE_NEW_TYPE(vector4 *, "ffff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF(), value->z = cmd->In()->GetF(), value->w = cmd->In()->GetF()),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ), cmd->Out()->SetF( value->z ), cmd->Out()->SetF( value->w ))
);

/**
    More could be defined:
    line2 ff ff
    line3 fff fff
    bbox3 fff fff
    plane ffff
    polar2 ff
    nFloat4, nFloat3, nFloat2, float2, float3, float4
*/

#endif
