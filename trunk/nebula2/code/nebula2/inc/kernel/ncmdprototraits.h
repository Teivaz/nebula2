#ifndef N_CMDPROTOSIGNATURE_H
#define N_CMDPROTOSIGNATURE_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaScriptServices
    @class nCmdProtoTraits

    nCmdProtoTraits class is a helper class helping with the automatic
    calculation of signatures, invocation of commands, etc.

    (C) 2004 Tragnarion Studios
*/
//------------------------------------------------------------------------------
#include "loki/TypeManip.h"
#include "loki/Typelist.h"

#include "kernel/ntypes.h"
#include "kernel/ncrc.h"
#include "kernel/ncmdargtypes.h"

//------------------------------------------------------------------------------
#ifndef TYPELIST_0
#define TYPELIST_0() Loki::NullType
#endif

//------------------------------------------------------------------------------
/**
    Simple macros to avoid repetition
*/

#define NCMDPROTO_REPEAT(NTIMES,MACRO)   NCMDPROTO_REPEAT ## NTIMES(MACRO)
#define NCMDPROTO_REPEAT0(MACRO)
#define NCMDPROTO_REPEAT1(MACRO)    MACRO(1)
#define NCMDPROTO_REPEAT2(MACRO)    MACRO(1) MACRO(2)
#define NCMDPROTO_REPEAT3(MACRO)    MACRO(1) MACRO(2) MACRO(3)
#define NCMDPROTO_REPEAT4(MACRO)    MACRO(1) MACRO(2) MACRO(3) MACRO(4)
#define NCMDPROTO_REPEAT5(MACRO)    MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5)
#define NCMDPROTO_REPEAT6(MACRO)    MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6)
#define NCMDPROTO_REPEAT7(MACRO)    MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7)
#define NCMDPROTO_REPEAT8(MACRO)    MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7) MACRO(8)
#define NCMDPROTO_REPEAT9(MACRO)    MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7) MACRO(8) MACRO(9)
#define NCMDPROTO_REPEAT10(MACRO)   MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7) MACRO(8) MACRO(9) MACRO(10)
#define NCMDPROTO_REPEAT11(MACRO)   MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7) MACRO(8) MACRO(9) MACRO(10) MACRO(11)
#define NCMDPROTO_REPEAT12(MACRO)   MACRO(1) MACRO(2) MACRO(3) MACRO(4) MACRO(5) MACRO(6) MACRO(7) MACRO(8) MACRO(9) MACRO(10) MACRO(11) MACRO(12)

#define NCMDPROTO_REPEAT_COMMA(NTIMES,MACRO)        NCMDPROTO_REPEAT_PRECOMMA_JOIN(0,NTIMES,MACRO)

#define NCMDPROTO_REPEAT_PRECOMMA(NTIMES,MACRO)     NCMDPROTO_REPEAT_PRECOMMA_JOIN(1,NTIMES,MACRO)

#define NCMDPROTO_REPEAT_PRECOMMA_JOIN(PRE,NTIMES,MACRO) NCMDPROTO_REPEAT_PRECOMMA_JOIN ## NTIMES(PRE,MACRO)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN0(PRE,MACRO)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN1(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN2(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN3(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN4(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN5(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN6(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN7(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6), MACRO(7)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN8(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6), MACRO(7), MACRO(8)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN9(PRE,MACRO)  NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6), MACRO(7), MACRO(8), MACRO(9)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN10(PRE,MACRO) NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6), MACRO(7), MACRO(8), MACRO(9), MACRO(10)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN11(PRE,MACRO) NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6), MACRO(7), MACRO(8), MACRO(9), MACRO(10), MACRO(11)
#define NCMDPROTO_REPEAT_PRECOMMA_JOIN12(PRE,MACRO) NCMDPROTO_COMMA(PRE) MACRO(1), MACRO(2), MACRO(3), MACRO(4), MACRO(5), MACRO(6), MACRO(7), MACRO(8), MACRO(9), MACRO(10), MACRO(11), MACRO(12)

