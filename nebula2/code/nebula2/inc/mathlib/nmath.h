#ifndef N_MATH_H
#define N_MATH_H
//-------------------------------------------------------------------
//
//  © a. flemming, member of <46>, november '96
//  this is a part of nebula (TM)
//
//  CLASS
//		---
//
//	OVERVIEW
//	
//  Mathematische Deklarationen für Nebula (TM)
//
//  Es hat sich als günstig erwiesen, eineige zusätzliche mathemische
//  Deklarationen zu machen bzw. bestehende zu erweitern und zu ersetzen.
//
//  26-Oct-00   floh    no longer depends on Nebula
//  08-Dec-00   floh    + n_sin/n_cos
//-------------------------------------------------------------------
#include <math.h>

#ifdef _MSC_VER
#define isnan _isnan
#define isinf _isinf
#endif

//-------------------------------------------------------------------
#ifndef PI
#define PI (3.1415926535897932384626433832795028841971693993751f)
#endif

#define N_PI PI

#ifndef TINY
#define TINY (0.0000001)
#endif

#define n_max(a,b)      (((a) > (b)) ? (a) : (b))
#define n_min(a,b)      (((a) < (b)) ? (a) : (b))
#define n_abs(a)        (((a)<0.0f) ? (-(a)) : (a))
#define n_sgn(a)        (((a)<0.0f) ? (-1) : (1))

//-------------------------------------------------------------------
// 15-Oct-00    floh    oops, hier war aber ein boeser Bug drin,
//                      'd' bzw. 'r' waren nicht in Klammern, so dass
//                      Bullshit rauskam, wenn 'd' bzw. 'r' eine
//                      Expression waren.
//-------------------------------------------------------------------
#define n_deg2rad(d)    (((d)*PI)/180.0f)
#define n_rad2deg(r)    (((r)*180.0f)/PI)

//-------------------------------------------------------------------
//  14-Dec-99   floh    new float wrapper routines
//-------------------------------------------------------------------
#define n_sin(x) (float(sin(x)))
#define n_cos(x) (float(cos(x)))

static inline float n_acos(float x)
{
    if(x >  1.0f) x =  1.0f;
    if(x < -1.0f) x = -1.0f;
    return (float)acos(x);
}

static inline float n_asin(float x)
{
    if(x >  1.0f) x =  1.0f;
    if(x < -1.0f) x = -1.0f;
    return (float)asin(x);
}

static inline float n_sqrt(float x)
{
    if (x<0.0f) x=(float)0.0f;
    return (float) sqrt(x);
}

//-------------------------------------------------------------------
//  n_fequal()
//  Return true if (f0==f1)
//-------------------------------------------------------------------
static inline bool n_fequal(float f0, float f1, float tol) {
    float f = f0-f1;
    if ((f>(-tol)) && (f<tol)) return true;
    else                       return false;
}

//-------------------------------------------------------------------
//  n_fless()
//  Return true if (f0<f1).
//-------------------------------------------------------------------
static inline bool n_fless(float f0, float f1, float tol) {
    if ((f0-f1)<tol) return true;
    else             return false;
}

//-------------------------------------------------------------------
//  n_fgreater()
//  Return true if (f0>f1)
//-------------------------------------------------------------------
static inline bool n_fgreater(float f0, float f1, float tol) {
    if ((f0-f1)>tol) return true;
    else             return false;
}

//-------------------------------------------------------------------
//  fast float to int conversion (always truncates)
//  see http://www.stereopsis.com/FPU.html for a discussion.
//  NOTE: this works only on x86 endian machines.
//-------------------------------------------------------------------
static inline long n_ftol(float val)
{
    double v = double(val) + (68719476736.0*1.5);
    return ((long*)&v)[0] >> 16;
}

//-------------------------------------------------------------------
//  smooth a new value towards an old value using a change value.
//-------------------------------------------------------------------
static inline float n_smooth(float newVal, float curVal, float maxChange)
{
    float diff = newVal - curVal;
    if (fabs(diff) > maxChange)
    {
        if (diff > 0.0f)
        {
            curVal += maxChange;
            if (curVal > newVal)
            {
                curVal = newVal;
            }
        }
        else if (diff < 0.0f)
        {
            curVal -= maxChange;
            if (curVal < newVal)
            {
                curVal = newVal;
            }
        }
    }
    else
    {
        curVal = newVal;
    }
    return curVal;
}

//-------------------------------------------------------------------
//  clamp a value against lower und upper boundary.
//-------------------------------------------------------------------
static inline float n_clamp(float val, float lower, float upper)
{
    if (val < lower)
    {
        val = lower;
    }
    else if (val > upper)
    {
        val = upper;
    }
    return val;
}

//-------------------------------------------------------------------
#endif










