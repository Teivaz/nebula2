#ifndef N_DEBUG_H
#define N_DEBUG_H
//------------------------------------------------------------------------------
/**
    Nebula debug macros.
  
    n_assert()  - the vanilla assert() Macro
    n_verify()  - like assert() except that the statement in parens is simply evaluated, unchecked, if __NEBULA_NO_ASSERT is set
    n_assert2() - an assert() plus a message from the programmer
*/
#include <stdlib.h>

#ifdef __NEBULA_NO_ASSERT__
#define n_assert(exp)
#define n_verify(exp) (exp)
#define n_verify2(exp,imsg) (exp)
#define n_assert2(exp, msg)
#define n_dxtrace(hr, msg)
#else
#define n_assert(exp) { if (!(exp)) n_barf(#exp,__FILE__,__LINE__); }
#define n_verify(exp) { if (!(exp)) n_barf(#exp,__FILE__,__LINE__); }

// an assert with a verbose hint from the programmer
#define n_assert2(exp, msg) { if (!(exp)) n_barf2(#exp,msg,__FILE__,__LINE__); }
#define n_verify2(exp, msg) { if (!(exp)) n_barf2(#exp,msg,__FILE__,__LINE__); }

// dx9 specific: check HRESULT and display DX9 specific message box
#define n_dxtrace(hr, msg) { if (FAILED(hr)) DXTrace(__FILE__,__LINE__,hr,msg,true); }

#endif
//------------------------------------------------------------------------------
#endif
