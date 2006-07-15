//-----------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

static void n_setrootpath(void* slf, nCmd* cmd);
static void n_getrootpath(void* slf, nCmd* cmd);
static void n_setrootwindow(void* slf, nCmd* cmd);
static void n_getrootwindow(void* slf, nCmd* cmd);
static void n_addsystemfont(void* slf, nCmd* cmd);
static void n_addcustomfont(void* slf, nCmd* cmd);
static void n_ismouseovergui(void* slf, nCmd* cmd);
static void n_newwindow(void* slf, nCmd* cmd);
static void n_newskin(void* slf, nCmd* cmd);
static void n_setskin(void* slf, nCmd* cmd);
static void n_getskin(void* slf, nCmd* cmd);
static void n_setsystemskin(void* slf, nCmd* cmd);
static void n_getsystemskin(void* slf, nCmd* cmd);
static void n_togglesystemgui(void* slf, nCmd* cmd);
static void n_setdragbox(void* slf, nCmd* cmd);
static void n_getdragbox(void* slf, nCmd* cmd);
static void n_computescreenspacebrushsize(void* slf, nCmd* cmd);
static void n_runcommand(void* slf, nCmd* cmd);
static void n_putevent(void* slf, nCmd* cmd);
static void n_setdisplaysize(void* slf, nCmd* cmd);

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nguiserver

    @cppclass
    nGuiServer

    @superclass
    nroot

    @classinfo
    Handle user interfaces.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setrootpath_s",                  'SRTP', n_setrootpath);
    cl->AddCmd("s_getrootpath_v",                  'GRTP', n_getrootpath);
    cl->AddCmd("v_setrootwindow_s",                'SRTW', n_setrootwindow);
    cl->AddCmd("s_getrootwindow_v",                'GRTW', n_getrootwindow);
    cl->AddCmd("v_addsystemfont_ssibbb",           'ADSF', n_addsystemfont);
    cl->AddCmd("v_addcustomfont_sssibbb",          'ADCF', n_addcustomfont);
    cl->AddCmd("b_ismouseovergui_v",               'IMOG', n_ismouseovergui);
    cl->AddCmd("o_newwindow_sb",                   'NEWW', n_newwindow);
    cl->AddCmd("o_newskin_s",                      'NSKN', n_newskin);
    cl->AddCmd("v_setskin_o",                      'SSKN', n_setskin);
    cl->AddCmd("o_getskin_v",                      'GSKN', n_getskin);
    cl->AddCmd("v_setsystemskin_o",                'SSSK', n_setsystemskin);
    cl->AddCmd("o_getsystemskin_v",                'GSSK', n_getsystemskin);
    cl->AddCmd("v_togglesystemgui_v",              'TGSG', n_togglesystemgui);
    cl->AddCmd("v_setdragbox_o",                   'SDGB', n_setdragbox);
    cl->AddCmd("o_getdragbox_v",                   'GDGB', n_getdragbox);
    cl->AddCmd("ff_computescreenspacebrushsize_s", 'CSBS', n_computescreenspacebrushsize);
    cl->AddCmd("b_runcommand_os",                  'RUNC', n_runcommand);
    cl->AddCmd("v_putevent_os",                    'PUTE', n_putevent);
    cl->AddCmd("v_setdisplaysize_ii",              'SDSZ', n_setdisplaysize);
    cl->EndCmds();
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setrootpath
    @input
    s(RootPath)
    @output
    v
    @info
    Set the root path in Nebula's object hierarchy. Default is "/res/gui".
    This is where all the windows live.
