#ifndef N_DEBUG_H
#define N_DEBUG_H
//------------------------------------------------------------------------------
/**
    Nebula debug macros.
  
    n_assert()  - the vanilla assert() Macro
*/
#include <stdlib.h>

#ifdef __NEBULA_NO_ASSERT__
#define n_assert(exp)
#else
#define n_assert(exp) { if (!(exp)) n_barf(#exp,__FILE__,__LINE__); }
#endif
//------------------------------------------------------------------------------
#endif
