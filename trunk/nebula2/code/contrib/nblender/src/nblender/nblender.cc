//-----------------------------------------------------------------------------
//  nblender.cc
//  (C) 2005-2006 James Mastro
//-----------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "python/npythonserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(npythonserver);
nNebulaUsePackage(ntoollib);

void
nPythonRegisterPackages(nKernelServer* kernelServer)
{
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(nnetwork);
    kernelServer->AddPackage(ndirect3d9);
    kernelServer->AddPackage(ndinput8);
    kernelServer->AddPackage(npythonserver);
    kernelServer->AddPackage(ntoollib);
}

extern "C" {
#ifdef __WIN32__
__declspec(dllexport)
#endif
void
initnblender()
{
    nPythonInitialize(nPythonModuleName());
}

}
