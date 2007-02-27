

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Feb 27 10:17:11 2007
 */
/* Compiler settings for .\nmaxenvelopecurve.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __nmaxenvelopecurveidl_h__
#define __nmaxenvelopecurveidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___Dnmaxenvelopecurve_FWD_DEFINED__
#define ___Dnmaxenvelopecurve_FWD_DEFINED__
typedef interface _Dnmaxenvelopecurve _Dnmaxenvelopecurve;
#endif     /* ___Dnmaxenvelopecurve_FWD_DEFINED__ */


#ifndef ___DnmaxenvelopecurveEvents_FWD_DEFINED__
#define ___DnmaxenvelopecurveEvents_FWD_DEFINED__
typedef interface _DnmaxenvelopecurveEvents _DnmaxenvelopecurveEvents;
#endif     /* ___DnmaxenvelopecurveEvents_FWD_DEFINED__ */


#ifndef __nmaxenvelopecurve_FWD_DEFINED__
#define __nmaxenvelopecurve_FWD_DEFINED__

#ifdef __cplusplus
typedef class nmaxenvelopecurve nmaxenvelopecurve;
#else
typedef struct nmaxenvelopecurve nmaxenvelopecurve;
#endif /* __cplusplus */

#endif     /* __nmaxenvelopecurve_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __nmaxenvelopecurveLib_LIBRARY_DEFINED__
#define __nmaxenvelopecurveLib_LIBRARY_DEFINED__

/* library nmaxenvelopecurveLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_nmaxenvelopecurveLib;

#ifndef ___Dnmaxenvelopecurve_DISPINTERFACE_DEFINED__
#define ___Dnmaxenvelopecurve_DISPINTERFACE_DEFINED__

/* dispinterface _Dnmaxenvelopecurve */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__Dnmaxenvelopecurve;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("949C1889-5D48-44FF-B422-CBE798F64039")
    _Dnmaxenvelopecurve : public IDispatch
    {
    };
    
#else     /* C style interface */

    typedef struct _DnmaxenvelopecurveVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _Dnmaxenvelopecurve * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _Dnmaxenvelopecurve * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _Dnmaxenvelopecurve * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _Dnmaxenvelopecurve * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _Dnmaxenvelopecurve * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _Dnmaxenvelopecurve * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _Dnmaxenvelopecurve * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DnmaxenvelopecurveVtbl;

    interface _Dnmaxenvelopecurve
    {
        CONST_VTBL struct _DnmaxenvelopecurveVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _Dnmaxenvelopecurve_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _Dnmaxenvelopecurve_AddRef(This)    \
    (This)->lpVtbl -> AddRef(This)

#define _Dnmaxenvelopecurve_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define _Dnmaxenvelopecurve_GetTypeInfoCount(This,pctinfo)    \
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _Dnmaxenvelopecurve_GetTypeInfo(This,iTInfo,lcid,ppTInfo)    \
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _Dnmaxenvelopecurve_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)    \
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _Dnmaxenvelopecurve_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)    \
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif     /* C style interface */


#endif     /* ___Dnmaxenvelopecurve_DISPINTERFACE_DEFINED__ */


#ifndef ___DnmaxenvelopecurveEvents_DISPINTERFACE_DEFINED__
#define ___DnmaxenvelopecurveEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DnmaxenvelopecurveEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DnmaxenvelopecurveEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("BC8E451C-50C9-4F41-9F4D-7113C3212D05")
    _DnmaxenvelopecurveEvents : public IDispatch
    {
    };
    
#else     /* C style interface */

    typedef struct _DnmaxenvelopecurveEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DnmaxenvelopecurveEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DnmaxenvelopecurveEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DnmaxenvelopecurveEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DnmaxenvelopecurveEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DnmaxenvelopecurveEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DnmaxenvelopecurveEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DnmaxenvelopecurveEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DnmaxenvelopecurveEventsVtbl;

    interface _DnmaxenvelopecurveEvents
    {
        CONST_VTBL struct _DnmaxenvelopecurveEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DnmaxenvelopecurveEvents_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DnmaxenvelopecurveEvents_AddRef(This)    \
    (This)->lpVtbl -> AddRef(This)

#define _DnmaxenvelopecurveEvents_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define _DnmaxenvelopecurveEvents_GetTypeInfoCount(This,pctinfo)    \
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DnmaxenvelopecurveEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)    \
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DnmaxenvelopecurveEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)    \
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DnmaxenvelopecurveEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)    \
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif     /* C style interface */


#endif     /* ___DnmaxenvelopecurveEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_nmaxenvelopecurve;

#ifdef __cplusplus

class DECLSPEC_UUID("EDA6DBCD-A8BB-4709-AB92-40181C0C58CE")
nmaxenvelopecurve;
#endif
#endif /* __nmaxenvelopecurveLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