#define NCMDPROTO_COMMA(PRE)    NCMDPROTO_COMMA ## PRE()
#define NCMDPROTO_COMMA0()
#define NCMDPROTO_COMMA1()      ,
#define NCMDPROTO_COMMA2()      ,
#define NCMDPROTO_COMMA3()      ,
#define NCMDPROTO_COMMA4()      ,
#define NCMDPROTO_COMMA5()      ,
#define NCMDPROTO_COMMA6()      ,
#define NCMDPROTO_COMMA7()      ,
#define NCMDPROTO_COMMA8()      ,
#define NCMDPROTO_COMMA9()      ,
#define NCMDPROTO_COMMA10()     ,
#define NCMDPROTO_COMMA11()     ,
#define NCMDPROTO_COMMA12()     ,

#define NCMDPROTO_PROTO_IN0()
#define NCMDPROTO_PROTO_IN1(t1)                  t1 inarg1
#define NCMDPROTO_PROTO_IN2(t1,t2)               t1 inarg1, t2 inarg2
#define NCMDPROTO_PROTO_IN3(t1,t2,t3)            t1 inarg1, t2 inarg2, t3 inarg3
#define NCMDPROTO_PROTO_IN4(t1,t2,t3,t4)         t1 inarg1, t2 inarg2, t3 inarg3, t4 inarg4
#define NCMDPROTO_PROTO_IN5(t1,t2,t3,t4,t5)      t1 inarg1, t2 inarg2, t3 inarg3, t4 inarg4, t5 inarg5
#define NCMDPROTO_PROTO_IN6(t1,t2,t3,t4,t5,t6)   t1 inarg1, t2 inarg2, t3 inarg3, t4 inarg4, t5 inarg5, t6 inarg6

#define NCMDPROTO_PROTO_OUT0()
#define NCMDPROTO_PROTO_OUT1(t1)                 t1 outarg1
#define NCMDPROTO_PROTO_OUT2(t1,t2)              t1 outarg1, t2 outarg2
#define NCMDPROTO_PROTO_OUT3(t1,t2,t3)           t1 outarg1, t2 outarg2, t3 outarg3
#define NCMDPROTO_PROTO_OUT4(t1,t2,t3,t4)        t1 outarg1, t2 outarg2, t3 outarg3, t4 outarg4
#define NCMDPROTO_PROTO_OUT5(t1,t2,t3,t4,t5)     t1 outarg1, t2 outarg2, t3 outarg3, t4 outarg4, t5 outarg5
#define NCMDPROTO_PROTO_OUT6(t1,t2,t3,t4,t5,t6)  t1 outarg1, t2 outarg2, t3 outarg3, t4 outarg4, t5 outarg5, t6 outarg6

#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN(NUMIN,NUMOUT,ARGS)    NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN ## NUMOUT(NUMIN,NUMOUT,ARGS)
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN0(NUMIN,NUMOUT,ARGS)
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN1(NUMIN,NUMOUT,ARGS)   NCMDPROTO_COMMA(NUMIN) NCMDPROTO_PROTO_OUT ## NUMOUT ## ARGS
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN2(NUMIN,NUMOUT,ARGS)   NCMDPROTO_COMMA(NUMIN) NCMDPROTO_PROTO_OUT ## NUMOUT ## ARGS
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN3(NUMIN,NUMOUT,ARGS)   NCMDPROTO_COMMA(NUMIN) NCMDPROTO_PROTO_OUT ## NUMOUT ## ARGS
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN4(NUMIN,NUMOUT,ARGS)   NCMDPROTO_COMMA(NUMIN) NCMDPROTO_PROTO_OUT ## NUMOUT ## ARGS
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN5(NUMIN,NUMOUT,ARGS)   NCMDPROTO_COMMA(NUMIN) NCMDPROTO_PROTO_OUT ## NUMOUT ## ARGS
#define NCMDPROTO_PROTO_OUT_PRECOMMA_JOIN6(NUMIN,NUMOUT,ARGS)   NCMDPROTO_COMMA(NUMIN) NCMDPROTO_PROTO_OUT ## NUMOUT ## ARGS

//------------------------------------------------------------------------------
/**
    NCMDPROTO_REPEATIO_INOUTARG is defined by the user
*/
#define NCMDPROTO_REPEATIO_INARG(NumIn)             \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,0)            \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,1)            \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,2)            \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,3)            \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,4)            \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,5)            \
    NCMDPROTO_REPEATIO_INOUTARG(NumIn,6)

