//------------------------------------------------------------------------------
/**
    @class nWrapperApp

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Bruce Mitchener, Jr.
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "wrapper/nlanguagewrapper.h"
#include "wrapper/nwrapperapp.h"

nWrapperApp::nWrapperApp()
{
    // Empty
}

nWrapperApp::~nWrapperApp()
{
    // Empty
}

void
nWrapperApp::Run()
{
    nKernelServer* kernelServer = nKernelServer::Instance();
    nLanguageWrapper* backend;
    backend = (nLanguageWrapper*) kernelServer->New(this->GetBackendClass(),
                                                    "/sys/servers/wrapper");
    if (0 == backend)
    {
        n_error("Could not create backend of class '%s'",
                this->GetBackendClass());
    }

    nClass * rootClass = kernelServer->FindClass("nroot");

    this->DumpClassTree(kernelServer, backend, rootClass);

    backend->Release();
}

void
nWrapperApp::FindSubClasses(nKernelServer * kernelServer,
                            nClass        * cl,
                            nArray<nClass *> * subClasses)
{
    const nHashList * classes = kernelServer->GetClassList();
    nClass * classObject ;
    for (classObject=(nClass *)classes->GetHead();
         classObject;
         classObject=(nClass *)classObject->GetSucc())
    {
        if (cl == classObject->GetSuperClass())
        {
            subClasses->PushBack(classObject);
        }
    }
}

void
nWrapperApp::DumpClassTree(nKernelServer    * kernelServer,
                           nLanguageWrapper * backend,
                           nClass           * cl)
{
    this->DumpClass(kernelServer, backend, cl);
    nArray<nClass *> subClasses;
    this->FindSubClasses(kernelServer, cl, &subClasses);
    int classIndex, numClasses = subClasses.Size();
    for (classIndex = 0; classIndex < numClasses; classIndex++)
    {
        nClass * scl = subClasses[classIndex];
        this->DumpClassTree(kernelServer, backend, scl);
    }
}

void
nWrapperApp::DumpClass(nKernelServer    * kernelServer,
                       nLanguageWrapper * backend,
                       nClass           * cl)
{
    backend->OpenClass(cl);

    nHashList * cl_cmdprotos = cl->GetCmdList();
    if (NULL != cl_cmdprotos)
    {
        nCmdProto *cmd_proto;
        for (cmd_proto=(nCmdProto *) cl_cmdprotos->GetHead();
             cmd_proto;
             cmd_proto=(nCmdProto *) cmd_proto->GetSucc())
        {
            backend->WriteCommand(cl, cmd_proto);
        }
    }

    backend->CloseClass(cl);
}

