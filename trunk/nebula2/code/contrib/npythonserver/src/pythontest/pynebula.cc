//------------------------------------------------------------------------------
/**
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(npythonserver);

void
nPythonRegisterPackages(nKernelServer * kernelServer)
{
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(nnetwork);
    kernelServer->AddPackage(ndirect3d9);
    kernelServer->AddPackage(ndinput8);
    kernelServer->AddPackage(npythonserver);
}

