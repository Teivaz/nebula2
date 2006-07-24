//------------------------------------------------------------------------------
//  (C) 2003    Megan Fox
//------------------------------------------------------------------------------
#include "physdemo/simpleobject.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    simpleobject

    @cppclass
    SimpleObject

    @superclass
    nroot

    @classinfo
    It's a simple entity, meant to hold all the data relevant to a particular rigid body existing
    in the physdemo world.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps  writes the nCmd object contents out to a file.
    @return     success or failure
*/
bool
SimpleObject::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

