//------------------------------------------------------------------------------
//  nprefserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "misc/nprefserver.h"

static void n_setcompanyname(void* slf, nCmd* cmd);
static void n_getcompanyname(void* slf, nCmd* cmd);
static void n_setapplicationname(void* slf, nCmd* cmd);
static void n_getapplicationname(void* slf, nCmd* cmd);
static void n_open(void* slf, nCmd* cmd);
static void n_close(void* slf, nCmd* cmd);
static void n_isopen(void* slf, nCmd* cmd);
static void n_keyexists(void* slf, nCmd* cmd);
static void n_writestring(void* slf, nCmd* cmd);
static void n_writeint(void* slf, nCmd* cmd);
static void n_writefloat(void* slf, nCmd* cmd);
static void n_writebool(void* slf, nCmd* cmd);
static void n_writevector4(void* slf, nCmd* cmd);
static void n_writevector3(void* slf, nCmd* cmd);
static void n_readstring(void* slf, nCmd* cmd);
static void n_readint(void* slf, nCmd* cmd);
static void n_readfloat(void* slf, nCmd* cmd);
static void n_readbool(void* slf, nCmd* cmd);
static void n_readvector4(void* slf, nCmd* cmd);
static void n_readvector3(void* slf, nCmd* cmd);

//-------------------------------------------------------------------
/**
    @scriptclass
    nprefserver
    @cppclass
    nPrefServer
    @superclass
    nroot
    @classinfo
    Manages persistent information between application invocations.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setcompanyname_s",     'SCPN', n_setcompanyname);
    cl->AddCmd("s_getcompanyname_v",     'GCPN', n_getcompanyname);
    cl->AddCmd("v_setapplicationname_s", 'SAPN', n_setapplicationname);
    cl->AddCmd("s_getapplicationname_v", 'GAPN', n_getapplicationname);
    cl->AddCmd("b_open_v",               'OPEN', n_open);
    cl->AddCmd("v_close_v",              'CLOS', n_close);
    cl->AddCmd("b_isopen_v",             'ISOP', n_isopen);
    cl->AddCmd("b_keyexists_v",          'KEXT', n_keyexists);
    cl->AddCmd("b_writestring_ss",       'WSTR', n_writestring);
    cl->AddCmd("b_writeint_si",          'WINT', n_writeint);
    cl->AddCmd("b_writefloat_sf",        'WFLT', n_writefloat);
    cl->AddCmd("b_writebool_sb",         'WBOL', n_writebool);
    cl->AddCmd("b_writevector4_sffff",   'WVC4', n_writevector4);
    cl->AddCmd("b_writevector3_sfff",    'WVC3', n_writevector3);
    cl->AddCmd("s_readstring_s",         'RSTR', n_readstring);
    cl->AddCmd("i_readint_s",            'RINT', n_readint);
    cl->AddCmd("f_readfloat_s",          'RFLT', n_readfloat);
    cl->AddCmd("b_readbool_s",           'RBOL', n_readbool);
    cl->AddCmd("ffff_readvector4_s",     'RVC4', n_readvector4);
    cl->AddCmd("fff_readvector3_s",      'RVC3', n_readvector3);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setcompanyname
    @input
    s(CompanyName)
    @output
    v
    @info
    Set company name for this application. This is used together with the
    application name to form an unique place where information is stored.
*/
static void
n_setcompanyname(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    self->SetCompanyName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getcompanyname
    @input
    @output
    v
    s(CompanyName)
    @info
    Get company name for this application.
*/
static void
n_getcompanyname(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetS(self->GetCompanyName().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setapplicationname
    @input
    s(ApplicationName)
    @output
    v
    @info
    Set the application name.
*/
static void
n_setapplicationname(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    self->SetApplicationName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getapplicationname
    @input
    v
    @output
    s(ApplicationName)
    @info
    Get application name.
*/
static void
n_getapplicationname(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetS(self->GetApplicationName().Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    open
    @input
    v
    @output
    b(Success)
    @info
    Opens the preferences server. A valid company and application name
    must be set before opening the server.
*/
static void
n_open(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetB(self->Open());
}

//------------------------------------------------------------------------------
/**
    @cmd
    close
    @input
    v
    @output
    v
    @info
    Closes the preferences server.
*/
static void
n_close(void* slf, nCmd* /*cmd*/)
{
    nPrefServer* self = (nPrefServer*) slf;
    self->Close();
}

//------------------------------------------------------------------------------
/**
    @cmd
    isopen
    @input
    v
    @output
    b(OpenFlag)
    @info
    Return true if the prefs server is open.
*/
static void
n_isopen(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetB(self->IsOpen());
}

//------------------------------------------------------------------------------
/**
    @cmd
    keyexists
    @input
    v
    @output
    b(KeyExists)
    @info
    Returns true if a key of that name exists.
*/
static void
n_keyexists(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetB(self->KeyExists(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    writestring
    @input
    s(Key), s(Value)
    @output
    b(Success)
    @info
    Write a persistent string.
*/
static void
n_writestring(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    nString s0 = cmd->In()->GetS();
    nString s1 = cmd->In()->GetS();
    cmd->Out()->SetB(self->WriteString(s0.Get(), s1.Get()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    writeint
    @input
    s(Key), i(Value)
    @output
    b(Success)
    @info
    Write a persistent int value.
*/
static void
n_writeint(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    nString s0 = cmd->In()->GetS();
    int i0 = cmd->In()->GetI();
    cmd->Out()->SetB(self->WriteInt(s0.Get(), i0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    writefloat
    @input
    s(Key), f(Value)
    @output
    b(Success)
    @info
    Write a persistent float value.
*/
static void
n_writefloat(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    nString s0 = cmd->In()->GetS();
    float f0 = cmd->In()->GetF();
    cmd->Out()->SetB(self->WriteFloat(s0.Get(), f0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    writebool
    @input
    s(Key), b(Value)
    @output
    b(Success)
    @info
    Write a persistent bool value.
*/
static void
n_writebool(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    nString s0 = cmd->In()->GetS();
    bool b0 = cmd->In()->GetB();
    cmd->Out()->SetB(self->WriteBool(s0.Get(), b0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    writevector4
    @input
    s(Key), ffff(xyzw)
    @output
    b(Success)
    @info
    Write a persistent vector4 value.
*/
static void
n_writevector4(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    nString s0 = cmd->In()->GetS();
    vector4 v0;
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v0.z = cmd->In()->GetF();
    v0.w = cmd->In()->GetF();
    cmd->Out()->SetB(self->WriteVector4(s0.Get(), v0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    writevector3
    @input
    s(Key), fff(xyz)
    @output
    b(Success)
    @info
    Write a persistent vector3 value.
*/
static void
n_writevector3(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    nString s0 = cmd->In()->GetS();
    vector3 v0;
    v0.x = cmd->In()->GetF();
    v0.y = cmd->In()->GetF();
    v0.z = cmd->In()->GetF();
    cmd->Out()->SetB(self->WriteVector3(s0.Get(), v0));
}

//------------------------------------------------------------------------------
/**
    @cmd
    readstring
    @input
    s(Key)
    @output
    s(Value)
    @info
    Read a persistent string value.
*/
static void
n_readstring(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetS(self->ReadString(cmd->In()->GetS()).Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    readint
    @input
    s(Key)
    @output
    i(Value)
    @info
    Read a persistent int value.
*/
static void
n_readint(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetI(self->ReadInt(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    readfloat
    @input
    s(Key)
    @output
    f(Value)
    @info
    Read a persistent float value.
*/
static void
n_readfloat(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetF(self->ReadFloat(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    readbool
    @input
    s(Key)
    @output
    b(Value)
    @info
    Read a persistent bool value.
*/
static void
n_readbool(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    cmd->Out()->SetB(self->ReadBool(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    readvector4
    @input
    s(Key)
    @output
    ffff(xyzw)
    @info
    Read a persistent vector4 value.
*/
static void
n_readvector4(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    vector4 value = self->ReadVector4(cmd->In()->GetS());
    cmd->Out()->SetF(value.x);
    cmd->Out()->SetF(value.y);
    cmd->Out()->SetF(value.z);
    cmd->Out()->SetF(value.w);
}

//------------------------------------------------------------------------------
/**
    @cmd
    readvector3
    @input
    s(Key)
    @output
    fff(xyz)
    @info
    Read a persistent vector3 value.
*/
static void
n_readvector3(void* slf, nCmd* cmd)
{
    nPrefServer* self = (nPrefServer*) slf;
    vector3 value = self->ReadVector3(cmd->In()->GetS());
    cmd->Out()->SetF(value.x);
    cmd->Out()->SetF(value.y);
    cmd->Out()->SetF(value.z);
}
