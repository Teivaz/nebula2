#ifndef N_SYSTEM_H
#define N_SYSTEM_H
//------------------------------------------------------------------------------
/**
    Platform specific macros and defines.

    (C) 2002 RadonLabs GmbH
*/
#ifdef __XBxX__
    #ifndef N_STATIC
    #define N_STATIC
    #endif

    // temporarly disabled because of bugs in SSE math classes
    #define __USE_SSE__ (0)   
#endif

// use MicroTcl?
#define __MICROTCL__ (1)

#ifdef __WIN32__
#undef __WIN32__
#endif

#ifdef WIN32
#   define __WIN32__ (1)
#endif

#ifdef __WIN32__
#   ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   endif

    // this speeds up windows.h processing dramatically...
#   define NOGDICAPMASKS
#   define OEMRESOURCE
#   define NOATOM
#   define NOCLIPBOARD
#   define NOCTLMGR
#   define NOMEMMGR
#   define NOMETAFILE
#   define NOOPENFILE
#   define NOSERVICE
#   define NOSOUND
#   define NOCOMM
#   define NOKANJI
#   define NOHELP
#   define NOPROFILER
#   define NODEFERWINDOWPOS
#   define NOMCX
#endif

//--------------------------------------------------------------------
//  compiler identification:
//  __VC__      -> Visual C
//  __GNUC__    -> gcc
//--------------------------------------------------------------------
#ifdef _MSC_VER
#define __VC__ (1)
#endif

//--------------------------------------------------------------------
//  disable some VC warnings
//--------------------------------------------------------------------
#ifdef __VC__
#pragma warning( disable : 4251 )       // class XX needs DLL interface to be used...
#pragma warning( disable : 4355 )       // initialization list uses 'this' 
#pragma warning( disable : 4275 )       // base class has not dll interface...
#pragma warning( disable : 4786 )       // symbol truncated to 255 characters
#pragma warning( disable : 4530 )       // C++ exception handler used, but unwind semantics not enabled
#endif

//--------------------------------------------------------------------
#ifdef __VC__
#   ifndef N_STATIC
#       define N_EXPORT __declspec(dllexport)
#   else
#       define N_EXPORT
#   endif
#else
#   define __cdecl
#   define N_EXPORT
#endif

//--------------------------------------------------------------------
#endif
