#ifndef N_WATCHVAR_H
#define N_WATCHVAR_H

//-------------------------------------------------------------------
/**
    @class nWatchVar

    @brief A small statistics object.
    
    This is a helper class to allow variables to be viewed by the console 
    server's watch command.
*/
//-------------------------------------------------------------------

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

class nWatchVar {
public: 
    nWatchVar(nKernelServer *kernelServer, const char *name)
    {
        char buf[N_MAXPATH];
        sprintf(buf,"/sys/var/%s",name);
        nEnv *e = (nEnv *) kernelServer->Lookup(buf);
        if (!e)
        {
            e = (nEnv*) kernelServer->New("nenv", buf);
        }
        n_assert(e);
        this->ref_env = e;
        this->ref_env->AddRef();
    };
    ~nWatchVar() {
        if (ref_env.isvalid()) ref_env->Release();
    };
    
    void operator=(const char *obj)
    {
        this->ref_env->SetS(obj);
    };
    void operator=(int obj)
    {
        this->ref_env->SetI(obj);
    };
    void operator=(float obj)
    {
        this->ref_env->SetF(obj);
    };
    void operator=(bool obj)
    {
        this->ref_env->SetB(obj);
    };
    
    void operator+=(int obj)
    {
        this->ref_env->SetI(obj + this->ref_env->GetI());
    };
    void operator+=(float obj)
    {
        this->ref_env->SetF(obj + this->ref_env->GetF());
    };
    void operator-=(int obj)
    {
        this->ref_env->SetI(obj - this->ref_env->GetI());
    };
    void operator-=(float obj)
    {
        this->ref_env->SetF(obj - this->ref_env->GetF());
    };

private:
    nRef<nEnv> ref_env;
};

#endif