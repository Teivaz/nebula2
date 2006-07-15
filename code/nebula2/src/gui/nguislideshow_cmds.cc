//------------------------------------------------------------------------------
//  nguislideshow_cmds.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguislideshow.h"

static void n_setinterval(void* slf, nCmd* cmd);
static void n_addpicture(void* slf, nCmd* cmd);
static void n_setloopmode(void* slf, nCmd* cmd);
static void n_removeallpictures(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nguislideshow
    
    @cppclass
    nGuiSlideShow
    
    @superclass
    nGuiWidget

    @classinfo
    Implements a picture slideshow
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setinterval_f",                 'SITV', n_setinterval);
    cl->AddCmd("v_setloopmode_b",                 'SLMD', n_setloopmode);
    cl->AddCmd("v_addpicture_s",                  'APIC', n_addpicture);
    cl->AddCmd("v_removeallpictures_v",           'RMPX', n_removeallpictures);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setinterval
    @input
    d (interval)
    @output
    v
    @info
    Set the interval between two pictures
*/
static void
n_setinterval(void* slf, nCmd* cmd)
{
    nGuiSlideShow* self = (nGuiSlideShow*) slf;
    nTime interval = cmd->In()->GetF();
    self->SetInterval(interval);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setloopmode
    @input
    b (loopmode)
    @output
    v
    @info
    Set if the slideshow should loop
*/
static void
n_setloopmode(void* slf, nCmd* cmd)
{
    nGuiSlideShow* self = (nGuiSlideShow*) slf;
    bool mode = cmd->In()->GetB();
    self->SetLoopMode(mode);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addpicture
    @input
    s (picture)
    @output
    v
    @info
    Add a picture to the slideshow
*/
static void
n_addpicture(void* slf, nCmd* cmd)
{
    nGuiSlideShow* self = (nGuiSlideShow*) slf;
    nString name = cmd->In()->GetS();
    self->AddPicture(name);
}

//------------------------------------------------------------------------------
/**
    @cmd
    removeallpictures
    @input
    v (void)
    @output
    v
    @info
    Removes all pictures from the slideshow
*/
static void
n_removeallpictures(void* slf, nCmd* /*cmd*/)
{
    nGuiSlideShow* self = (nGuiSlideShow*) slf;
    self->RemoveAllPictures();
}