#define NCMDPROTO_REPEATIO_ALL()                    \
    NCMDPROTO_REPEATIO_INARG(0)                     \
    NCMDPROTO_REPEATIO_INARG(1)                     \
    NCMDPROTO_REPEATIO_INARG(2)                     \
    NCMDPROTO_REPEATIO_INARG(3)                     \
    NCMDPROTO_REPEATIO_INARG(4)                     \
    NCMDPROTO_REPEATIO_INARG(5)                     \
    NCMDPROTO_REPEATIO_INARG(6)

#define NCMDPROTO_REPEATALL_ALL()   \
    NCMDPROTO_REPEATALL_ARG(0)      \
    NCMDPROTO_REPEATALL_ARG(1)      \
    NCMDPROTO_REPEATALL_ARG(2)      \
    NCMDPROTO_REPEATALL_ARG(3)      \
    NCMDPROTO_REPEATALL_ARG(4)      \
    NCMDPROTO_REPEATALL_ARG(5)      \
    NCMDPROTO_REPEATALL_ARG(6)      \
    NCMDPROTO_REPEATALL_ARG(7)      \
    NCMDPROTO_REPEATALL_ARG(8)      \
    NCMDPROTO_REPEATALL_ARG(9)      \
    NCMDPROTO_REPEATALL_ARG(10)     \
    NCMDPROTO_REPEATALL_ARG(11)     \
    NCMDPROTO_REPEATALL_ARG(12)

//------------------------------------------------------------------------------
#define NCMDPROTO_GETSIGNATURE(TYPE)    \
    nGetSignatureStr< typename n_remove_modifiers_and_reference<TYPE>::Type >()

#define NCMDPROTO_INARG_TYPE(NUMBER)     Traits::TPI ## NUMBER
#define NCMDPROTO_INARG(NUMBER)          inarg ## NUMBER
#define NCMDPROTO_INARG_PROTO(NUMBER)    NCMDPROTO_INARG(NUMBER)
#define NCMDPROTO_INARG_GETCMD(NUMBER)                                              \
    typename n_remove_modifiers_and_reference<NCMDPROTO_INARG_TYPE(NUMBER)>::Type   \
        NCMDPROTO_INARG(NUMBER);                                                    \
    typename n_space_for_pointers<                                                  \
        typename n_remove_modifiers_and_reference<NCMDPROTO_INARG_TYPE(NUMBER)>::Type \
        > argspace ## NUMBER(NCMDPROTO_INARG(NUMBER));                              \
    nGetCmdArg<                                                                     \
        typename n_remove_modifiers_and_reference<NCMDPROTO_INARG_TYPE(NUMBER)>::Type \
    >(cmd, (NCMDPROTO_INARG(NUMBER)));

#define NCMDPROTO_OUTARG_TYPE(NUMBER)    Traits::TPO ## NUMBER
#define NCMDPROTO_OUTARG(NUMBER)         outarg ## NUMBER
#define NCMDPROTO_OUTARG_PRE(NUMBER)                                                        \
    typename n_remove_modifiers_and_reference<NCMDPROTO_OUTARG_TYPE(NUMBER)>::Type          \
        NCMDPROTO_OUTARG(NUMBER);                                                           \
    memset(&NCMDPROTO_OUTARG(NUMBER), 0, sizeof(NCMDPROTO_OUTARG(NUMBER)));                 \
    typename n_space_for_pointers<                                                          \
        typename n_remove_const<NCMDPROTO_OUTARG_TYPE(NUMBER)>::Type                        \
        > outargspace ## NUMBER(NCMDPROTO_OUTARG(NUMBER));

