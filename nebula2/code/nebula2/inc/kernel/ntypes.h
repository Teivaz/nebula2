#ifndef N_TYPES_H
#define N_TYPES_H
//------------------------------------------------------------------------------
/**
    Lowlevel Nebula defs.

    (C) 2002 RadonLabs GmbH
*/
#ifndef __XBxX__
#include <errno.h>
#include <stdio.h>
#endif

#include "kernel/nsystem.h"
#include "kernel/ndebug.h"
#include "kernel/ndefclass.h"

// Shortcut Typedefs
typedef unsigned long  ulong;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef float float2[2];
typedef float float3[3];
typedef float float4[4];
struct nFloat4
{
    float x;
    float y;
    float z;
    float w;
};
struct nFloat3
{
    float x;
    float y;
    float z;
};
struct nFloat2
{
    float x;
    float y;
};
typedef unsigned int nFourCC;
typedef double nTime;

#ifndef NULL
#define NULL (0L)
#endif

//------------------------------------------------------------------------------
#define N_MAXPATH (512)     // maximum length for complete path
#define N_MAXNAMELEN (32)   // maximum lentgh for single path component

//------------------------------------------------------------------------------
#define nID(a,b,c,d) ((a<<24)|(b<<16)|(c<<8)|(d))
#define MAKE_FOURCC(ch0,ch1,ch2,ch3) (ch0 | ch1<<8 | ch2<<16 | ch3<<24)
#define FOURCC(i) (((i&0xff000000)>>24) | ((i&0x00ff0000)>>8) | ((i&0x0000ff00)<<8) | ((i&0x000000ff)<<24))
#define N_WHITESPACE " \r\n\t"

#ifdef __LINUX__
#define n_stricmp strcasecmp
#else
#define n_stricmp stricmp
#endif

#ifdef __WIN32__
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

// maps unsigned 8 bits/channel to D3DCOLOR
#define N_ARGB(a,r,g,b) ((uint)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define N_RGBA(r,g,b,a) N_ARGB(a,r,g,b)
#define N_XRGB(r,g,b)   N_ARGB(0xff,r,g,b)
#define N_COLORVALUE(r,g,b,a) N_RGBA((uint)((r)*255.f),(uint)((g)*255.f),(uint)((b)*255.f),(uint)((a)*255.f))

//------------------------------------------------------------------------------
//  public kernel C functions
//------------------------------------------------------------------------------
void __cdecl n_printf(const char *, ...);
void __cdecl n_error(const char*, ...);
void __cdecl n_message(const char*, ...);
void n_sleep(double);
char *n_strdup(const char *);
char *n_strncpy2(char *, const char *, size_t);
void *nn_malloc(size_t, const char *, int);
void *nn_calloc(size_t, size_t, const char *, int);
void *nn_realloc(void *, size_t, const char *, int);
void n_free(void *);
bool n_strmatch(const char *, const char *);
void n_strcat(char *, const char *, size_t);

void n_barf(const char *, const char *, int);
void n_barf2(const char*, const char*, const char*, int);

void *n_dllopen(const char *);
void  n_dllclose(void *);
void *n_dllsymbol(void *, const char *);

//------------------------------------------------------------------------------
//  Nebula mem manager wrappers.
//------------------------------------------------------------------------------
#ifdef __STANDALONE__
#define n_malloc(s) malloc(s)
#define n_calloc(s,n) calloc(s,n)
#define n_realloc(o,s) realloc(o,s)
#define n_free(p) free(p)
#else
#define n_malloc(s) nn_malloc(s,__FILE__,__LINE__)
#define n_calloc(s,n) nn_calloc(s,n,__FILE__,__LINE__)
#define n_realloc(o,s) nn_realloc(o,s,__FILE__,__LINE__)
#endif

#ifdef new
#undef new
#endif

#ifdef delete
#undef delete
#endif

#ifdef __NEBULA_MEM_MANAGER__
static inline void * operator new(size_t size)
{
    void *p = nn_malloc(size, __FILE__, __LINE__);
    return p;
}
static inline void *operator new[](size_t size)
{
    void *p = nn_malloc(size, __FILE__, __LINE__);
    return p;
}

static inline void * operator new(size_t size, const char* file, int line)
{
    void *p = nn_malloc(size, file, line);
    return p;
}
static inline void *operator new[](size_t size, const char* file, int line)
{
    void *p = nn_malloc(size, file, line);
    return p;
}

static inline void operator delete(void *p)
{
    n_free(p);
}

static inline void operator delete(void *p, const char* file, int line)
{
    n_free(p);
}

static inline void operator delete[](void *p)
{
    n_free(p);
}
static inline void operator delete[](void *p, const char* file, int line)
{
    n_free(p);
}

#define n_new new(__FILE__, __LINE__)
#define n_delete delete

#else
#define n_new new
#define n_delete delete
#endif
//--------------------------------------------------------------------
#endif
