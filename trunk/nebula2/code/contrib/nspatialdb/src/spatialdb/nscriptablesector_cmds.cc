//-------------------------------------------------------------------
//  nscriptablesector_cmds.cc
//  (C) 2004 Gary Haussmann
//-------------------------------------------------------------------
#include "spatialdb/nscriptablesector.h"
#include "kernel/npersistserver.h"

static void n_addvisibleobject(void *slf, nCmd *cmd);
static void n_addoccludingobject(void *slf, nCmd *cmd);
static void n_addportalobject(void *slf, nCmd *cmd);
static void n_remobject(void *slf, nCmd *cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nscriptablesector

    @superclass
    nroot

    @classinfo
*/
void n_initcmds(nClass *cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_addvisibleobject_ssffff",       'AVOB',  n_addvisibleobject);
    cl->AddCmd("v_addoccludingobject_sffffff",      'AOOB', n_addoccludingobject);
    cl->AddCmd("v_addportalobject_ssffff",          'APOB', n_addportalobject);
    cl->AddCmd("v_remobject_s",                     'RMOB', n_remobject);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    addvisibleobject
    @input
    s(object name), s(scenenode path), f(xcoord), f(ycoord), f(zcoord), f(radius)
    @output
    v
    @info
    Add a visible object into the sector, using the specified path as the root scene node.
*/
static void
n_addvisibleobject(void* slf, nCmd* cmd)
{
    nScriptableSector *self = (nScriptableSector *)slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    float r = cmd->In()->GetF();
    self->AddVisibleObject(s0, s1, vector3(x,y,z), r);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addoccludingobject
    @input
    s(object name), f(min xcoord), f(min ycoord), f(min zcoord), f(max xcoord), f(max ycoord), f(max zcoord)
    @output
    v
    @info
    Add an occluding object to the sector, using the specific bounding box as the occluder.  Note that this
    doesn't actually render anything except in debug visualization mode; if you want to actually see the
    occluder you'll have to add a visible object with the same size, shape and position.
*/
static void
n_addoccludingobject(void* slf, nCmd* cmd)
{
    nScriptableSector *self = (nScriptableSector *)slf;
    const char* s0 = cmd->In()->GetS();
    float x1 = cmd->In()->GetF();
    float y1 = cmd->In()->GetF();
    float z1 = cmd->In()->GetF();
    float x2 = cmd->In()->GetF();
    float y2 = cmd->In()->GetF();
    float z2 = cmd->In()->GetF();
    self->AddOccludingObject( s0, vector3(x1,y1,z1), vector3(x2,y2,z2) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    addportalobject
    @input
    s(object name), s(NOH path to sector on other side of portal), f(xcoord), f(ycoord), f(zcoord), f(radius)
    @output
    v
    @info
    Add an occluding object to the sector, using the specific bounding box as the occluder.  Note that this
    doesn't actually render anything except in debug visualization mode; if you want to actually see the
    occluder you'll have to add a visible object with the same size, shape and position.
*/
static void
n_addportalobject(void* slf, nCmd* cmd)
{
    nScriptableSector *self = (nScriptableSector *)slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    float r = cmd->In()->GetF();
    self->AddPortalObject( s0, s1, vector3(x,y,z), r );
}

//------------------------------------------------------------------------------
/**
    @cmd
    remobject
    @input
    s(object name)
    @output
    v
    @info
    Remove the named object from the sector
*/
static void
n_remobject(void* slf, nCmd* cmd)
{
    nScriptableSector *self = (nScriptableSector *)slf;
    const char* s0 = cmd->In()->GetS();
    self->RemObject( s0 );
}

bool
nScriptableSector::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        // walk our objects and add them to the persistserver
        // find the element in here somewhere...
        nCmd* cmd;

        for (int i=0; i < scriptelm_array.Size(); i++)
        {
            nSpatialElement *walknodes = scriptelm_array[i];
            nScriptableSectorObject *walkobject = (nScriptableSectorObject *)walknodes->GetPtr();

            // dump this object out--what type is it?
            nSpatialElement *se = walkobject->spatialinfo;
            if (se->GetSpatialType() & nSpatialElement::N_SPATIAL_OCCLUDER)
            {
                // write out an occluder
                cmd = ps->GetCmd(this, 'AOOB');
                cmd->In()->SetS(walkobject->objectname.Get());
                cmd->In()->SetF(se->minCorner.x);
                cmd->In()->SetF(se->minCorner.y);
                cmd->In()->SetF(se->minCorner.z);
                cmd->In()->SetF(se->maxCorner.x);
                cmd->In()->SetF(se->maxCorner.y);
                cmd->In()->SetF(se->maxCorner.z);
                ps->PutCmd(cmd);
            }
            else if (se->GetSpatialType() & nSpatialElement::N_SPATIAL_PORTAL)
            {
                // write out a portal object
                cmd = ps->GetCmd(this, 'APOB');
                cmd->In()->SetS(walkobject->objectname.Get());
                cmd->In()->SetS("argh");
                cmd->In()->SetF(se->pos.x);
                cmd->In()->SetF(se->pos.y);
                cmd->In()->SetF(se->pos.z);
                cmd->In()->SetF(se->radius);
                ps->PutCmd(cmd);
            }
            else
            {
                // write out a visible object
                cmd = ps->GetCmd(this, 'AVOB');
                cmd->In()->SetS(walkobject->objectname.Get());
                cmd->In()->SetS(walkobject->rendernode.getname());
                cmd->In()->SetF(se->pos.x);
                cmd->In()->SetF(se->pos.y);
                cmd->In()->SetF(se->pos.z);
                cmd->In()->SetF(se->radius);
                ps->PutCmd(cmd);
            }
        }

        return true;
    }

    return false;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