*/
static void
n_setrootpath(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    self->SetRootPath(cmd->In()->GetS());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getrootpath
    @input
    v
    @output
    s(RootPath)
    @info
    Get the root path.
*/
static void
n_getrootpath(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetS(self->GetRootPath());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setrootwindow
    @input
    s(RootWindow)
    @output
    v
    @info
    Set a new root window.
    An empty window identifier implies the null window.
*/
static void 
n_setrootwindow(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    const char* rootWindow = cmd->In()->GetS();
    if( rootWindow && rootWindow[0] == 0 )
        rootWindow = 0;
    self->SetRootWindow(rootWindow);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getrootwindow
    @input
    v
    @output
    s(RootWindow)
    @info
    Get current root window.
*/
static void
n_getrootwindow(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetS(self->GetRootWindow());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addsystemfont
    @input
    s(FontName), s(TypeFace), i(Height), b(Bold), b(Italic), b(Underline)
    @output
    v
    @info
    Add a font definition using a standard system font.
*/
static void
n_addsystemfont(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    int i0 = cmd->In()->GetI();
    bool b0 = cmd->In()->GetB();
    bool b1 = cmd->In()->GetB();
    bool b2 = cmd->In()->GetB();
    self->AddSystemFont(s0, s1, i0, b0, b1, b2);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    addcustomfont
    @input
    s(FontName), s(FontFile), s(TypeFace), i(Height), b(Bold), b(Italic), b(Underline)
    @output
    v
    @info
    Add a font definition using a custom font.
*/
static void
n_addcustomfont(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    const char* s2 = cmd->In()->GetS();
    int i0 = cmd->In()->GetI();
    bool b0 = cmd->In()->GetB();
    bool b1 = cmd->In()->GetB();
    bool b2 = cmd->In()->GetB();
    self->AddCustomFont(s0, s1, s2, i0, b0, b1, b2);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    ismouseovergui
    @input
    v
    @output
    b(MouseOverGui)
    @info
    Returns true if mouse is currently over a GUI element.
*/
static void
n_ismouseovergui(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetB(self->IsMouseOverGui());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    newwindow
    @input
    s(ClassName), b(Visible)
    @output
    o(Window)
    @info
    Create a new window of the given class.
*/
static void
n_newwindow(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    const char* s0 = cmd->In()->GetS();
    bool b0 = cmd->In()->GetB();
    cmd->Out()->SetO(self->NewWindow(s0, b0));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    newskin
    @input
    s(SkinName)
    @output
    o(SkinObject)
    @info
    Create a new skin of the given name.
*/
static void
n_newskin(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetO(self->NewSkin(cmd->In()->GetS()));
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setskin
    @input
    o(SkinObject)
    @output
    v
    @info
    Set the current skin by object (as returned by newskin).
*/
static void
n_setskin(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    nGuiSkin* skin = (nGuiSkin*) cmd->In()->GetO();
    n_assert(skin->IsA(nKernelServer::Instance()->FindClass("nguiskin")));
    self->SetSkin(skin);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getskin
    @input
    v
    @output
    o(SkinObject)
    @info
    Return the current skin object.
*/
static void
n_getskin(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetO(self->GetSkin());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    setsystemskin
    @input
    o(SkinObject)
    @output
    v
    @info
    Set the system skin to use for the Nebula system GUI. This will
    be activated when ToggleSystemGui() is called.
*/
static void
n_setsystemskin(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    nGuiSkin* skin = (nGuiSkin*) cmd->In()->GetO();
    n_assert(skin->IsA(nKernelServer::Instance()->FindClass("nguiskin")));
    self->SetSystemSkin(skin);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getsystemskin
    @input
    v
    @output
    o(SkinObject)
    @info
    Return the system skin object.
*/
static void
n_getsystemskin(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetO(self->GetSystemSkin());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    togglesystemgui
    @input
    v
    @output
    v
    @info
    Toggle Nebula's system GUI.
*/
static void
n_togglesystemgui(void* slf, nCmd* /*cmd*/)
{
    nGuiServer* self = (nGuiServer*) slf;
    self->ToggleSystemGui();
}
//-----------------------------------------------------------------------------
/**
    @cmd
    computescreenspacebrushsize
    @input
    s (brush name)
    @output
    ff (x,y size)
    @info
    Computes the screen space size of a brush if a 1:1 mapping
    is assumed between brush texels and screen pixels.
*/
static void
n_computescreenspacebrushsize(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;    
    const vector2& brushSize = 
        self->ComputeScreenSpaceBrushSize( cmd->In()->GetS() );
    cmd->Out()->SetF( brushSize.x );
    cmd->Out()->SetF( brushSize.y );
}

//-----------------------------------------------------------------------------
/**
    @cmd
    runcommand
    @input
    os (widget object, command string)
    @output
    b 
    @info
    Run given script to nGuiWidget.

    27-Aug-04   kims   created.
*/
static void 
n_runcommand(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    nGuiWidget* widget = (nGuiWidget*) cmd->In()->GetO();
    nString command = cmd->In()->GetS();
    bool ret = self->RunCommand(widget, command);
    cmd->Out()->SetB(ret);
}

//-----------------------------------------------------------------------------
/**
    @cmd
    putevent
    @input
    os (widget object, event type)
    @output
    v 
    @info
    put an event to given widget object.

    27-Aug-04   kims   created.
*/
static void n_putevent(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;

    nGuiWidget* widget   = (nGuiWidget*) cmd->In()->GetO();    
    nGuiEvent::Type type = nGuiEvent::StringToType(cmd->In()->GetS());

    self->PutEvent(nGuiEvent(widget, type));
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setdragbox
    @input
    o(DragBoxObject)
    @output
    v
    @info
    Set pointer to optional global nGuiDragBox object.
*/
static void
n_setdragbox(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    self->SetDragBox((nGuiDragBox*)cmd->In()->GetO());
}

//-----------------------------------------------------------------------------
/**
    @cmd
    getdragbox
    @input
    v
    @output
    o(DragBoxObject)
    @info
    Get pointer to optional global nGuiDragBox object.
*/
static void
n_getdragbox(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    cmd->Out()->SetO(self->GetDragBox());
}
//-----------------------------------------------------------------------------
/**
    @cmd
    setdisplaysize
    @input
    i (x size), i (y size)
    @output
    v 
    @info
    Set the gui server's display size to the given dimensions.

    29-Sep-04   Rafael   created.
*/
static void n_setdisplaysize(void* slf, nCmd* cmd)
{
    nGuiServer* self = (nGuiServer*) slf;
    int xDim = cmd->In()->GetI();
    int yDim = cmd->In()->GetI();    
    self->SetDisplaySize(vector2((float)xDim, (float)yDim));
}
