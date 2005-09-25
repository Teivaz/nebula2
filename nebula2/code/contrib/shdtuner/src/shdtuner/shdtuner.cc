#include "kernel/nkernelserver.h"
#include "python/npythonserver.h"
#include "kernel/nref.h"
#include "shdtuner/nshdtunerapp.h"

#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif

nNebulaUsePackage(nkernel);
nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(ngui);
nNebulaUsePackage(npythonserver);
nNebulaUsePackage(shdtuner);

const char* nPythonModuleName() {
    return "pynebula";
}

void nPythonInitializeEnvironment() {
    PyRun_SimpleString("from pynebula import *");
}

void nPythonRegisterPackages(nKernelServer* kernelSvr) {
    kernelSvr->AddPackage(nnebula);
    kernelSvr->AddPackage(ndinput8);
    kernelSvr->AddPackage(ndirect3d9);
    kernelSvr->AddPackage(ngui);
    kernelSvr->AddPackage(npythonserver);
    kernelSvr->AddPackage(shdtuner);
}

#ifdef __WIN32__
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    nKernelServer kernelSvr;
    nWin32LogHandler logHandler("shdtuner");
    kernelSvr.SetLogHandler(&logHandler);
    nWinMainCmdLineArgs args(lpCmdLine);
#else
int main(int argc, const char** argv) {
    nKernelServer kernelSvr;
    nCmdLineArgs args(argc, argv);
#endif

    nPythonRegisterPackages(&kernelSvr);
    nRef<nShdTunerApp> app = static_cast<nShdTunerApp*>(kernelSvr.New("nshdtunerapp", "/app"));
    n_assert(app.isvalid());
    if (app->Open()) {
        app->Run();
        app->Close();
    }
    app->Release();
    return 0;
}