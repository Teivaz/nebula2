
#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        
#endif

#ifndef WINVER              
#define WINVER 0x0400       
#endif

#ifndef _WIN32_WINNT     
#define _WIN32_WINNT 0x0400
#endif                        

#ifndef _WIN32_WINDOWS     
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE           
#define _WIN32_IE 0x0400    
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    

#include <afxctl.h>         
#include <afxext.h>         
#include <afxdtctl.h>       
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxdb.h>          
#include <afxdao.h>         
#include <afxwin.h>

