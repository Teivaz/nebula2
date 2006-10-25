#ifndef N_CMDPROTONATIVECPP_H
#define N_CMDPROTONATIVECPP_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaScriptServices
    @ingroup NebulaScriptAutoWrapper
    @class nCmdProtoNativeCPP

    nCmdProtoNativeCPP classes are used to make easier thet definition of the script
    commands in Nebula classes (programmed in C++). It avoids to have to program
    the wrapper manually in plain C language.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/ncmdproto.h"
#include "kernel/ncmdprototraits.h"

//#include "kernel/nroot.h"
//#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/ncmdargtypes.h"

//------------------------------------------------------------------------------
/**
    @def NCMDPROTONATIVECPP_INITCMDS_BEGIN(ClassName)
    @brief Begin n_initcmds function & cl->BeginCmds()
    @param ClassName Name of the C++ class having the commands.
*/
#define NCMDPROTONATIVECPP_INITCMDS_BEGIN(ClassName)         \
void                                                        \
n_initcmds(nClass* cl)                                      \
{                                                           \
    typedef ClassName ScriptClass_;                         \
    cl->BeginCmds();

/**
    @def NCMDPROTONATIVECPP_ADDCMD(MemberName)
    @brief Adds the command to the nClass in n_initcmds functions.
    @param MemberName       name of the C++ member function
*/
#define NCMDPROTONATIVECPP_ADDCMD(MemberName)                     \
    ScriptClass_::AddCmd_ ## MemberName<ScriptClass_>(cl);

/**
    @def NCMDPROTONATIVECPP_INITCMDS_END()
    @brief End of n_initcmds function & cl->EndCmds()
*/
#define NCMDPROTONATIVECPP_INITCMDS_END()                         \
    cl->EndCmds();                                          \
}

//------------------------------------------------------------------------------

/**
    @def NCMDPROTONATIVECPP_DECLARE_BEGIN(ClassName)
    @brief Start the group of scripting commands.
    @param ClassName Name of the C++ class where scripting commands are declared.
*/
#define NCMDPROTONATIVECPP_DECLARE_BEGIN(ClassName)   typedef ClassName ScriptClass_;

/**
    @def NCMDPROTONATIVECPP_DECLARE_END(ClassName)
    @brief End of commands declaration.
    @param ClassName Name of the C++ class where scripting commands are declared.
*/
#define NCMDPROTONATIVECPP_DECLARE_END(ClassName)

