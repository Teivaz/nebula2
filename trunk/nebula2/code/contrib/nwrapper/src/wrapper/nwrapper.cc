//------------------------------------------------------------------------------
/**
    @page NebulaToolsnwrapper nwrapper

    nsh

    <dl>
     <dt>-help</dt>
       <dd>show this help</dd>
    </dl>

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Bruce Mitchener, Jr.
*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "tools/ncmdlineargs.h"
#include "wrapper/nlanguagewrapper.h"
#include "util/narray.h"

static void dumpClassTree(nKernelServer    * kernelServer,
                          nLanguageWrapper * backend,
                          nClass           * cl);
static void dumpClass(nKernelServer    * kernelServer,
                      nLanguageWrapper * backend,
                      nClass           * cl);

int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);

    // get cmd line args
    bool helpArg                 = args.GetBoolArg("-help");
    const char * backendArg      = args.GetStringArg("-backend",
                                                     "nlanguagewrapper");

    if (helpArg)
    {
        n_printf("(C) 2003 Bruce Mitchener, Jr.\n"
                 "nwrapper - Nebula2 wrapper generator\n"
                 "Command line args:\n"
                 "------------------\n"
                 "-help                   show this help\n");
        return 5;
    }

    // create minimal Nebula runtime
    nKernelServer* kernelServer = new nKernelServer;
    nLanguageWrapper* backend;
    backend = (nLanguageWrapper*) kernelServer->New(backendArg,
                                                    "/sys/servers/wrapper");
    if (0 == backend)
    {
        n_printf("Could not create script server of class '%s'\n", backendArg);
        delete kernelServer;
        return 10;
    }

    nClass * rootClass = kernelServer->FindClass("nroot");

    dumpClassTree(kernelServer, backend, rootClass);

    backend->Release();
    delete kernelServer;
    return 0;
}

static
void
findSubClasses(nKernelServer * kernelServer,
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

static
void
dumpClassTree(nKernelServer    * kernelServer,
              nLanguageWrapper * backend,
              nClass           * cl)
{
    dumpClass(kernelServer, backend, cl);
    nArray<nClass *> subClasses;
    findSubClasses(kernelServer, cl, &subClasses);
    int classIndex, numClasses = subClasses.Size();
    for (classIndex = 0; classIndex < numClasses; classIndex++)
    {
        nClass * scl = subClasses[classIndex];
        dumpClassTree(kernelServer, backend, scl);
    }
}

static
void
dumpClass(nKernelServer    * kernelServer,
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

