//-----------------------------------------------------------------------------
//  nmaxenvelopecurve.cpp
//
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "nmaxenvelopecurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


nmaxenvelopecurveApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
        { 0xB9F5585A, 0xF75A, 0x4348, { 0xA8, 0x1A, 0x53, 0x2, 0x9C, 0x1A, 0x52, 0xB6 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CnmaxenvelopecurveApp::InitInstance 

//-----------------------------------------------------------------------------
/**
*/
BOOL nmaxenvelopecurveApp::InitInstance()
{
    BOOL bInit = COleControlModule::InitInstance();

    if (bInit)
    {
        // TODO: Add your own module initialization code.
    }

    return bInit;
}

//-----------------------------------------------------------------------------
/**
*/
int nmaxenvelopecurveApp::ExitInstance()
{
    return COleControlModule::ExitInstance();
}

//-----------------------------------------------------------------------------
/**
*/
STDAPI DllRegisterServer(void)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
        return ResultFromScode(SELFREG_E_CLASS);

    return NOERROR;
}

//-----------------------------------------------------------------------------
/**
*/
STDAPI DllUnregisterServer(void)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
        return ResultFromScode(SELFREG_E_CLASS);

    return NOERROR;
}
