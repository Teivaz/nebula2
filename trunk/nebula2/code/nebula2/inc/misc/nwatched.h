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
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nWatched
{
public:
    /// default constructor
    nWatched();
    /// constructor
    nWatched(const nString& name, nArg::Type type);
    /// initialize if default constructor used
    void Initialize(const nString& name, nArg::Type type);
    /// dereference operator
    nEnv* operator->();

private:
    nDynAutoRef<nEnv> refEnv;
};
//------------------------------------------------------------------------------
#if __NEBULA_STATS__
#define WATCHER_DECLARE(watcher) nWatched watcher;
#define WATCHER_INIT(watcher,name,type) watcher.Initialize(name, type);
#define WATCHER_RESET_INT(watcher) watcher->SetI(0);
#define WATCHER_RESET_FLOAT(watcher) watcher->SetF(0.0f);
#define WATCHER_RESET_STRING(watcher) watcher->SetS("");
#define WATCHER_RESET_BOOL(watcher) watcher->SetB(false);
#define WATCHER_RESET_VECTOR3(watcher) watcher->SetV3(vector3(0.0f, 0.0f, 0.0f));
#define WATCHER_RESET_VECTOR4(watcher) watcher->SetV4(vector3(0.0f, 0.0f, 0.0f, 0.0f));
#define WATCHER_SET_INT(watcher,val) watcher->SetI(val);
#define WATCHER_SET_FLOAT(watcher,val) watcher->SetF(val);
#define WATCHER_SET_STRING(watcher,val) watcher->SetS(val);
#define WATCHER_SET_BOOL(watcher,val) watcher->SetB(val);
#define WATCHER_SET_VECTOR3(watcher,val) watcher->SetV3(val);
#define WATCHER_SET_VECTOR4(watcher,val) watcher->SetV4(val);
#define WATCHER_ADD_INT(watcher,val) watcher->SetI(watcher->GetI() + val);
#else
#define WATCHER_DECLARE(watcher)
#define WATCHER_INIT(watcher,name,type)
#define WATCHER_RESET_INT(watcher)
#define WATCHER_RESET_FLOAT(watcher)
#define WATCHER_RESET_STRING(watcher)
#define WATCHER_RESET_BOOL(watcher)
#define WATCHER_RESET_VECTOR3(watcher)
#define WATCHER_RESET_VECTOR4(watcher)
#define WATCHER_SET_INT(watcher,val)
#define WATCHER_SET_FLOAT(watcher,val)
#define WATCHER_SET_STRING(watcher,val)
#define WATCHER_SET_BOOL(watcher,val)
#define WATCHER_SET_VECTOR3(watcher,val)
#define WATCHER_SET_VECTOR4(watcher,val)
#define WATCHER_ADD_INT(watcher,val)
#endif

//------------------------------------------------------------------------------
/**
*/
inline
nWatched::nWatched()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nWatched::Initialize(const nString& name, nArg::Type type)
{
    n_assert(name.IsValid());
    nString path;
    path.Format("/sys/var/%s", name.Get());
    this->refEnv = path.Get();
    if (!this->refEnv.isvalid())
    {
        this->refEnv = (nEnv*)nKernelServer::Instance()->New("nenv", path.Get());
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
nWatched::nWatched(const nString& name, nArg::Type type)
{
    this->Initialize(name, type);
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
