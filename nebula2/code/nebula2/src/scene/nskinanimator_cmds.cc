//------------------------------------------------------------------------------
//  nskinanimator_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nskinanimator.h"
#include "kernel/npersistserver.h"

static void n_setanim(void* slf, nCmd* cmd);
static void n_getanim(void* slf, nCmd* cmd);
static void n_beginjoints(void* slf, nCmd* cmd);
static void n_setjoint(void* slf, nCmd* cmd);
static void n_endjoints(void* slf, nCmd* cmd);
static void n_getnumjoints(void* slf, nCmd* cmd);
static void n_getjoint(void* slf, nCmd* cmd);
static void n_setstatechannel(void* slf, nCmd* cmd);
static void n_getstatechannel(void* slf, nCmd* cmd);
static void n_beginstates(void* slf, nCmd* cmd);
static void n_setstate(void* slf, nCmd* cmd);
static void n_setstatename(void* slf, nCmd* cmd);
static void n_endstates(void* slf, nCmd* cmd);
static void n_getnumstates(void* slf, nCmd* cmd);
static void n_getstateat(void* slf, nCmd* cmd);
static void n_beginclips(void* slf, nCmd* cmd);
static void n_setclip(void* slf, nCmd* cmd);
static void n_endclips(void* slf, nCmd* cmd);
static void n_getnumclips(void* slf, nCmd* cmd);
static void n_getclipat(void* slf, nCmd* cmd);
static void n_addjointname(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nskinanimator

    @cppclass
    nSkinAnimator
    
    @superclass
    nanimator

    @classinfo
    Provide an animated joint skeleton for nSkinShape node.
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setanim_s",               'SANM', n_setanim);
    cl->AddCmd("s_getanim_v",               'GANM', n_getanim);
    cl->AddCmd("v_beginjoints_i",           'BJNT', n_beginjoints);
    cl->AddCmd("v_setjoint_iiffffffffff",   'SJNT', n_setjoint);
    cl->AddCmd("v_endjoints_v",             'EJNT', n_endjoints);
    cl->AddCmd("i_getnumjoints_v",          'GNJT', n_getnumjoints);
    cl->AddCmd("iffffffffff_getjoint_i",    'GJNT', n_getjoint);
    cl->AddCmd("v_setstatechannel_s",       'SSCN', n_setstatechannel);
    cl->AddCmd("s_getstatechannel_v",       'GSCN', n_getstatechannel);
    cl->AddCmd("v_beginstates_i",           'BGST', n_beginstates);
    cl->AddCmd("v_setstate_iif",            'SSTT', n_setstate);
    cl->AddCmd("v_setstatename_is",         'SSNM', n_setstatename);
    cl->AddCmd("v_endstates_v",             'ENDS', n_endstates);
    cl->AddCmd("i_getnumstates_v",          'GNST', n_getnumstates);
    cl->AddCmd("if_getstateat_i",           'GSTA', n_getstateat);
    cl->AddCmd("v_beginclips_ii",           'BGCL', n_beginclips);
    cl->AddCmd("v_setclip_iis",             'STCL', n_setclip);
    cl->AddCmd("v_endclips_i",              'EDCL', n_endclips);
    cl->AddCmd("i_getnumclips_i",           'GNCL', n_getnumclips);
    cl->AddCmd("s_getclipat_ii",            'GCLA', n_getclipat);
    cl->AddCmd("v_addjointname_is",         'ADJN', n_addjointname);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanim
    @input
    s(AnimResource)
    @output
    v
    @info
    Set the name of the anim resource.
*/
static void
n_setanim(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->SetAnim(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getanim
    @input
    v
    @output
    s(AnimResource)
    @info
    Get the name of the anim resource.
*/
static void
n_getanim(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    cmd->Out()->SetS(self->GetAnim());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginjoints
    @input
    i(NumJoints)
    @output
    v
    @info
    Begin defining joints.
*/
static void
n_beginjoints(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->BeginJoints(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setjoint
    @input
    i(jointIndex), 
    i(parentJointIndex),
    fff(poseTranslate)
    ffff(poseRotate)
    fff(poseScale)
    @output
    v
    @info
    Set a joint.
*/
static void
n_setjoint(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    vector3 poseTranslate, poseScale;
    quaternion poseRotate;
    int jointIndex = cmd->In()->GetI();
    int parentJointIndex = cmd->In()->GetI();
    
    poseTranslate.x = cmd->In()->GetF();
    poseTranslate.y = cmd->In()->GetF();
    poseTranslate.z = cmd->In()->GetF();

    poseRotate.x = cmd->In()->GetF();
    poseRotate.y = cmd->In()->GetF();
    poseRotate.z = cmd->In()->GetF();
    poseRotate.w = cmd->In()->GetF();

    poseScale.x = cmd->In()->GetF();
    poseScale.y = cmd->In()->GetF();
    poseScale.z = cmd->In()->GetF();

    self->SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endjoints
    @input
    v
    @output
    v
    @info
    Finish defining joints
*/
static void
n_endjoints(void* slf, nCmd* /*cmd*/)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->EndJoints();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumjoints
    @input
    i(JointIndex)
    @output
    i(NumJoints)
    @info
    Get number of joints
*/
static void
n_getnumjoints(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    cmd->Out()->SetI(self->GetNumJoints());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getjoint
    @input
    i(JointIndex)
    @output
    i(ParentJointIndex)
    fff(poseTranslate)
    ffff(poseRotate)
    fff(poseScale)
    @info
    Get joint attributes at given joint index.
*/
static void
n_getjoint(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int parentJoint;
    vector3 poseTranslate, poseScale;
    quaternion poseRotate;

    self->GetJoint(cmd->In()->GetI(), parentJoint, poseTranslate, poseRotate, poseScale);
    cmd->Out()->SetI(parentJoint);

    cmd->Out()->SetF(poseTranslate.x);
    cmd->Out()->SetF(poseTranslate.y);
    cmd->Out()->SetF(poseTranslate.z);

    cmd->Out()->SetF(poseRotate.x);
    cmd->Out()->SetF(poseRotate.y);
    cmd->Out()->SetF(poseRotate.z);
    cmd->Out()->SetF(poseRotate.w);

    cmd->Out()->SetF(poseScale.x);
    cmd->Out()->SetF(poseScale.y);
    cmd->Out()->SetF(poseScale.z);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstatechannel
    @input
    s(StateChannelName)
    @output
    v
    @info
    Set the name of the channel which controls the current animation state.
*/
static void
n_setstatechannel(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->SetStateChannel(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstatechannel
    @input
    v
    @output
    s(StateChannelName)
    @info
    Get the name of the channel which controls the current animation state.
*/
static void
n_getstatechannel(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    cmd->Out()->SetS(self->GetStateChannel());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginstates
    @input
    i(NumStates)
    @output
    v
    @info
    Begin setting animation states.
*/
static void
n_beginstates(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->BeginStates(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstate
    @input
    i(StateIndex), i(AnimGroupIndex), f(FadeInTime)
    @output
    v
    @info
    Define an animation state. 
*/
static void
n_setstate(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int i0, i1;
    float f0;
    i0 = cmd->In()->GetI();
    i1 = cmd->In()->GetI();
    f0 = cmd->In()->GetF();
    self->SetState(i0, i1, f0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstatename
    @input
    i(StateIndex), s(StateName)
    @output
    v
    @info
    Sets an optional state name.
*/
static void
n_setstatename(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int i = cmd->In()->GetI();
    nString s = cmd->In()->GetS();
    self->SetStateName(i, s);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endstates
    @input
    v
    @output
    v
    @info
    Finish defining animation states.
*/
static void
n_endstates(void* slf, nCmd* /*cmd*/)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->EndStates();
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumstates
    @input
    v
    @output
    i(NumStates)
    @info
    Returns number of animation states.
*/
static void
n_getnumstates(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    cmd->Out()->SetI(self->GetNumStates());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstateat
    @input
    i(StateIndex)
    @output
    i(AnimGroupIndex)
    f(FadeInTime)
    @info
    Get state definition at index.
*/
static void
n_getstateat(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    const nAnimState& state = self->GetStateAt(cmd->In()->GetI());
    cmd->Out()->SetI(state.GetAnimGroupIndex());
    cmd->Out()->SetF(state.GetFadeInTime());
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginclips
    @input
    i(StateIndex), i(NumClips)
    @output
    v
    @info
    Begin adding animation clips to a state.    
*/
static void
n_beginclips(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int i0, i1;
    i0 = cmd->In()->GetI();
    i1 = cmd->In()->GetI();
    self->BeginClips(i0, i1);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setclip
    @input
    i(StateIndex), i(ClipIndex), s(WeightChannelName)
    @output
    v
    @info
    Add an animation clip to a state.
*/
static void
n_setclip(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int i0, i1;
    const char* s0;
    i0 = cmd->In()->GetI();
    i1 = cmd->In()->GetI();
    s0 = cmd->In()->GetS();
    self->SetClip(i0, i1, s0);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endclips
    @input
    i(StateIndex)
    @output
    v
    @info
    Finish adding animation clips to a state.
*/
static void
n_endclips(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->EndClips(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumclips
    @input
    i(StateIndex)
    @output
    i(NumClips)
    @info
    Finish adding animation clips to a state.
*/
static void
n_getnumclips(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    cmd->Out()->SetI(self->GetNumClips(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getclipat
    @input
    i(StateIndex), i(ClipIndex)
    @output
    s(ClipWeightChannelName)
    @info
    Get clip information.
*/
static void
n_getclipat(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int clipIndex = cmd->In()->GetI();
    const char* weightChannelName;
    self->GetClipAt(stateIndex, clipIndex, weightChannelName);
    cmd->Out()->SetS(weightChannelName);
}

//------------------------------------------------------------------------------
/**
    @cmd
    addjointname
    @input
    i(JointIndex), s(JointName)
    @output
    v
    @info
    Add a joint name to the skin animator
*/
static void
n_addjointname(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int jointIndex = cmd->In()->GetI();
    const char *nameStr = cmd->In()->GetS();
    
    self->AddJointName(jointIndex, nameStr);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSkinAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setanim ---
        cmd = ps->GetCmd(this, 'SANM');
        cmd->In()->SetS(this->GetAnim());
        ps->PutCmd(cmd);

        //--- beginjoints ---
        int numJoints = this->GetNumJoints();
        cmd = ps->GetCmd(this, 'BJNT');
        cmd->In()->SetI(numJoints);
        ps->PutCmd(cmd);

        //--- setjoint ---
        int jointIndex;
        for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
        {
            int parentJoint;
            vector3 poseTranslate, poseScale;
            quaternion poseRotate;

            this->GetJoint(jointIndex, parentJoint, poseTranslate, poseRotate, poseScale);
            cmd = ps->GetCmd(this, 'SJNT');
            cmd->In()->SetI(jointIndex);
            cmd->In()->SetI(parentJoint);

            cmd->In()->SetF(poseTranslate.x);
            cmd->In()->SetF(poseTranslate.y);
            cmd->In()->SetF(poseTranslate.z);

            cmd->In()->SetF(poseRotate.x);
            cmd->In()->SetF(poseRotate.y);
            cmd->In()->SetF(poseRotate.z);
            cmd->In()->SetF(poseRotate.w);

            cmd->In()->SetF(poseScale.x);
            cmd->In()->SetF(poseScale.y);
            cmd->In()->SetF(poseScale.z);

            ps->PutCmd(cmd);
        }
        
        //--- endjoints ---
        cmd = ps->GetCmd(this, 'EJNT');
        ps->PutCmd(cmd);

        // Add all the joint names
        nStrNode *strNode = this->jointNameList.GetHead();
        while (strNode)
        {
            //--- addjointname ---
            cmd = ps->GetCmd(this, 'ADJN');
            cmd->In()->SetI((unsigned int)strNode->GetPtr());
            cmd->In()->SetS(strNode->GetName());
            ps->PutCmd(cmd);
            
            strNode = strNode->GetSucc();
        }

        //--- setstatechannel ---
        cmd = ps->GetCmd(this, 'SSCN');
        cmd->In()->SetS(this->GetStateChannel());
        ps->PutCmd(cmd);

        int numStates = this->GetNumStates();
        if (numStates > 0)
        {
            //--- beginstates ---
            cmd = ps->GetCmd(this, 'BGST');
            cmd->In()->SetI(numStates);
            ps->PutCmd(cmd);

            int stateIndex;
            for (stateIndex = 0; stateIndex < numStates; stateIndex++)
            {
                const nAnimState& state = this->GetStateAt(stateIndex);

                //--- setstate ---
                cmd = ps->GetCmd(this, 'SSTT');
                cmd->In()->SetI(stateIndex);
                cmd->In()->SetI(state.GetAnimGroupIndex());
                cmd->In()->SetF(state.GetFadeInTime());
                ps->PutCmd(cmd);

                //--- setstatename ---
                if (!state.GetName().IsEmpty())
                {
                    cmd = ps->GetCmd(this, 'SSNM');
                    cmd->In()->SetI(stateIndex);
                    cmd->In()->SetS(state.GetName().Get());
                    ps->PutCmd(cmd);
                }

                int numClips = this->GetNumClips(stateIndex);
                if (numClips > 0)
                {
                    //--- beginclips ---
                    cmd = ps->GetCmd(this, 'BGCL');
                    cmd->In()->SetI(stateIndex);
                    cmd->In()->SetI(numClips);
                    ps->PutCmd(cmd);

                    //--- setclip ---
                    int clipIndex;
                    for (clipIndex = 0; clipIndex < numClips; clipIndex++)
                    {
                        const char* weightChannelName;
                        this->GetClipAt(stateIndex, clipIndex, weightChannelName);

                        cmd = ps->GetCmd(this, 'STCL');
                        cmd->In()->SetI(stateIndex);
                        cmd->In()->SetI(clipIndex);
                        cmd->In()->SetS(weightChannelName);
                        ps->PutCmd(cmd);
                    }

                    //--- endclips ---
                    cmd = ps->GetCmd(this, 'EDCL');
                    cmd->In()->SetI(stateIndex);
                    ps->PutCmd(cmd);
                }
            }

            //--- endstates
            cmd = ps->GetCmd(this, 'ENDS');
            ps->PutCmd(cmd);
        }
        return true;
    }
    return false;
}
