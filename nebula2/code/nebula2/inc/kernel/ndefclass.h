#ifndef N_DEFCLASS_H
#define N_DEFCLASS_H
//------------------------------------------------------------------------------
/**
    Macros which wraps the Nebula class module functions.
    Replaces the old *_init.cc Files.

    (C) 2001 RadonLabs GmbH
*/

//------------------------------------------------------------------------------
/*
    nNebulaClass() creates a simple Nebula class without script interface.
    It takes the C name of the class, and a string defining the superclass 
    name:

    nNebulaClass(nTestClass, "nparentclass");
*/
#define nNebulaClass(CLASS, SUPERCLASSNAME) \
    static nClass* cl = 0; \
    nKernelServer* CLASS::kernelServer = 0; \
    extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern "C" void N_EXPORT n_fini(); \
    extern "C" N_EXPORT void* n_create(); \
    extern "C" N_EXPORT char* n_version(); \
    N_EXPORT bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        cl = clazz; \
        CLASS::kernelServer = kernelServer; \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        return true; \
    }; \
    N_EXPORT void n_fini() { CLASS::kernelServer->RemClass(cl); }; \
    N_EXPORT void* n_create() { return n_new CLASS(); }; \
    N_EXPORT char* n_version() { return "x"; };

//------------------------------------------------------------------------------
/*
    nNebulaClassStaticInit(), declare Nebula class and call static
    data initialization function

    nNebulaClassStaticInit(nTestClass, "nparentclass", staticInitFunction);
*/
#define nNebulaClassStaticInit(CLASS, SUPERCLASSNAME, INITSTATICDATAFUNC) \
    static nClass* cl = 0; \
    nKernelServer* CLASS::kernelServer = 0; \
    extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern "C" void N_EXPORT n_fini(); \
    extern "C" N_EXPORT void* n_create(); \
    extern "C" N_EXPORT char* n_version(); \
    N_EXPORT bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        cl = clazz; \
        CLASS::kernelServer = kernelServer; \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        INITSTATICDATAFUNC(); \
        return true; \
    }; \
    N_EXPORT void n_fini() { CLASS::kernelServer->RemClass(cl); }; \
    N_EXPORT void* n_create() { return n_new CLASS(); }; \
    N_EXPORT char* n_version() { return "x"; };

//------------------------------------------------------------------------------
/*
    nNebulaScriptClass() creates a Nebula class with script interface (you'll
    have to provide a function void n_initcmds(nClass*)).
    It takes the C name of the class, and a string defining the superclass 
    name:

    nNebulaScriptClass(nTestClass, "nparentclass");
*/

#define nNebulaScriptClass(CLASS, SUPERCLASSNAME) \
    static nClass* cl = 0; \
    nKernelServer* CLASS::kernelServer = 0; \
    extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern "C" void N_EXPORT n_fini(); \
    extern "C" N_EXPORT void* n_create(); \
    extern "C" N_EXPORT char* n_version(); \
    extern void n_initcmds(nClass *); \
    N_EXPORT bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        cl = clazz; \
        CLASS::kernelServer = kernelServer; \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        n_initcmds(clazz); \
        return true; \
    }; \
    N_EXPORT void n_fini() { CLASS::kernelServer->RemClass(cl); }; \
    N_EXPORT void* n_create() { return n_new CLASS(); }; \
    N_EXPORT char* n_version() { return "x"; };

//------------------------------------------------------------------------------
/*
    nNebulaScriptClassStaticInit(), declare Nebula class with script interface
    and call static data initialization function.

    nNebulaScriptClassStaticInit(nTestClass, "nparentclass", staticInitFunction);
*/
#define nNebulaScriptClassStaticInit(CLASS, SUPERCLASSNAME, INITSTATICDATAFUNC) \
    static nClass* cl = 0; \
    nKernelServer* CLASS::kernelServer = 0; \
    extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern "C" void N_EXPORT n_fini(); \
    extern "C" N_EXPORT void* n_create(); \
    extern "C" N_EXPORT char* n_version(); \
    extern void n_initcmds(nClass *); \
    N_EXPORT bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        cl = clazz; \
        CLASS::kernelServer = kernelServer; \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        INITSTATICDATAFUNC(); \
        n_initcmds(clazz); \
        return true; \
    }; \
    N_EXPORT void n_fini() { CLASS::kernelServer->RemClass(cl); }; \
    N_EXPORT void* n_create() { return n_new CLASS(); }; \
    N_EXPORT char* n_version() { return "x"; };


//------------------------------------------------------------------------------
/*
    nNebulaRootClass(): Special macro for the Nebula root class
*/

#define nNebulaRootClass(CLASS) \
    static nClass* cl = 0; \
    nKernelServer* CLASS::kernelServer = 0; \
    extern "C" bool N_EXPORT n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern "C" void N_EXPORT n_fini(); \
    extern "C" N_EXPORT void* n_create(); \
    extern "C" N_EXPORT char* n_version(); \
    extern void n_initcmds(nClass *); \
    N_EXPORT bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        cl = clazz; \
        CLASS::kernelServer = kernelServer; \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        n_initcmds(clazz); \
        return true; \
    }; \
    N_EXPORT void n_fini() {  }; \
    N_EXPORT void* n_create() { return n_new CLASS(); }; \
    N_EXPORT char* n_version() { return "x"; };

//------------------------------------------------------------------------------
#endif
