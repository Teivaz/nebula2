#ifndef N_VECTOR_H
#define N_VECTOR_H
//------------------------------------------------------------------------------
/**
    Implement 2, 3 and 4-dimensional vector classes.

    (C) 2002 RadonLabs GmbH
*/

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifdef __USE_SSE__

// sse vector classes
#include "mathlib/_vector2.h"
#include "mathlib/_vector3_sse.h"
#include "mathlib/_vector4_sse.h"

typedef _vector2 vector2; 
typedef _vector3_sse vector3;
typedef _vector4_sse vector4;

#else

// generic vector classes
#include "mathlib/_vector2.h"
#include "mathlib/_vector3.h"
#include "mathlib/_vector4.h"

typedef _vector2 vector2;
typedef _vector3 vector3;
typedef _vector4 vector4;

#endif

//------------------------------------------------------------------------------
#endif
