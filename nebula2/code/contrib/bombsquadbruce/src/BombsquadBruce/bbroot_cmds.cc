//------------------------------------------------------------------------------
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/bbroot.h"
#include "kernel/npersistserver.h"
#include "scene/nscenenode.h"

static void n_show(void* slf, nCmd* cmd);
static void n_setshapenode(void* slf, nCmd* cmd);
static void n_setposition(void* slf, nCmd* cmd);
static void n_getposition(void* slf, nCmd* cmd);
static void n_setcollisionradius(void* slf, nCmd* cmd);
static void n_setrotation(void* slf, nCmd* cmd);
static void n_setscale(void* slf, nCmd* cmd);
static void n_setintvariable(void* slf, nCmd* cmd);
static void n_setfloat4variable(void* slf, nCmd* cmd);
static void n_setpathvariable(void* slf, nCmd* cmd);
static void n_setshaderoverridefloat4(void* slf, nCmd* cmd);
static void n_setshaderoverridefloat(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    bbroot
    
    @superclass
    nroot

    @classinfo
    Root class for every visible object in a BBWorld, except terrain.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd("v_show_b",                        'SHOW', n_show);
    clazz->AddCmd("v_setshapenode_s",                'SVIS', n_setshapenode);
    clazz->AddCmd("v_setposition_fff",               'SPOS', n_setposition);
    clazz->AddCmd("fff_getposition_v",               'GPOS', n_getposition);
    clazz->AddCmd("v_setcollisionradius_f",          'SCRD', n_setcollisionradius);
    clazz->AddCmd("v_setrotation_f",                 'SROT', n_setrotation);
    clazz->AddCmd("v_setscale_fff",                  'SSCL', n_setscale);
    clazz->AddCmd("v_setintvariable_ii",             'SIVR', n_setintvariable);
    clazz->AddCmd("v_setfloat4variable_iffff",       'SFFV', n_setfloat4variable);
    clazz->AddCmd("v_setpathvariable_is",            'SPVR', n_setpathvariable);
    clazz->AddCmd("v_setshaderoverridefloat4_sffff", 'SOIV', n_setshaderoverridefloat4);
    clazz->AddCmd("v_setshaderoverridefloat_sf",     'SOFL', n_setshaderoverridefloat);
    clazz->EndCmds();
}
//------------------------------------------------------------------------------
/**
@cmd
show

@input
b

@output
v

@info
The parameter determines whether or not the object will
be visible (whether its shape node will be attached to 
the scene graph), assuming it normally would be (e.g.
it is in the viewing frustum).
*/

