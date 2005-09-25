//----------------------------------------------------------------------------
//  (c) 2005    John Smith
//----------------------------------------------------------------------------
#include "shdtuner/nshdtunerapp.h"

static void n_resetscene(void* slf, nCmd* cmd);
static void n_loadobject(void* slf, nCmd* cmd);
static void n_getpickedobject(void* slf, nCmd* cmd);
static void n_getobjectnode(void* slf, nCmd* cmd);
static void n_getobjectposition(void* slf, nCmd* cmd);
static void n_getobjectsize(void* slf, nCmd* cmd);

//----------------------------------------------------------------------------
/**
    @scriptclass
    nshdtunerapp

    @cppclass
    nShdTunerApp

    @superclass
    nroot

    @classinfo
    a brief description of the class
*/
void n_initcmds(nClass* cl) {
    cl->BeginCmds();
    cl->AddCmd("v_resetscene_v",            'RSSC', n_resetscene);
    cl->AddCmd("i_loadobject_s",            'LDOB', n_loadobject);
    cl->AddCmd("i_getpickedobject_v",       'GPOB', n_getpickedobject);
    cl->AddCmd("o_getobjectnode_i",         'GOND', n_getobjectnode);
    cl->AddCmd("fff_getobjectposition_i",   'GOPS', n_getobjectposition);
    cl->AddCmd("fff_getobjectsize_i",       'GOSZ', n_getobjectsize);
    cl->EndCmds();
}

//----------------------------------------------------------------------------
/**
*/
static void n_resetscene(void* slf, nCmd* cmd) {
    nShdTunerApp* self = static_cast<nShdTunerApp*>(slf);
    self->ResetScene();
}

//----------------------------------------------------------------------------
/**
*/
static void n_loadobject(void* slf, nCmd* cmd) {
    nShdTunerApp* self = static_cast<nShdTunerApp*>(slf);
    const char* fileName = cmd->In()->GetS();
    uint objectId = self->LoadObject(fileName);
    cmd->Out()->SetI(objectId);
}

//----------------------------------------------------------------------------
/**
*/
static void n_getpickedobject(void* slf, nCmd* cmd) {
    nShdTunerApp* self = static_cast<nShdTunerApp*>(slf);
    uint objectId = self->GetPickedObject();
    cmd->Out()->SetI(objectId);
}

//----------------------------------------------------------------------------
/**
*/
static void n_getobjectnode(void* slf, nCmd* cmd) {
    nShdTunerApp* self = static_cast<nShdTunerApp*>(slf);
    uint objectId = cmd->In()->GetI();
    nTransformNode* node = self->GetObjectNode(objectId);
    cmd->Out()->SetO(node);
}

//----------------------------------------------------------------------------
/**
*/
static void n_getobjectposition(void* slf, nCmd* cmd) {
    nShdTunerApp* self = static_cast<nShdTunerApp*>(slf);
    uint objectId = cmd->In()->GetI();
    vector3 pos = self->GetObjectPosition(objectId);
    cmd->Out()->SetF(pos.x);
    cmd->Out()->SetF(pos.y);
    cmd->Out()->SetF(pos.z);
}

//----------------------------------------------------------------------------
/**
*/
static void n_getobjectsize(void* slf, nCmd* cmd) {
    nShdTunerApp* self = static_cast<nShdTunerApp*>(slf);
    uint objectId = cmd->In()->GetI();
    vector3 size = self->GetObjectSize(objectId);
    cmd->Out()->SetF(size.x);
    cmd->Out()->SetF(size.y);
    cmd->Out()->SetF(size.z);
}

//----------------------------------------------------------------------------
/**
    @param  ps    Writes the nCmd object contents out to a file.
    @return       Success or failure.
*/
bool nShdTunerApp::SaveCmds(nPersistServer* ps) {
    if (nRoot::SaveCmds(ps)) {
        //nCmd* cmd = ps->GetCmd(this, 'XXXX');
        //ps->PutCmd(cmd);
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
