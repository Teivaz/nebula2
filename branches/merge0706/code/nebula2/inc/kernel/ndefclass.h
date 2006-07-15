#ifndef N_DEFCLASS_H
#define N_DEFCLASS_H
//------------------------------------------------------------------------------
/**
    Macros which wraps the Nebula class module functions.
    Replaces the old *_init.cc Files.

    - 24-Mar-04     floh    removed the static nKernelServer* in nRoot derived
                            classes

    (C) 2001 RadonLabs GmbH
*/

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    Wraps the Nebula class package initialization function.

    Use like this:

    @code
    nNebulaUsePackage(blub);

    void bla()
    {
        nKernelServer kernelServer;
        kernelServer.AddPackage(blub);
    }
    @endcode
*/
#define nNebulaUsePackage(PACKAGE) extern "C" void PACKAGE()

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem
    
    nNebulaClass() creates a simple Nebula class without script interface.
    It takes the C name of the class, and a string defining the superclass
    name:

    @code
    nNebulaClass(nTestClass, "nparentclass");
    @endcode
*/
#define nNebulaClass(CLASS, SUPERCLASSNAME) \
    extern bool n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern void* n_create(); \
    bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        return true; \
    }; \
    void* n_create() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    nNebulaClassStaticInit(), declare Nebula class and call static
    data initialization function

    @code
    nNebulaClassStaticInit(nTestClass, "nparentclass", staticInitFunction);
    @endcode
*/
#define nNebulaClassStaticInit(CLASS, SUPERCLASSNAME, INITSTATICDATAFUNC) \
    extern bool n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern void* n_create(); \
    bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        INITSTATICDATAFUNC(); \
        return true; \
    }; \
    void* n_create() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    nNebulaScriptClass() creates a Nebula class with script interface (you'll
    have to provide a function void n_initcmds(nClass*)).
    It takes the C name of the class, and a string defining the superclass
    name:

    @code
    nNebulaScriptClass(nTestClass, "nparentclass");
    @endcode
*/

#define nNebulaScriptClass(CLASS, SUPERCLASSNAME) \
    extern bool n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern void* n_create(); \
    extern void n_initcmds(nClass *); \
    bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        n_initcmds(clazz); \
        return true; \
    }; \
    void* n_create() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    nNebulaScriptClassStaticInit(), declare Nebula class with script interface
    and call static data initialization function.

    @code
    nNebulaScriptClassStaticInit(nTestClass, "nparentclass", staticInitFunction);
    @endcode
*/
#define nNebulaScriptClassStaticInit(CLASS, SUPERCLASSNAME, INITSTATICDATAFUNC) \
    extern bool n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern void* n_create(); \
    extern void n_initcmds(nClass *); \
    bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        INITSTATICDATAFUNC(); \
        n_initcmds(clazz); \
        return true; \
    }; \
    void* n_create() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    @code
    nNebulaRootClass(): Special macro for the Nebula root class
    @endcode
*/

#define nNebulaRootClass(CLASS) \
    nKernelServer* CLASS::kernelServer = 0; \
    extern bool n_init(nClass* clazz, nKernelServer* kernelServer); \
    extern void* n_create(); \
    extern void n_initcmds(nClass *); \
    bool n_init(nClass* clazz, nKernelServer* kernelServer) {\
        CLASS::kernelServer = kernelServer; \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        n_initcmds(clazz); \
        return true; \
    }; \
    void* n_create() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
#endif