static
void
n_show(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
	self->Show( cmd->In()->GetB() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setshapenode

    @input
    s(nShapeNode)

    @output
    v

    @info
    Sets visual node of the object.
*/
static
void
n_setshapenode(void* slf, nCmd* cmd)
{
    BBRoot* self = static_cast<BBRoot*>( slf );
    self->SetShapeNode(cmd->In()->GetS());
}


//------------------------------------------------------------------------------
/**
@cmd
setposition

@input
f(x position), f(y position), f(z position)

@output
v

@info
Sets the position of the object on the terrain
*/

static
void
n_setposition(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
	self->SetPosition( vector3( x, y, z ) );
}
//------------------------------------------------------------------------------
/**
@cmd
setscale

@input
f(x), f(y), f(z)

@output
v

@info
Scales the objects.  Multiple calls are cumulative.
*/

static
void
n_setscale(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
	float x = cmd->In()->GetF();
	float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
	self->SetScale( vector3( x, y, z ) );
}
//------------------------------------------------------------------------------
/**
@cmd
getposition

@input
v

@output
f(x position), f(y position), f(z position)

@info
Gets the position of the object in worldspace
*/

static
void
n_getposition(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
	const vector3& pos = self->GetPosition();
	cmd->Out()->SetF( pos.x );
	cmd->Out()->SetF( pos.y );
    cmd->Out()->SetF( pos.z );
}
//------------------------------------------------------------------------------
/**
@cmd
setcollisionradius

@input
f(x position), f(y position)

@output
v

@info
Sets the collision radius (the radii of other objects aren't allowed to
overlap this)
*/

static
void
n_setcollisionradius(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
	self->SetCollisionRadius( cmd->In()->GetF() );
}


//------------------------------------------------------------------------------
/**
@cmd
setrotation

@input
f(angle in radians)

@output
v

@info
Sets the object's rotation around its own y axis,
(relative to the default model position)
to the provided angle
*/

static
void
n_setrotation(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
	self->SetRotation( cmd->In()->GetF() );
}

//------------------------------------------------------------------------------
/**
@cmd
setintvariable

@input
i(variable handle), i(variable value)

@output
v

@info
Sets an integer variable in the render context.
*/

static
void
n_setintvariable(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
    int handle = cmd->In()->GetI();
	int val = cmd->In()->GetI();
	self->SetVariable( handle, val );
}
//------------------------------------------------------------------------------
/**
@cmd
setfloat4variable

@input
i(variable handle), ffff(variable value)

@output
v

@info
Sets an float4 variable in the render context.
*/

static
void
n_setfloat4variable(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
    int handle = cmd->In()->GetI();
	nFloat4 val;
    val.x = cmd->In()->GetF();
    val.y = cmd->In()->GetF();
    val.z = cmd->In()->GetF();
    val.w = cmd->In()->GetF();
	self->SetVariable( handle, val );
}

//------------------------------------------------------------------------------
/**
@cmd
setpathvariable

@input
i(variable handle), s(NOH path)

@output
v

@info
Sets an object variable in the render context,
pointing to the object at the given path
(which must already exist)
*/

static
void
n_setpathvariable(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
    int handle = cmd->In()->GetI();
	const char* path = cmd->In()->GetS();
	self->SetPathVariable( handle, path );
}
//------------------------------------------------------------------------------
/**
@cmd
setshaderoverridefloat4

@input
i(shader param name), ffff(variable value)

@output
v

@info
Sets a shader override in the render context.
We assume unsigned floats, so a negative value in any 
field is interpreted as "keep the existing value".
If there is no existing value, negative are set to 1.
*/

static
void
n_setshaderoverridefloat4(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
    const char* paramString = cmd->In()->GetS();
    nFloat4 val;
    val.x = cmd->In()->GetF();
    val.y = cmd->In()->GetF();
    val.z = cmd->In()->GetF();
    val.w = cmd->In()->GetF();
    nShaderState::Param param = nShaderState::StringToParam( paramString );
    nShaderParams& overrides = self->m_RenderContext.GetShaderOverrides();
    if( val.x < 0 || val.y < 0 || val.z < 0 || val.w < 0 )
    {
        nFloat4 curVal;
        if( overrides.IsParameterValid( param ) )
        {
            const nShaderArg& arg = overrides.GetArg(param);
            n_assert( arg.GetType() == nShaderState::Float4 );
            curVal = arg.GetFloat4();
        }
        else
        {
            curVal.x = curVal.y = curVal.z = curVal.w = 1.0f;
        }
        if( val.x < 0 )
            val.x = curVal.x;
        if( val.y < 0 )
            val.y = curVal.y;
        if( val.z < 0 )
            val.z = curVal.z;
        if( val.w < 0 )
            val.w = curVal.w;
    }

    overrides.SetArg( param, nShaderArg(val) );
}
//------------------------------------------------------------------------------
/**
@cmd
setshaderoverridefloat

@input
i(shader param name), f(variable value)

@output
v

@info
Sets a shader override in the render context.
*/

static
void
n_setshaderoverridefloat(void* slf, nCmd* cmd)
{
	BBRoot* self = static_cast<BBRoot*>( slf );
    const char* paramString = cmd->In()->GetS();
    self->m_RenderContext.GetShaderOverrides().SetArg( nShaderState::StringToParam( paramString ), nShaderArg(cmd->In()->GetF()) );
}
//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
BBRoot::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
			 nCmd* cmd;
			// setshapenode
			cmd = ps->GetCmd(this, 'SVIS');
			if(cmd)
			{
                cmd->In()->SetS(m_RenderContext.GetRootNode()->GetName());
				ps->PutCmd(cmd);
			}

			// setposition
			cmd = ps->GetCmd(this, 'SPOS');
			if(cmd)
			{
				const vector3 & posVec = GetPosition();
				cmd->In()->SetF(posVec.x);
				cmd->In()->SetF(posVec.z);
				ps->PutCmd(cmd);
			}

        return true;
    }
    return false;
}
