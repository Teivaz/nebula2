//------------------------------------------------------------------------------
//  (c) 2005 Tragnarion Studios
//  Scripting commands for signals
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nobject.h"
#include "kernel/nref.h"

static void n_bindsignal(void* o, nCmd* cmd);
static void n_unbindsignal(void* o, nCmd* cmd);
static void n_unbindsignalobject(void* o, nCmd* cmd);
static void n_unbindobject(void* o, nCmd* cmd);
static void n_unbindall(void* o, nCmd* cmd);
static void n_getsignals(void* o, nCmd* cmd);
static void n_addsignal(void* o, nCmd* cmd);

//------------------------------------------------------------------------------
void n_initcmds_nsignalemitter(nClass *cl)
{
    cl->AddCmd("b_bindsignal_sosi",         'BISI', n_bindsignal);
    cl->AddCmd("b_unbindsignal_sos",        'UNSI', n_unbindsignal);
    cl->AddCmd("b_unbindsignalobject_so",   'UNSO', n_unbindsignalobject);
    cl->AddCmd("b_unbindobject_o",          'UNOB', n_unbindobject);
    cl->AddCmd("v_unbindall_v",             'UNAL', n_unbindall);
    cl->AddCmd("l_getsignals_v",            'GSIG', n_getsignals);
    cl->AddCmd("b_addsignal_ss",            'ASIG', n_addsignal);
}

//------------------------------------------------------------------------------
static void n_bindsignal(void* o, nCmd* cmd)
{
    nObject* self = (nObject*) o;

    const char *signalName = cmd->In()->GetS();
    nObject * object = static_cast<nObject *> (cmd->In()->GetO());
    const char *cmdName = cmd->In()->GetS();
    int priority = cmd->In()->GetI();
    cmd->Out()->SetB(self->BindSignal(signalName, object, cmdName, priority));
}

//------------------------------------------------------------------------------
static void n_unbindsignal(void* o, nCmd* cmd)
{
    nObject* self = (nObject*) o;

    const char *signalName = cmd->In()->GetS();
    nObject * object = static_cast<nObject *> (cmd->In()->GetO());
    const char *cmdName = cmd->In()->GetS();
    cmd->Out()->SetB(self->UnbindSignal(signalName, object, cmdName));
}

//------------------------------------------------------------------------------
static void n_unbindsignalobject(void* o, nCmd* cmd)
{
    nObject* self = (nObject*) o;

    const char *signalName = cmd->In()->GetS();
    nObject * object = static_cast<nObject *> (cmd->In()->GetO());
    cmd->Out()->SetB(self->UnbindTargetObject(signalName, object));
}

//------------------------------------------------------------------------------
static void n_unbindobject(void* o, nCmd* cmd)
{
    nObject* self = (nObject*) o;

    nObject * object = static_cast<nObject *> (cmd->In()->GetO());
    cmd->Out()->SetB(self->UnbindTargetObject(object));
}

//------------------------------------------------------------------------------
static void n_unbindall(void* o, nCmd* /*cmd*/)
{
    nObject* self = (nObject*) o;

    self->UnbindAllSignals();
}

//-------------------------------------------------------------------
static void n_getsignals(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    nHashList signalList;
    nHashNode* node;
    int numSignals = 0;

    self->GetSignals(&signalList);

    // count signals
    for (node = signalList.GetHead(); node; node = node->GetSucc())
    {
        numSignals++;
    }

    nArg* args = n_new_array(nArg, numSignals);
    int i = 0;
    while (0 != (node = signalList.RemHead()))
    {
        args[i++].SetS(((nSignal*) node->GetPtr())->GetProtoDef());
        n_delete(node);
    }
    cmd->Out()->SetL(args, numSignals);
}

//-------------------------------------------------------------------
static void n_addsignal(void *o, nCmd *cmd)
{
    nObject* self = (nObject*) o;

    const char * proto_def = cmd->In()->GetS();
    const char * strfourcc = cmd->In()->GetS();
    unsigned int fourcc;
    if (strlen(strfourcc) == 4)
    {
        fourcc = MAKE_FOURCC(strfourcc[0],strfourcc[1],strfourcc[2],strfourcc[3]);
    }
    else
    {
        fourcc = atoi(strfourcc);
    }

    cmd->Out()->SetB(self->GetClass()->AddSignal(proto_def, fourcc));
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
