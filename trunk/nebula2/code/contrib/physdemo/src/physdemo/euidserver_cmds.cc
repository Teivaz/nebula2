//------------------------------------------------------------------------------
//  (C) 2003	Megan Fox
//------------------------------------------------------------------------------
#include "physdemo/euidserver.h"
#include "kernel/npersistserver.h"

static void n_getnewid(void* slf, nCmd* cmd);
static void n_releaseid(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    euidserver
    
    @superclass
    nroot

    @classinfo
    The eUIDserver just hands out unique IDs from an internal pool.  Any
    deleted IDs are returned to the pool, and all new uIDs recieved from a
    single server are guaranteed to be unique. Of course, uIDs from two
    separately-created servers could be identical, but that should be obvious.

    Be careful if you request more than 2^31 unique IDs - the server is
    using signed integers internally, and the negative integers you're
    get after the wrap-around may or may not work for your purposes.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
	clazz->AddCmd("i_getnewid_v", 'IDGN', n_getnewid);
    clazz->AddCmd("v_releaseid_i", 'IDRL', n_releaseid);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnewid

    @input
    v

    @output
    i

    @info
    Get a new unique ID
*/
static
void
n_getnewid(void* slf, nCmd* cmd)
{
    eUIDServer* self = (eUIDServer*) slf;

    cmd->Out()->SetI(self->GetNewID());
}

//------------------------------------------------------------------------------
/**
    @cmd
    releaseid

    @input
    i

    @output
    v

    @info
    release an unused unique id
*/
static
void
n_releaseid(void* slf, nCmd* cmd)
{
    eUIDServer* self = (eUIDServer*) slf;

    self->ReleaseID(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @param  ps  writes the nCmd object contents out to a file.
    @return     success or failure
*/
bool
eUIDServer::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

