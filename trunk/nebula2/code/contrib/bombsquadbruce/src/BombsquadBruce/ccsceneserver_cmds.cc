//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------

#include "BombsquadBruce/ccsceneserver.h"

static void n_getalphaflaghandle(void* slf, nCmd* cmd);
static void n_getloddisthandle(void* slf, nCmd* cmd);
static void n_getlodshapehandle(void* slf, nCmd* cmd);


//------------------------------------------------------------------------------
/**
    @scriptclass
    ccsceneserver
    
    @superclass
    nstdsceneserver

    @classinfo
    The standard Crazy Chipmunk scene server.  Works just like nStdSceneServer,
    except that:

    1. Objects can declare themselves alpha-blended, which causes
    them to be rendered after all other objects (to prevent transparent polys
    from "obscuring" opaque ones behind them).  They do this by setting the
    alpha variable, whose handle they can get from the server (its value is
    irrelevant -- only the fact that it exists matters).
    Don't forget to turn ZWrite
    off in the .fx files of objects that use this flag!  IS THAT ACTUALLY A GOOD IDEA?

    2. Objects can declare level of detail categories, with corresponding scene nodes.
    They do this by setting the LODDist variable, which is an array of four floats 
    (and whose handle they can get from this server), as follows:
    The standard mesh will be used for a distance-to-camera of up to LODDist[0]
    Between LODDist[0] and LODDist[1], an alternative (presumably lower poly) model will be substituted
    Between LODDist[1] and LODDist[2], a billboard will be used
    Beyond LODDist[2], the object will not be displayed.

    So if you set the variable to {2000.0, 5000.0, 10000.0}, it means use the standard mesh
    if this object is closer than 2000 worldspace units, then switch to the low poly model
    mesh up to a distance of 5000 units, then the billboard up to 10000, then don't show anything.

    Note that you can remove categories by giving them a range of 0: 
    LODDist of { 2000.0, 2000.0, 5000.0, 10000 } would go straight from standard mesh to billboard.

    LODDist[3] is ignored for now, but you should set it to -1 in case this changes.  

    The LOD variables are used to store the alternate shape nodes:
    LOD[0] is the lowpoly version,
    LOD[1] is the billboard.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("i_getalphaflaghandle_v", 'GAFH', n_getalphaflaghandle);
    clazz->AddCmd("i_getloddisthandle_v", 'GLDH', n_getloddisthandle);
    clazz->AddCmd("i_getlodshapehandle_i", 'GLSH', n_getlodshapehandle);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getalphaflaghandle

    @input
    v

    @output
    i

    @info
    Gets the handle of the variable that must be set
    in the render contexts of objects that want to 
    activate alpha blending mode.
*/
static void n_getalphaflaghandle(void* slf, nCmd* cmd)
{
    CCSceneServer* self = (CCSceneServer*)slf;
    cmd->Out()->SetI( self->GetAlphaVarHandle() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getloddisthandle

    @input
    v

    @output
    i

    @info
    Gets the handle of the variable that must be set
    in the render contexts of objects that want to 
    use level of detail.
*/
static void n_getloddisthandle(void* slf, nCmd* cmd)
{
    CCSceneServer* self = (CCSceneServer*)slf;
    cmd->Out()->SetI( self->GetLODDistVarHandle() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlodshapehandle

    @input
    i (index)

    @output
    i

    @info
    Gets the handle of the render context variable 
    where the index-th level of detail's render
    context is stored.
*/
static void n_getlodshapehandle(void* slf, nCmd* cmd)
{
    CCSceneServer* self = (CCSceneServer*)slf;
    cmd->Out()->SetI( self->GetLODVarHandle( cmd->In()->GetI() ) );
}