#define NCMDPROTO_OUTARG_POST(NUMBER)                                                       \
    nSetCmdArg<                                                                             \
        typename n_remove_modifiers_and_reference<NCMDPROTO_OUTARG_TYPE(NUMBER)>::Type      \
    >(cmd, outarg ## NUMBER);

#define NCMDPROTO_RETCMDARG()                                                               \
    nSetCmdArg<                                                                             \
        typename n_remove_modifiers_and_reference<TR>::Type                                 \
    >(cmd, ret);

#define NCMDPROTO_ALLARG_TYPE(NUMBER)   typename Traits::TPA ## NUMBER
#define NCMDPROTO_ALLARG(NUMBER)        arg ## NUMBER
#define NCMDPROTO_ALLARG_TYPEARG(NUMBER) NCMDPROTO_ALLARG_TYPE(NUMBER) NCMDPROTO_ALLARG(NUMBER)


/**
    The damn doubles are passed as float by the compilers in variable argument lists.
    The following construction does the trick.
*/
#define NCMDPROTO_VALIST_ARG(NUMBER)                                                        \
    typedef typename n_remove_modifiers_and_reference<NCMDPROTO_ALLARG_TYPE(NUMBER)>::Type  \
        Type ## NUMBER;                                                                     \
    typedef Loki::Select<                                                                   \
        Loki::IsSameType<Type ## NUMBER, float>::value,                                     \
        double,                                                                             \
        Type ## NUMBER>::Result TypeVA ## NUMBER;                                           \
    Type ## NUMBER NCMDPROTO_ALLARG(NUMBER) = static_cast<Type ## NUMBER>                   \
        (va_arg(marker, TypeVA ## NUMBER));

//------------------------------------------------------------------------------

#define NCMDPROTO_REPEATIO_INOUTARG(NumIn,NumOut)                       \
    NCMDPROTO_DISPATCH_TEMPLATE_VOID(NumIn,NumOut)                      \
    NCMDPROTO_DISPATCH_TEMPLATE_RETURN(NumIn,NumOut)

#define NCMDPROTO_DISPATCH_TEMPLATE_VOID(NumIn,NumOut)                  \
    template <>                                                         \
    static bool DispatchCmd<NumIn,NumOut,true>(TClass * pthis, TMemberFunction memf, nCmd * cmd)\
    {                                                                   \
        NCMDPROTO_REPEAT(NumIn, NCMDPROTO_INARG_GETCMD);                \
        NCMDPROTO_REPEAT(NumOut, NCMDPROTO_OUTARG_PRE);                 \
        (pthis->*memf)(                                                 \
            NCMDPROTO_REPEAT_COMMA(NumIn, NCMDPROTO_INARG)              \
            NCMDPROTO_REPEAT_PRECOMMA_JOIN(NumIn,NumOut,NCMDPROTO_OUTARG)\
        );                                                              \
        NCMDPROTO_REPEAT(NumOut, NCMDPROTO_OUTARG_POST);                \
        return true;                                                    \
    }

#define NCMDPROTO_DISPATCH_TEMPLATE_RETURN(NumIn,NumOut)                \
    template <>                                                         \
    static bool DispatchCmd<NumIn,NumOut,false>(TClass * pthis, TMemberFunction memf, nCmd * cmd) \
    {                                                                   \
        NCMDPROTO_REPEAT(NumIn, NCMDPROTO_INARG_GETCMD);                \
        NCMDPROTO_REPEAT(NumOut, NCMDPROTO_OUTARG_PRE);                 \
        typename n_remove_const<TR>::Type ret = (n_remove_const<TR>::Type) \
        (pthis->*memf)(                                                 \
            NCMDPROTO_REPEAT_COMMA(NumIn, NCMDPROTO_INARG)              \
            NCMDPROTO_REPEAT_PRECOMMA_JOIN(NumIn,NumOut,NCMDPROTO_OUTARG)\
        );                                                              \
        NCMDPROTO_RETCMDARG();                                          \
        NCMDPROTO_REPEAT(NumOut, NCMDPROTO_OUTARG_POST);                \
        return true;                                                    \
    }

//------------------------------------------------------------------------------

#define NCMDPROTO_REPEATALL_ARG(NumArg)                                 \
    NCMDPROTO_DISPATCH_VALIST_VOID(NumArg)                              \
    NCMDPROTO_DISPATCH_VALIST_RET(NumArg)

#define NCMDPROTO_DISPATCH_VALIST_VOID(NumArg)                          \
    template <>                                                         \
    static void DispatchValistVoid<NumArg>(TClass * pthis, TMemberFunction memf, va_list marker)\
    {                                                                   \
        NCMDPROTO_REPEAT(NumArg, NCMDPROTO_VALIST_ARG);                 \
        (pthis->*memf)(                                                 \
            NCMDPROTO_REPEAT_COMMA(NumArg, NCMDPROTO_ALLARG)            \
        );                                                              \
    }

#define NCMDPROTO_DISPATCH_VALIST_RET(NumArg)                           \
    template <>                                                         \
    static TR DispatchValistRet<NumArg>(TClass * pthis, TMemberFunction memf, va_list marker)\
    {                                                                   \
        NCMDPROTO_REPEAT(NumArg, NCMDPROTO_VALIST_ARG);                 \
        typename n_remove_const<TR>::Type ret = (n_remove_const<TR>::Type) \
        (pthis->*memf)(                                                 \
            NCMDPROTO_REPEAT_COMMA(NumArg, NCMDPROTO_ALLARG)            \
        );                                                              \
        return ret;                                                     \
    }

//------------------------------------------------------------------------------
#pragma warning (disable : 4127 4100)

template <class TR, class TListIn, class TListOut>
class nCmdProtoTraits
{
public:

    typedef typename nCmdProtoTraits<TR,TListIn,TListOut> Traits;

    // single types in the input typelist
    typedef typename Loki::TL::TypeAtNonStrict<TListIn, 0>::Result TPI1;
    typedef typename Loki::TL::TypeAtNonStrict<TListIn, 1>::Result TPI2;
    typedef typename Loki::TL::TypeAtNonStrict<TListIn, 2>::Result TPI3;
    typedef typename Loki::TL::TypeAtNonStrict<TListIn, 3>::Result TPI4;
    typedef typename Loki::TL::TypeAtNonStrict<TListIn, 4>::Result TPI5;
    typedef typename Loki::TL::TypeAtNonStrict<TListIn, 5>::Result TPI6;

    // single types in the output typelist
    typedef typename Loki::TL::TypeAtNonStrict<TListOut, 0>::Result TPO1;
    typedef typename Loki::TL::TypeAtNonStrict<TListOut, 1>::Result TPO2;
    typedef typename Loki::TL::TypeAtNonStrict<TListOut, 2>::Result TPO3;
    typedef typename Loki::TL::TypeAtNonStrict<TListOut, 3>::Result TPO4;
    typedef typename Loki::TL::TypeAtNonStrict<TListOut, 4>::Result TPO5;
    typedef typename Loki::TL::TypeAtNonStrict<TListOut, 5>::Result TPO6;

    // combined typelist input and output types
    typedef typename Loki::TL::Append<TListIn, TListOut>::Result TListAll;

    // single types in the combined input and output type list
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 0>::Result TPA1;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 1>::Result TPA2;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 2>::Result TPA3;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 3>::Result TPA4;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 4>::Result TPA5;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 5>::Result TPA6;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 6>::Result TPA7;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 7>::Result TPA8;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 8>::Result TPA9;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 9>::Result TPA10;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 10>::Result TPA11;
    typedef typename Loki::TL::TypeAtNonStrict<TListAll, 11>::Result TPA12;

    // Number of arguments of each argument list
    enum {
        /// Number of input arguments
        NumInArgs = Loki::TL::Length<TListIn>::value,
        /// Number of output arguments
        NumOutArgs = Loki::TL::Length<TListOut>::value,
        /// Number of output arguments
        NumAllArgs = Loki::TL::Length<TListAll>::value,
        /// true when is void return, false otherwise
        ReturnsVoid = Loki::IsSameType<TR, void>::value
    };

    /// Calculate the fourcc
    static unsigned int CalcFourCC(const char * cmdname, unsigned int fourcc)
    {
        // if provided fourcc is zero then calculate a fourcc based on the CRC of command name
        if (!fourcc)
        {
            nCRC crc;
            fourcc = crc.Checksum((uchar *)cmdname, static_cast<unsigned int>(strlen(cmdname)));
        }

        return fourcc;
    }

    /// Calculate the prototype
    static char * CalcPrototype(char * signature, const char * cmdname)
    {
        n_assert(signature);
        n_assert(cmdname);

        char * ptr = signature;

        // check cmdname does not have underscores
        n_assert2((0 == strchr(cmdname, '_')), "Do not use underscores for command/signal names");

        // initialize to empty string
        ptr[0] = 0;

        // return type signature
        if (!ReturnsVoid || !NumOutArgs)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TR));
        }

        // outparameter signature
        if (NumOutArgs >= 1)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPO1));
        }
        if (NumOutArgs >= 2)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPO2));
        }
        if (NumOutArgs >= 3)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPO3));
        }
        if (NumOutArgs >= 4)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPO4));
        }
        if (NumOutArgs >= 5)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPO5));
        }
        if (NumOutArgs >= 6)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPO6));
        }

        // separator
        strcat(ptr, "_");

        // name
        ptr = ptr + strlen(ptr);
        const char * ptrname = cmdname;
        while (*ptrname)
        {
            *ptr++ = static_cast<char>(tolower(*ptrname++));
        }
        *ptr = 0;

        // separator
        strcat(ptr, "_");

        // inparameter signature
        if (NumInArgs == 0)
        {
            strcat(ptr, "v");
        }
        if (NumInArgs >= 1)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPI1));
        }
        if (NumInArgs >= 2)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPI2));
        }
        if (NumInArgs >= 3)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPI3));
        }
        if (NumInArgs >= 4)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPI4));
        }
        if (NumInArgs >= 5)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPI5));
        }
        if (NumInArgs >= 6)
        {
            strcat(ptr, NCMDPROTO_GETSIGNATURE(TPI6));
        }

        return signature;
    }

    // helper template struct to automatically obtain the proper member function pointers type
    template <class TClass, int NumArgs>
    struct TMemberFunctionHelper
    {
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 0>
    {
        typedef TR (TClass::*Type)();
        typedef TR (TClass::*TypeConst)() const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 1>
    {
        typedef TR (TClass::*Type)(TPA1);
        typedef TR (TClass::*TypeConst)(TPA1) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 2>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 3>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 4>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 5>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 6>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 7>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 8>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 9>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 10>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9, TPA10);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9, TPA10) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 11>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9, TPA10, TPA11);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9, TPA10, TPA11) const;
    };

    template <class TClass>
    struct TMemberFunctionHelper<TClass, 12>
    {
        typedef TR (TClass::*Type)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9, TPA10, TPA11, TPA12);
        typedef TR (TClass::*TypeConst)(TPA1, TPA2, TPA3, TPA4, TPA5, TPA6, TPA7, TPA8, TPA9, TPA10, TPA11, TPA12) const;
    };

    // Dispatcher for nCmd commands
    template <class TClass>
    class TCmdDispatcher
    {
    public:
        typedef typename TMemberFunctionHelper<TClass, NumAllArgs>::Type TMemberFunction;
        typedef typename TMemberFunctionHelper<TClass, NumAllArgs>::TypeConst TMemberFunctionConst;

        static bool Dispatch(TClass * obj, TMemberFunction memf, nCmd * cmd)
        {
            return DispatchCmd<NumInArgs, NumOutArgs, ReturnsVoid>(obj, memf, cmd);
        }

    private:
        template <int NumIn, int NumOut, bool RetVoid>
        static bool DispatchCmd(TClass * pthis, TMemberFunction memf, nCmd * cmd);

        NCMDPROTO_REPEATIO_ALL()
    };

    // Dispatcher for variable argument list va_list
    template <class TClass>
    class TValistDispatcher
    {
    public:
        typedef typename TMemberFunctionHelper<TClass, NumAllArgs>::Type TMemberFunction;
        typedef typename TMemberFunctionHelper<TClass, NumAllArgs>::TypeConst TMemberFunctionConst;

        template <bool RetVoid>
        static TR Dispatch(TClass * obj, TMemberFunction memf, va_list args);

        template <>
        static TR Dispatch<true>(TClass * obj, TMemberFunction memf, va_list args)
        {
            DispatchValistVoid<NumAllArgs>(obj, memf, args);
        }

        template <>
        static TR Dispatch<false>(TClass * obj, TMemberFunction memf, va_list args)
        {
            return DispatchValistRet<NumAllArgs>(obj, memf, args);
        }

    private:
        template <int NumArgs>
        static void DispatchValistVoid(TClass * pthis, TMemberFunction memf, va_list args);

        template <int NumArgs>
        static TR DispatchValistRet(TClass * pthis, TMemberFunction memf, va_list args);

        NCMDPROTO_REPEATALL_ALL()
    };

};

#pragma warning (default : 4127 4100)

//------------------------------------------------------------------------------
#undef NCMDPROTO_REPEATIO_INOUTARG
#undef NCMDPROTO_REPEATALL_ARG

//------------------------------------------------------------------------------

#endif