/**
    @def NCMDPROTONATIVECPP_DECLARE_NAME(FourCC,NAME,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)
    @brief Declare a C++ function with input and/or output arguments (generic).
    @param FourCC       fourcc code assigned to the scripting function
    @param NAME         name of the scripting command
    @param PREFIX       function prefix to allow definition of virtual, static, ...
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMINPARAM   number of input parameters
    @param INPARAM      list of input parameter types between parenthesis and separated by commas
    @param NUMOUTPARAM  number of output parameters
    @param OUTPARAM     list of output parameter types between parenthesis and separated by commas
*/
#define NCMDPROTONATIVECPP_DECLARE_NAME(FourCC,NAME,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)                           \
    NCMDPROTONATIVECPP_DECLARE_ADDCMD(FourCC,NAME,TR,MemberName,TYPELIST_ ## NUMINPARAM ## INPARAM,TYPELIST_ ## NUMOUTPARAM ## OUTPARAM)    \
    PREFIX TR MemberName(                                                   \
        NCMDPROTO_PROTO_IN ## NUMINPARAM ## INPARAM                         \
        NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN(NUMINPARAM,NUMOUTPARAM,OUTPARAM)  \
    )

/**
    @def NCMDPROTONATIVECPP_DECLARE(FourCC,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)
    @brief Declare a C++ function with input and/or output arguments (generic).
    @param FourCC       fourcc code assigned to the scripting function
    @param PREFIX       function prefix to allow definition of virtual, static, ...
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMINPARAM   number of input parameters
    @param INPARAM      list of input parameter types between parenthesis and separated by commas
    @param NUMOUTPARAM  number of output parameters
    @param OUTPARAM     list of output parameter types between parenthesis and separated by commas
*/
#define NCMDPROTONATIVECPP_DECLARE(FourCC,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)                                      \
    NCMDPROTONATIVECPP_DECLARE_ADDCMD(FourCC,MemberName,TR,MemberName,TYPELIST_ ## NUMINPARAM ## INPARAM,TYPELIST_ ## NUMOUTPARAM ## OUTPARAM)   \
    PREFIX TR MemberName(                                                   \
        NCMDPROTO_PROTO_IN ## NUMINPARAM ## INPARAM                         \
        NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN(NUMINPARAM,NUMOUTPARAM,OUTPARAM)  \
    )

/**
    @def NCMDPROTONATIVECPP_DECLARE_NOARG(FourCC,PREFIX,TR,MemberName)
    @brief Declare a C++ function without arguments.
    @param FourCC       fourcc code assigned to the scripting function
    @param PREFIX       function prefix to allow definition of virtual, static, ...
    @param TR           return type
    @param MemberName   name of the function in C++
*/
#define NCMDPROTONATIVECPP_DECLARE_NOARG(FourCC,PREFIX,TR,MemberName)                \
    NCMDPROTONATIVECPP_DECLARE(FourCC,PREFIX,TR,MemberName,0,(),0,())

/**
    @def NCMDPROTONATIVECPP_DECLARE_INARG(FourCC,PREFIX,TR,MemberName,NUMPARAM,PARAM)
    @brief Declare a C++ function with input arguments.
    @param FourCC       fourcc code assigned to the scripting function
    @param PREFIX       function prefix to allow definition of virtual, static, ...
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMPARAM     number of input parameters
    @param PARAM        list of input parameter types between parenthesis and separated by commas
*/
#define NCMDPROTONATIVECPP_DECLARE_INARG(FourCC,PREFIX,TR,MemberName,NUMPARAM,PARAM) \
    NCMDPROTONATIVECPP_DECLARE(FourCC,PREFIX,TR,MemberName,NUMPARAM,PARAM,0,())

/**
    @def NCMDPROTONATIVECPP_DECLARE_OUTARG(FourCC,PREFIX,TR,MemberName,NUMOUTPARAM,OUTPARAM)
    @brief Declare a C++ function with with output arguments
    @param FourCC       fourcc code assigned to the scripting function
    @param PREFIX       function prefix to allow definition of virtual, static, ...
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMOUTPARAM  number of output parameters
    @param OUTPARAM     list of output parameter types between parenthesis and separated by commas
*/
#define NCMDPROTONATIVECPP_DECLARE_OUTARG(FourCC,PREFIX,TR,MemberName,NUMOUTPARAM,OUTPARAM)      \
    NCMDPROTONATIVECPP_DECLARE(FourCC,PREFIX,TR,MemberName,0,(),NUMOUTPARAM,OUTPARAM)

/**
    @def NCMDPROTONATIVECPP_DECLARE_INOUTARG(FourCC,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)
    @brief Declare a C++ function with input & output arguments.
    @param FourCC       fourcc code assigned to the scripting function
    @param PREFIX       function prefix to allow definition of virtual, static, ...
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMINPARAM   number of input parameters
    @param INPARAM      list of input parameter types between parenthesis and separated by commas
    @param NUMOUTPARAM  number of output parameters
    @param OUTPARAM     list of output parameter types between parenthesis and separated by commas
*/
#define NCMDPROTONATIVECPP_DECLARE_INOUTARG(FourCC,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)   \
    NCMDPROTONATIVECPP_DECLARE(FourCC,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)

/*
    Auxiliar helper macro used internally in declarations.
*/
#define NCMDPROTONATIVECPP_DECLARE_ADDCMD(FourCC,NAME,TR,MemberName,TLIN,TLOUT)    \
template <class TClassCast>                                             \
inline static void AddCmd_ ## NAME (nClass * cl)                        \
{                                                                       \
    char ncmd_signature[N_MAXPATH];                                     \
    unsigned int fourcc = FourCC;                                       \
    typedef nCmdProtoNativeCPP< ScriptClass_, TR, TLIN, TLOUT, TClassCast > \
        TCmdProtoNativeCPP;                                             \
    TCmdProtoNativeCPP::Traits::CalcPrototype(                          \
        ncmd_signature, #NAME);                                         \
    fourcc = TCmdProtoNativeCPP::Traits::CalcFourCC(                    \
        #NAME, fourcc);                                                 \
    TCmdProtoNativeCPP * val =                                          \
        n_new( TCmdProtoNativeCPP(                                      \
            ncmd_signature,fourcc,&ScriptClass_::MemberName)            \
        );                                                              \
    cl->AddCmd(val);                                                    \
}

//------------------------------------------------------------------------------

#pragma warning (disable : 4127 4100)

template < class TClass, class TR, class TListIn, class TListOut, class TClassCast >
class nCmdProtoNativeCPP : public nCmdProto
{
public:

    typedef nCmdProtoTraits<TR,TListIn,TListOut> Traits;

    typedef typename Traits::TCmdDispatcher<TClass> TDispatcher;

    /// constructor
    nCmdProtoNativeCPP(const char * signature, int fourcc, typename TDispatcher::TMemberFunction memf) :
        nCmdProto(signature, fourcc),
        memf_(memf)
    {
        /// empty
    }

    /// constructor with const method
    nCmdProtoNativeCPP(const char * signature, int fourcc, typename TDispatcher::TMemberFunctionConst memf) :
        nCmdProto(signature, fourcc),
        memf_(reinterpret_cast<typename TDispatcher::TMemberFunction> (memf))
    {
        /// empty
    }

    /// destructor
    virtual ~nCmdProtoNativeCPP()
    {
        /// empty
    }

    /// dispatch command
    virtual bool Dispatch(void * slf, nCmd * cmd)
    {
        TClass * obj = reinterpret_cast<TClassCast *> (slf);
        return TDispatcher::Dispatch(obj, memf_, cmd);
    }

private:

    typename TDispatcher::TMemberFunction memf_;

};

#pragma warning (default : 4127 4100)

//------------------------------------------------------------------------------

#endif
