//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nmap.h"
#include "kernel/npersistserver.h"

static void n_setheightmap(void* slf, nCmd* cmd);
static void n_getheightmap(void* slf, nCmd* cmd);
static void n_setgridinterval(void* slf, nCmd* cmd);
static void n_getgridinterval(void* slf, nCmd* cmd);
static void n_setheightrange(void* slf, nCmd* cmd);
static void n_getheightrange(void* slf, nCmd* cmd);
static void n_getheight(void* slf, nCmd* cmd);
static void n_getnormal(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nmap
    
    @superclass
    nroot

    @classinfo
    Map data source for a renderer to work off.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_setheightmap_s",    'SHGT', n_setheightmap);
    clazz->AddCmd("s_getheightmap_v",    'GHGT', n_getheightmap);
    clazz->AddCmd("v_setgridinterval_f", 'SGIT', n_setgridinterval);
    clazz->AddCmd("f_getgridinterval_v", 'GGIT', n_getgridinterval);
    clazz->AddCmd("v_setheightrange_ff", 'SHRG', n_setheightrange);
    clazz->AddCmd("ff_getheightrange_v", 'GHRG', n_getheightrange);
    clazz->AddCmd("f_getheight_ff",      'GHXY', n_getheight);
    clazz->AddCmd("fff_getnormal_ff",    'GNXY', n_getnormal);
    clazz->EndCmds();
}

//-------------------------------------------------------------------
/**
    @cmd
    setheightmap

    @input
    s (HeightMapPath)

    @output
    v

    @info
    Define the name of a 24 bpp BMP file which is loaded as height map.
    Only the red channel is used.
*/
//-------------------------------------------------------------------
void n_setheightmap(void *o, nCmd *cmd)
{
    nMap* self = (nMap*) o;
    self->SetHeightMap(cmd->In()->GetS());
}

//-------------------------------------------------------------------
/**
    @cmd
    getheightmap

    @input
    v

    @output
    s (HeightMapPath)

    @info
    Return the current height map filename.
*/
//-------------------------------------------------------------------
void n_getheightmap(void *o, nCmd *cmd)
{
    nMap* self = (nMap*) o;
    cmd->Out()->SetS(self->GetHeightMap());
}

//-------------------------------------------------------------------
/**
    @cmd
    setgridinterval

    @input
    f (interval)

    @output
    v

    @info
    Set the spacing between grid points in metres.
*/
//-------------------------------------------------------------------
void n_setgridinterval(void* o, nCmd *cmd)
{
    nMap* self = (nMap*)o;
    self->SetGridInterval(cmd->In()->GetF());
}

//-------------------------------------------------------------------
/**
    @cmd
    getgridinterval

    @input
    v

    @output
    f (interval)

    @info
    Returns the spacing between grid points in metres.
*/
//-------------------------------------------------------------------
void n_getgridinterval(void* o, nCmd* cmd)
{
    nMap* self = (nMap*)o;
    cmd->Out()->SetF(self->GetGridInterval());
}

//-------------------------------------------------------------------
/**
    @cmd
    setheightrange

    @input
    f (min height), f (max height)

    @output
    v

    @info
    Set range of the terrain height in metres.
*/
//-------------------------------------------------------------------
void n_setheightrange(void* o, nCmd* cmd)
{
    nMap* self = (nMap*)o;
    float min = cmd->In()->GetF();
    float max = cmd->In()->GetF();
    self->SetHeightRange(min, max);
}

//-------------------------------------------------------------------
/**
    @cmd
    getheightrange

    @input
    v

    @output
    f (min height), f (max height)

    @info
    Returns height range in metres.
*/
//-------------------------------------------------------------------
void n_getheightrange(void* o, nCmd* cmd)
{
    nMap* self = (nMap*)o;
    cmd->Out()->SetF(self->GetHeightRangeMin());
    cmd->Out()->SetF(self->GetHeightRangeMax());
}

//-------------------------------------------------------------------
/**
    @cmd
    getheight

    @input
    f (X coordinate), f (Z coordinate)

    @output
    f (interpolated height at specified coordinate)

    @info
    Returns the height at the specified location.  Height ranges from min
    to max.
*/
//-------------------------------------------------------------------
void n_getheight(void* o, nCmd* cmd)
{
    nMap* self = (nMap*)o;
    float x = cmd->In()->GetF();
    float z = cmd->In()->GetF();

    cmd->Out()->SetF(self->GetHeight(x,z));
}
//-------------------------------------------------------------------
/**
    @cmd
    getnormal

    @input
    f (X coordinate), f (Z coordinate)

    @output
    fff (normal xyz)

    @info
    Returns the triangle mesh normal at the specified location.
*/
//-------------------------------------------------------------------
void n_getnormal(void* o, nCmd* cmd)
{
    nMap* self = (nMap*)o;
    float x = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    vector3 normal;
    self->GetNormal(x,z,normal);
    cmd->Out()->SetF(normal.x);
    cmd->Out()->SetF(normal.y);
    cmd->Out()->SetF(normal.z);
}
//------------------------------------------------------------------------------
/**
    @param  ps  writes the nCmd object contents out to a file.
    @return     success or failure
*/
bool
nMap::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd = NULL;

        // setheightmap
        cmd = ps->GetCmd(this, 'SHGT');
        if (NULL != cmd)
        {
            cmd->In()->SetS(GetHeightMap());
            ps->PutCmd(cmd);
        }

        // setgridinterval
        cmd = ps->GetCmd(this, 'SGIT');
        if (NULL != cmd)
        {
            cmd->In()->SetF(GetGridInterval());
            ps->PutCmd(cmd);
        }

        // setheightrange
        cmd = ps->GetCmd(this, 'SHRG');
        if (NULL != cmd)
        {
            cmd->In()->SetF(GetHeightRangeMax());
            cmd->In()->SetF(GetHeightRangeMin());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}

