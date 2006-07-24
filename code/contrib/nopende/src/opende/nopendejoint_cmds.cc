//------------------------------------------------------------------------------
//  (c) 2004    Vadim Macagon
//------------------------------------------------------------------------------
#include "opende/nopendejoint.h"
#include "kernel/npersistserver.h"

static void n_Create( void* slf, nCmd* cmd );
static void n_AttachTo( void* slf, nCmd* cmd );
static void n_GetJointType( void* slf, nCmd* cmd );
static void n_GetBodyName( void* slf, nCmd* cmd );
static void n_GetFeedback( void* slf, nCmd* cmd );
static void n_IsFeedbackEnabled( void* slf, nCmd* cmd );
static void n_SetParam( void* slf, nCmd* cmd );
static void n_GetParam( void* slf, nCmd* cmd );

//------------------------------------------------------------------------------
/**
    @scriptclass
    nopendejoint

    @superclass
    nroot

    @classinfo
    Encapsulates some dJoint functions.
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd( "v_Create_s",                    'CREA', n_Create );
    clazz->AddCmd( "v_AttachTo_ss",                 'AT__', n_AttachTo );
    clazz->AddCmd( "s_GetJointType_v",              'GJT_', n_GetJointType );
    clazz->AddCmd( "s_GetBodyName_i",               'GBN_', n_GetBodyName );
    clazz->AddCmd( "b_IsFeedbackEnabled_v",         'IFBE', n_IsFeedbackEnabled );
    clazz->AddCmd( "ffffffffffff_GetFeedback_v",    'GF__', n_GetFeedback );
    clazz->AddCmd( "v_SetParam_sif",                'SP__', n_SetParam );
    clazz->AddCmd( "f_GetParam_si",                 'GP__', n_GetParam );
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    Create

    @input
    s(NOH path to an nOpendeWorld instance)

    @output
    v

    @info
    Create the underlying ODE object.
*/
static
void n_Create( void* slf, nCmd* cmd )
{
    nOpendeJoint* self = (nOpendeJoint*)slf;
    self->Create( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    AttachTo

    @input
    s(nOpendeBody Name), s(nOpendeBody Name)

    @output
    v

    @info
    Attach the joint to the bodies. Use "none" to specify a body with id 0
    (ie. static environment).
*/
static
void n_AttachTo( void* slf, nCmd* cmd )
{
    nOpendeJoint* self = (nOpendeJoint*)slf;
    const char* b1 = cmd->In()->GetS();
    const char* b2 = cmd->In()->GetS();
    self->AttachTo( b1, b2 );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetJointType

    @input
    v

    @output
    s(JointType Name)

    @info
    JointType name will be one of these values:
        - ball
        - hinge
        - slider
        - contact
        - universal
        - hinge2
        - fixed
        - amotor
*/
static
void n_GetJointType( void* slf, nCmd* cmd )
{
  nOpendeJoint* self = (nOpendeJoint*)slf;
  cmd->Out()->SetS( self->GetJointTypeName() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    GetBodyName

    @input
    i(BodyIndex)

    @output
    s(nOpendeBody Name)

    @info
    BodyIndex can be either 0 (first body) or 1 (second body).
    nOpendeBody name may be "none" to indicate a body with a zero id
    (ie. the static environment).
*/
static
void n_GetBodyName( void* slf, nCmd* cmd )
{
  nOpendeJoint* self = (nOpendeJoint*)slf;
  int index = cmd->In()->GetI();
  cmd->Out()->SetS( self->GetBodyName( index ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    IsFeedbackEnabled

    @input
    v

    @output
    b(True/False)

    @info
    Check if joint feedback is enabled.
    For more info lookup dJointSetFeedback in the ODE manual.
*/
static
void n_IsFeedbackEnabled( void* slf, nCmd* cmd )
{
  nOpendeJoint* self = (nOpendeJoint*)slf;
  cmd->Out()->SetB( self->IsFeedbackEnabled() );
}


//------------------------------------------------------------------------------
/**
    @cmd
    GetFeedback

    @input
    v

    @output
    f(f1.x), f(f1.y), f(f1.z), f(t1.x), f(t1.y), f(t1.z),
    f(f2.x), f(f2.y), f(f2.z), f(t2.x), f(t2.y), f(t2.z)

    @info
    For more info lookup dJointGetFeedback in the ODE manual.
*/
static
void n_GetFeedback( void* slf, nCmd* cmd )
{
  nOpendeJoint* self = (nOpendeJoint*)slf;
  vector3 f1, t1, f2, t2;
  self->GetFeedback( f1, t1, f2, t2 );
  cmd->Out()->SetF( f1.x );
  cmd->Out()->SetF( f1.y );
  cmd->Out()->SetF( f1.z );
  cmd->Out()->SetF( t1.x );
  cmd->Out()->SetF( t1.y );
  cmd->Out()->SetF( t1.z );
  cmd->Out()->SetF( f2.x );
  cmd->Out()->SetF( f2.y );
  cmd->Out()->SetF( f2.z );
  cmd->Out()->SetF( t2.x );
  cmd->Out()->SetF( t2.y );
  cmd->Out()->SetF( t2.z );
}

//------------------------------------------------------------------------------
/**
    @cmd
    SetParam

    @input
    s(ParamName), i(AxisNum), f(ParamValue)

    @output
    v

    @info
    Set a joint parameter.
    ParamName must be one of:
        - lostop
        - histop
        - vel
        - fmax
        - fudgefactor
        - bounce
        - cfm
        - stoperp
        - stopcfm
        - suspensionerp
        - suspensioncfm
    AxisNum must be 1, 2, or 3.

    For more info lookup dJointSet*Param in the ODE manual.
*/
static
void n_SetParam( void* slf, nCmd* cmd )
{
  nOpendeJoint* self = (nOpendeJoint*)slf;
  const char* pName = cmd->In()->GetS();
  int pAxis = cmd->In()->GetI();
  float pVal = cmd->In()->GetF();
  self->SetParam2( pName, pAxis, pVal );
}

//------------------------------------------------------------------------------
/**
  @cmd
  GetParam

  @input
  s(ParamName), i(AxisNum)

  @output
  f(ParamValue)

  @info
  Get the value of a joint parameter.
  Valid values for ParamName and AxisNum are the same as for SetParam.
  For more info lookup dJointGet*Param in the ODE manual.
*/
static
void n_GetParam( void* slf, nCmd* cmd )
{
  nOpendeJoint* self = (nOpendeJoint*)slf;
  const char* pName = cmd->In()->GetS();
  int pAxis = cmd->In()->GetI();
  cmd->Out()->SetF( self->GetParam2( pName, pAxis ) );
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool nOpendeJoint::SaveCmds( nPersistServer* ps )
{
    if ( nRoot::SaveCmds( ps ) )
    {
        nCmd* cmd;

        // Create
        if ( this->ref_World.getname() )
        {
            cmd = ps->GetCmd( this, 'CREA' );
            cmd->In()->SetS( this->ref_World.getname() );
            ps->PutCmd( cmd );
        }

        // AttachTo
        cmd = ps->GetCmd( this, 'AT__' );
        cmd->In()->SetS( this->GetBodyName( 0 ) );
        cmd->In()->SetS( this->GetBodyName( 1 ) );
        ps->PutCmd( cmd );

        return true;
    }
    return false;
}
