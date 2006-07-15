//-----------------------------------------------------------------------------
//  nrmgomloader_main.h
//
//
//  Declaration file for RenderMonkey's plug-in DLL entry points and the geometry
//  plug-in itself.
//
//  (C) 2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------

#ifndef N_RMGEOMLOADER_MAIN_H
#define N_RMGEOMLOADER_MAIN_H

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------
class nRMGeomLoader;
class nRMN3d2Loader;

//-----------------------------------------------------------------------------
/**
    @class nRMGeomLoaderKernel
    @ingroup

    @brief
    N2 Loader's plug-in management kernel. This class can be used as an 
    illustration for managing multiple plug-ins in single DLL.                                                                                        
*/
class nRMGeomLoaderKernel
{
public:
    /// Constructor.
    nRMGeomLoaderKernel();
    /// Destructor.
    virtual ~nRMGeomLoaderKernel();

    /// Initializes the kernel instance:
    void Init();

    /// Perform any uninitialization code for the kernel (and thus the plug-ins that it
    /// manages). Note that this method gets called after the plug-ins were supposed
    /// to be freed by the application. 
    void Uninitialize();

    /// Retrieve the DLL instance for this DLL:
    HINSTANCE GetPlugInInstance() const { return hInstance; };

    /// Retrieve the number of plug-ins that this kernel manages 
    /// (1 in our case, since it's a single plug-in):
    int GetNumPlugIns()           const  { return 1; };

    /// Retrieve a plug-in specified by the index and store it in the interface pointer provided.
    /// This method is used by the main application to access individual plug-ins from each DLL:
    void GetPlugIn(int nIndex, IRmPlugIn **ppPlugIn);

    /// This method gets called to perform any plug-in specific uninitialization code and then
    // delete the actual plug-in from memory:
    void FreePlugIn(IRmPlugIn *pPlugIn);

private:
    HINSTANCE         hInstance;

    /// In this particular example there is only a single plug-in to manage. 
    /// However, it is easy to extend the approach to manage multiple plug-ins
    /// by the same kernel. 
    nRMN3d2Loader *loaderPlugIn;       

};

//-----------------------------------------------------------------------------
// Singleton accessor for the plug-in kernel.
//-----------------------------------------------------------------------------
nRMGeomLoaderKernel* GetLoaderKernel();

#endif
