#ifndef N_WATCHED_H
#define N_WATCHED_H
//------------------------------------------------------------------------------
/**
    @class nWatched
    @ingroup NebulaWatchedVariables
    @brief Creates a nEnv variable under /sys/var which can be "watched"
    through nConServer::Watch() method and through the nGuiWatcherWindow.
    This should only be used for debugging!

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "kernel/ndynautoref.h"

//------------------------------------------------------------------------------
class nWatched
{
public:
    /// constructor
    nWatched(const char* name, nArg::Type type);
    /// dereference operator
    nEnv* operator->();

private:
    nDynAutoRef<nEnv> refEnv; 
};

//------------------------------------------------------------------------------
/**
*/
inline
nWatched::nWatched(const char* name, nArg::Type type)
{
    n_assert(name);
    char buf[N_MAXPATH];
    sprintf(buf, "/sys/var/%s", name);
    this->refEnv = buf;
    if (!this->refEnv.isvalid())
    {
        this->refEnv = (nEnv*) nKernelServer::ks->New("nenv", buf);
        switch (type)
        {
            case nArg::Int:       
                this->refEnv->SetI(0);       
                break;

            case nArg::Float:     
                this->refEnv->SetF(0.0f);    
                break;

            case nArg::String:    
                this->refEnv->SetS("empty"); 
                break;

            case nArg::Bool:      
                this->refEnv->SetB(false);   
                break;

            case nArg::Object:    
                this->refEnv->SetO(0);       
                break;

            case nArg::Float4:
                {
                    nFloat4 f4 = { 0.0f, 0.0f, 0.0f, 0.0f };
                    this->refEnv->SetF4(f4);
                }
                break;

            default: 
                break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nEnv*
nWatched::operator->()
{
    return this->refEnv.get();
}

//------------------------------------------------------------------------------
#endif
