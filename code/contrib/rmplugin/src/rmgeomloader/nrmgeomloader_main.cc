//-----------------------------------------------------------------------------
//  nrmgeomloader_main.cc
//
//  N2 Loader's plug-in management kernel. 
//  This class can be used as an illustration for managing multiple  plug-ins 
//  in single DLL.
//
//  (C) 2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#include "rmgeomloader/nrmgeomloader_pch.h"
#include "rmgeomloader/nrmn3d2loader.h"
#include "rmgeomloader/nrmgeomloader_main.h"

//-----------------------------------------------------------------------------
// Accessor for the Kernel
//-----------------------------------------------------------------------------
nRMGeomLoaderKernel* GetLoaderKernel()
{
   static nRMGeomLoaderKernel myPlugInKernel;
   return &myPlugInKernel;
}

//-----------------------------------------------------------------------------
/**
*/
nRMGeomLoaderKernel::nRMGeomLoaderKernel() :
    hInstance(NULL),
    loaderPlugIn(NULL)
{
}

//-----------------------------------------------------------------------------
/**
*/
nRMGeomLoaderKernel::~nRMGeomLoaderKernel()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nRMGeomLoaderKernel::Init()
{
    this->loaderPlugIn = new nRMN3d2Loader();
}

//-----------------------------------------------------------------------------
/**
*/
void nRMGeomLoaderKernel::Uninitialize()
{
    hInstance = NULL;

    assert(this->loaderPlugIn==NULL); // PlugIn must have been freed already
}

//-----------------------------------------------------------------------------
/**
    PlugIn
*/
void nRMGeomLoaderKernel::GetPlugIn(int nIndex, IRmPlugIn **ppPlugIn)
{
    assert(nIndex==0);
    assert(this->loaderPlugIn!=NULL);

    *ppPlugIn = this->loaderPlugIn;
}

//-----------------------------------------------------------------------------
/**
*/
void nRMGeomLoaderKernel::FreePlugIn(IRmPlugIn *pPlugIn)
{
    assert(this->loaderPlugIn==pPlugIn);
    assert(this->loaderPlugIn!=NULL);

    this->loaderPlugIn->Uninitialize();
    delete this->loaderPlugIn;
    this->loaderPlugIn = NULL;
}

//-----------------------------------------------------------------------------
/**
    Entry point for New PlugIn Architecture
*/
bool RmInitPlugInDLL()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    GetLoaderKernel()->Init();

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
int  RmGetNumPlugIns()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int num = GetLoaderKernel()->GetNumPlugIns();
    return num;
}

//-----------------------------------------------------------------------------
/**
*/
void RmGetPlugIn(int nIndex, IRmPlugIn **ppPlugIn)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    GetLoaderKernel()->GetPlugIn(nIndex,ppPlugIn);
}

//-----------------------------------------------------------------------------
/**
*/
void RmFreePlugIn(IRmPlugIn *pPlugIn)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    GetLoaderKernel()->FreePlugIn(pPlugIn);
}

//-----------------------------------------------------------------------------
/**
*/
void RmUninitializePlugInDLL()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    GetLoaderKernel()->Uninitialize();
}
