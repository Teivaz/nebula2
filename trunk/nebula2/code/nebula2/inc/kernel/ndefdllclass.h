#ifndef N_DEFDLLCLASS_H
#define N_DEFDLLCLASS_H
//------------------------------------------------------------------------------
/**
    DLL export/import stuff (mostly __WIN32__ stuff.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_SYSTEM_H
#include "kernel/nsystem.h"
#endif

#undef N_DLLCLASS
#define N_DLLCLASS

#ifdef __VC__
#   if defined(N_IMPLEMENTS) && defined(N_DEFINES) && (N_IMPLEMENTS==N_DEFINES)
#       if defined(N_STATIC)
#           define N_PUBLIC
#       else
#           define N_PUBLIC __declspec(dllexport)
#       endif
#   else
#       define N_PUBLIC
#   endif
#else
#   define N_PUBLIC
#endif

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

//-------------------------------------------------------------------
#endif  