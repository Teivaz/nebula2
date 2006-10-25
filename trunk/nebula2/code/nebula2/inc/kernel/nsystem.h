#ifndef N_SYSTEM_H
#define N_SYSTEM_H
//------------------------------------------------------------------------------
/**
    Platform specific macros and defines.

    (C) 2002 RadonLabs GmbH
*/
#ifdef __XBxX__
    // temporarly disabled because of bugs in SSE math classes
    #define __USE_SSE__ (0)
#endif

#define __NEBULA_STATS__ (1)
//#define __NEBULA_NO_THREADS__ (1)
//#define __NEBULA_NO_ASSERT__ (1)

// use MicroTcl?
#define __MICROTCL__ (1)

#define N_ADV_SIGNALS_ENABLED (1)
#if defined(_MSC_VER) && (_MSC_VER < 1301)
#undef N_ADV_SIGNALS_ENABLED
#endif

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

//------------------------------------------------------------------------------
//  compiler identification:
//  __VC__      -> Visual C
//  __GNUC__    -> gcc
//------------------------------------------------------------------------------
#ifdef _MSC_VER
#define __VC__ (1)
#endif

//------------------------------------------------------------------------------
//  disable some VC warnings
//------------------------------------------------------------------------------
#ifdef __VC__
#pragma warning(disable : 4251)       // class XX needs DLL interface to be used...
#pragma warning(disable : 4355)       // initialization list uses 'this'
#pragma warning(disable : 4275)       // base class has not dll interface...
#pragma warning(disable : 4786)       // symbol truncated to 255 characters
#pragma warning(disable : 4530)       // C++ exception handler used, but unwind semantics not enabled
#pragma warning(disable : 4995)       // _OLD_IOSTREAMS_ARE_DEPRECATED
#pragma warning(disable : 4996)       // _CRT_INSECURE_DEPRECATE, VS8: old string routines are deprecated
#endif

//------------------------------------------------------------------------------
// Only do __attributes__ for GCC.
//------------------------------------------------------------------------------
#ifndef __GNUC__
#  define  __attribute__(x)  /**/
#endif

//------------------------------------------------------------------------------
// These are temporary until we finish getting rid of them
//------------------------------------------------------------------------------
#ifndef __cdecl
#define __cdecl
#endif
#define N_EXPORT

//------------------------------------------------------------------------------
// magically make the Nebula class module callbacks work (provide
// the N_INIT, N_FINI, N_NEW and N_VERSION per-source file
// via compiler command line switches, this is taken care of when
// updsrc.tcl mechanism is used to create build files).
//------------------------------------------------------------------------------
#ifdef N_INIT
#   define n_init N_INIT
#endif
#ifdef N_FINI
#   define n_fini N_FINI
#endif
#ifdef N_NEW
#   define n_create N_NEW
#endif
#ifdef N_VERSION
#   define n_version N_VERSION
#endif
#ifdef N_INITCMDS
#   define n_initcmds N_INITCMDS
#endif


//------------------------------------------------------------------------------
#endif
