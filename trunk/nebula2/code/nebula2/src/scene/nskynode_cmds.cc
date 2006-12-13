//------------------------------------------------------------------------------
//  nskynode_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nskynode.h"
#include "kernel/npersistserver.h"

static void n_settimefactor(void* slf, nCmd* cmd);
static void n_gettimefactor(void* slf, nCmd* cmd);
static void n_setstarttime(void* slf, nCmd* cmd);
static void n_getstarttime(void* slf, nCmd* cmd);
static void n_addstate(void* slf, nCmd* cmd);
static void n_removestate(void* slf, nCmd* cmd);
static void n_newelement(void* slf, nCmd* cmd);
static void n_addelement(void* slf, nCmd* cmd);
static void n_deleteelement(void* slf, nCmd* cmd);
static void n_linkto(void* slf, nCmd* cmd);
static void n_setrefreshtime(void* slf, nCmd* cmd);
static void n_settimeperiod(void* slf, nCmd* cmd);
static void n_gettimeperiod(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nskynode

    @cppclass
    nSkyNode

    @superclass
    ntransformnode

    @classinfo
    This renders a skybox with cellestrial elements
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_settimefactor_f",          'STFA', n_settimefactor);
    cl->AddCmd("f_gettimefactor_v",          'GTFA', n_gettimefactor);
    cl->AddCmd("v_setstarttime_f",           'SSTT', n_setstarttime);
    cl->AddCmd("f_getstarttime_v",           'GSTT', n_getstarttime);
    cl->AddCmd("v_addstate_ssf",             'ADDS', n_addstate);
    cl->AddCmd("v_removestate_ss",           'RMVS', n_removestate);
    cl->AddCmd("v_newelement_ss",            'NEWE', n_newelement);
    cl->AddCmd("v_addelement_ss",            'ADDE', n_addelement);
    cl->AddCmd("v_deleteelement_s",          'DELE', n_deleteelement);
    cl->AddCmd("v_linkto_ss",                'LKSL', n_linkto);
    cl->AddCmd("v_setrefreshtime_sf",        'SRFT', n_setrefreshtime);
    cl->AddCmd("v_settimeperiod_f",          'STPD', n_settimeperiod);
    cl->AddCmd("f_gettimeperiod_v",          'GTPD', n_gettimeperiod);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    linkto
    @input
    ss(two element names)
    @output
    v
    @info
    Links one element to another
*/
static void
n_linkto(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str1 = cmd->In()->GetS();
    const char* str2 = cmd->In()->GetS();
    self->LinkTo(str1,str2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addstate
    @input
    ssf(element, state, time)
    @output
    v
    @info
    Attaches a state to an element
*/
static void
n_addstate(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str1 = cmd->In()->GetS();
    const char* str2 = cmd->In()->GetS();
    float t = cmd->In()->GetF();
    self->AddState(str1,str2,t);
}

//------------------------------------------------------------------------------
/**
    @cmd
    removestate
    @input
    ss(element, state)
    @output
    v
    @info
    Detaches a state from an element
*/
static void
n_removestate(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str1 = cmd->In()->GetS();
    const char* str2 = cmd->In()->GetS();
    self->RemoveState(str1,str2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    newelement
    @input
    ss(elementtype, elementname)
    @output
    v
    @info
    Creates new sky element
*/
static void
n_newelement(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str1 = cmd->In()->GetS();
    const char* str2 = cmd->In()->GetS();
    self->NewElement(self->StringToType(str1),str2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addelement
    @input
    ss(elementtype, elementname)
    @output
    v
    @info
    Adds object as a sky element
*/
static void
n_addelement(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str1 = cmd->In()->GetS();
    const char* str2 = cmd->In()->GetS();
    self->AddElement(self->StringToType(str1),str2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    deleteelement
    @input
    s(elementname)
    @output
    v
    @info
    Deletes an element
*/
static void
n_deleteelement(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str1 = cmd->In()->GetS();
    self->DeleteElement(str1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    settimefactor
    @input
    f(timefactor)
    @output
    v
    @info
    set time factor
*/
static void
n_settimefactor(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    float t = cmd->In()->GetF();
    self->SetTimeFactor(t);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettimefactor
    @input
    v
    @output
    f(timefactor)
    @info
    get time factor
*/
static void
n_gettimefactor(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    cmd->Out()->SetF(self->GetTimeFactor());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstarttime
    @input
    f(starttime)
    @output
    v
    @info
    set start time
*/
static void
n_setstarttime(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    float t = cmd->In()->GetF();
    self->SetStartTime(t);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstarttime
    @input
    v
    @output
    f(starttime)
    @info
    get start time
*/
static void
n_getstarttime(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    cmd->Out()->SetF(self->GetStartTime());
}

//------------------------------------------------------------------------------
/**
    @cmd
    settimeperiode
    @input
    f(periode)
    @output
    v
    @info
    set time periode
*/
static void
n_settimeperiod(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    float t = cmd->In()->GetF();
    self->SetTimePeriod(t);
}

//------------------------------------------------------------------------------
/**
    @cmd
    gettimeperiode
    @input
    v
    @output
    f(periode)
    @info
    get time periode
*/
static void
n_gettimeperiod(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    cmd->Out()->SetF(self->GetTimePeriod());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrefreshtime
    @input
    sf(element, refreshtime)
    @output
    v
    @info
    sets refresh time of an element
*/
static void
n_setrefreshtime(void* slf, nCmd* cmd)
{
    nSkyNode* self = (nSkyNode*) slf;
    const char* str = cmd->In()->GetS();
    float t = cmd->In()->GetF();
    self->SetRefreshTime(str,t);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSkyNode::SaveCmds(nPersistServer* ps)
{
    if (nTransformNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- TimeFactor ---
        cmd = ps->GetCmd(this, 'STFA');
        cmd->In()->SetF(this->GetTimeFactor());
        ps->PutCmd(cmd);

        //--- TimePeriode ---
        cmd = ps->GetCmd(this, 'STPD');
        cmd->In()->SetF(this->GetTimePeriod());
        ps->PutCmd(cmd);

        //--- StartTime ---
        cmd = ps->GetCmd(this, 'SSTT');
        cmd->In()->SetF(this->GetSkyTime());
        ps->PutCmd(cmd);

        //--- elements ---
        int i;
        for (i=0; i < this->elements.Size(); i++)
        {
            cmd = ps->GetCmd(this, 'ADDE');
            cmd->In()->SetS(this->TypeToString(this->elements[i].type));
            cmd->In()->SetS(this->elements[i].refElement->GetName());
            ps->PutCmd(cmd);
        }

        //--- linkto ---
        int k;
        for (i=0; i < this->elements.Size(); i++)
        {
            for (k=0; k < this->elements[i].linkTo.Size(); k++)
            {
                cmd = ps->GetCmd(this, 'LKSL');
                cmd->In()->SetS(this->elements[i].refElement->GetName());
                cmd->In()->SetS(this->elements[this->elements[i].linkTo[k]].refElement->GetName());
                ps->PutCmd(cmd);
            }
        }

        //--- addState ---
        for (i=0; i < this->elements.Size(); i++)
        {
            for (k=0; k < this->elements[i].states.Size(); k++)
            {
                cmd = ps->GetCmd(this, 'ADDS');
                cmd->In()->SetS(this->elements[i].refElement->GetName());
                cmd->In()->SetS(this->elements[i].states[k].refState->GetName());
                cmd->In()->SetF(this->elements[i].states[k].time);
                ps->PutCmd(cmd);
            }
        }

        ////--- refreshTime ---
        //for (i=0; i < this->elements.Size(); i++)
        //{
        //    cmd = ps->GetCmd(this, 'SRFT');
        //    cmd->In()->SetS(this->elements[i].refElement->GetName());
        //    cmd->In()->SetF(this->elements[i].refreshTime);
        //    ps->PutCmd(cmd);
        //}

        return true;
    }
    return false;
}
