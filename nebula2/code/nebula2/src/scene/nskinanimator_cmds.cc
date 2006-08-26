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
static void n_getstatenameat(void* slf, nCmd* cmd);
static void n_beginclips(void* slf, nCmd* cmd);
static void n_setclip(void* slf, nCmd* cmd);
static void n_endclips(void* slf, nCmd* cmd);
static void n_getnumclips(void* slf, nCmd* cmd);
static void n_getclipat(void* slf, nCmd* cmd);
static void n_beginanimeventtracks(void* slf, nCmd* cmd);
static void n_beginanimeventtrack(void* slf, nCmd* cmd);
static void n_setanimevent(void* slf, nCmd* cmd);
static void n_endanimeventtrack(void* slf, nCmd* cmd);
static void n_endanimeventtracks(void* slf, nCmd* cmd);
static void n_setanimenabled(void* slf, nCmd* cmd);
static void n_isanimenabled(void* slf, nCmd* cmd);

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
    cl->AddCmd("v_setanim_s",                       'SANM', n_setanim);
    cl->AddCmd("s_getanim_v",                       'GANM', n_getanim);
    cl->AddCmd("v_beginjoints_i",                   'BJNT', n_beginjoints);
    cl->AddCmd("v_setjoint_iiffffffffffs",          'SJNT', n_setjoint);
    cl->AddCmd("v_endjoints_v",                     'EJNT', n_endjoints);
    cl->AddCmd("i_getnumjoints_v",                  'GNJT', n_getnumjoints);
    cl->AddCmd("iffffffffffs_getjoint_i",           'GJNT', n_getjoint);
    cl->AddCmd("v_setstatechannel_s",               'SSCN', n_setstatechannel);
    cl->AddCmd("s_getstatechannel_v",               'GSCN', n_getstatechannel);
    cl->AddCmd("v_beginstates_i",                   'BGST', n_beginstates);
    cl->AddCmd("v_setstate_iif",                    'SSTT', n_setstate);
    cl->AddCmd("v_setstatename_is",                 'SSNM', n_setstatename);
    cl->AddCmd("v_endstates_v",                     'ENDS', n_endstates);
    cl->AddCmd("i_getnumstates_v",                  'GNST', n_getnumstates);
    cl->AddCmd("if_getstateat_i",                   'GSAT', n_getstateat);
    cl->AddCmd("s_getstatenameat_i",                'GSNA', n_getstatenameat);
    cl->AddCmd("v_beginclips_ii",                   'BGCL', n_beginclips);
    cl->AddCmd("v_setclip_iis",                     'STCL', n_setclip);
    cl->AddCmd("v_endclips_i",                      'EDCL', n_endclips);
    cl->AddCmd("i_getnumclips_i",                   'GNCL', n_getnumclips);
    cl->AddCmd("s_getclipat_ii",                    'GCLA', n_getclipat);
    cl->AddCmd("v_beginanimeventtracks_iii",        'BATS', n_beginanimeventtracks);
    cl->AddCmd("v_beginanimeventtrack_iiisi",       'BATK', n_beginanimeventtrack);
    cl->AddCmd("v_setanimeevent_iiiifffffffffff",   'SAET', n_setanimevent);
    cl->AddCmd("v_endanimeventtrack_iii",           'EATK', n_endanimeventtrack);
    cl->AddCmd("v_endanimeventtracks_ii",           'EATS', n_endanimeventtracks);
    cl->AddCmd("v_setanimenabled_b",                'SANE', n_setanimenabled);
    cl->AddCmd("b_isanimenabled_v",                 'IAEN', n_isanimenabled);
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
    cmd->Out()->SetS(self->GetAnim().Get());
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
    fff(poseScale
    s(name)
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

    nString name(cmd->In()->GetS());

    self->SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale, name);
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
    fff(poseScale
    n(name)
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
    nString name;

    self->GetJoint(cmd->In()->GetI(), parentJoint, poseTranslate, poseRotate, poseScale, name);
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

    cmd->Out()->SetS(name.Get());
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
    getstatenameat
    @input
    i(StateIndex)
    @output
    s(StateName)
    @info
    Get state name at index.
*/
static void
n_getstatenameat(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    const nAnimState& state = self->GetStateAt(cmd->In()->GetI());
    cmd->Out()->SetS(state.GetName().Get());
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
    beginanimeventtracks
    @input
    i(StateIndex),i(ClipIndex),i(numTracks)
    @output
    v
    @info
    Begin adding animation event tracks.
*/
static void
n_beginanimeventtracks(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int clipIndex  = cmd->In()->GetI();
    int numTracks  = cmd->In()->GetI();
    self->BeginAnimEventTracks(stateIndex, clipIndex, numTracks);
}

//------------------------------------------------------------------------------
/**
    @cmd
    beginanimeventtrack
    @input
    i(StateIndex),i(ClipIndex),i(TrackIndex),s(Name),i(numEvents)
    @output
    v
    @info
    Begin and animation event tracks.
*/
static void
n_beginanimeventtrack(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int clipIndex  = cmd->In()->GetI();
    int trackIndex = cmd->In()->GetI();
    const char* name = cmd->In()->GetS();
    int numEvents = cmd->In()->GetI();
    self->BeginAnimEventTrack(stateIndex, clipIndex, trackIndex, name, numEvents);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimevent
    @input
    i(StateIndex),i(ClipIndex),i(TrackIndex),i(EventIndex),f(Time),fff(Translate),ffff(Rotate),fff(Scale)
    @output
    v
    @info
    Set an animation event.
*/
static void
n_setanimevent(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int clipIndex  = cmd->In()->GetI();
    int trackIndex = cmd->In()->GetI();
    int eventIndex = cmd->In()->GetI();
    float time     = cmd->In()->GetF();
    vector3 t, s;
    quaternion q;
    t.x = cmd->In()->GetF();
    t.y = cmd->In()->GetF();
    t.z = cmd->In()->GetF();
    q.x = cmd->In()->GetF();
    q.y = cmd->In()->GetF();
    q.z = cmd->In()->GetF();
    q.w = cmd->In()->GetF();
    s.x = cmd->In()->GetF();
    s.y = cmd->In()->GetF();
    s.z = cmd->In()->GetF();
    self->SetAnimEvent(stateIndex, clipIndex, trackIndex, eventIndex, time, t, q, s);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endanimeventtrack
    @input
    i(StateIndex),i(ClipIndex),i(TrackIndex)
    @output
    v
    @info
    Finish defining an animation event track.
*/
static void
n_endanimeventtrack(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int clipIndex  = cmd->In()->GetI();
    int trackIndex = cmd->In()->GetI();
    self->EndAnimEventTrack(stateIndex, clipIndex, trackIndex);
}

//------------------------------------------------------------------------------
/**
    @cmd
    endanimeventtracks
    @input
    i(StateIndex),i(ClipIndex)
    @output
    v
    @info
    Finish defining an animation event track.
*/
static void
n_endanimeventtracks(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    int stateIndex = cmd->In()->GetI();
    int clipIndex  = cmd->In()->GetI();
    self->EndAnimEventTracks(stateIndex, clipIndex);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setanimenabled
    @input
    b(AnimEnabled)
    @output
    v
    @info
    Enable/disable animation evaluation.
*/
static void
n_setanimenabled(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    self->SetAnimEnabled(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isanimenabled
    @input
    v
    @output
    b(AnimEnabled)
    @info
    Get animation enabled state.
*/
static void
n_isanimenabled(void* slf, nCmd* cmd)
{
    nSkinAnimator* self = (nSkinAnimator*) slf;
    cmd->Out()->SetB(self->IsAnimEnabled());
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
        cmd->In()->SetS(this->GetAnim().Get());
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
            nString name;

            this->GetJoint(jointIndex, parentJoint, poseTranslate, poseRotate, poseScale, name);
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

            cmd->In()->SetS(name.Get());

            ps->PutCmd(cmd);
        }

        //--- endjoints ---
        cmd = ps->GetCmd(this, 'EJNT');
        ps->PutCmd(cmd);

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

            // anim event tracks
            for (stateIndex = 0; stateIndex < numStates; stateIndex++)
            {
                const nAnimState& animState = this->animStateArray.GetStateAt(stateIndex);
                int numClips = this->GetNumClips(stateIndex);
                int clipIndex;
                for (clipIndex = 0; clipIndex < numClips; clipIndex++)
                {
                    nAnimClip& animClip = animState.GetClipAt(clipIndex);
                    int numAnimEventTracks = animClip.GetNumAnimEventTracks();
                    if (numAnimEventTracks > 0)
                    {
                        //--- beginanimeventtracks ---
                        cmd = ps->GetCmd(this, 'BATS');
                        cmd->In()->SetI(stateIndex);
                        cmd->In()->SetI(clipIndex);
                        cmd->In()->SetI(numAnimEventTracks);
                        ps->PutCmd(cmd);

                        int animEventTrackIndex;
                        for (animEventTrackIndex = 0; animEventTrackIndex < numAnimEventTracks; animEventTrackIndex++)
                        {
                            nAnimEventTrack& animEventTrack = animClip.GetAnimEventTrackAt(animEventTrackIndex);
                            int numEvents = animEventTrack.GetNumEvents();

                            //--- beginanimeventtrack ---
                            cmd = ps->GetCmd(this, 'BATK');
                            cmd->In()->SetI(stateIndex);
                            cmd->In()->SetI(clipIndex);
                            cmd->In()->SetI(animEventTrackIndex);
                            cmd->In()->SetS(animEventTrack.GetName().Get());
                            cmd->In()->SetI(numEvents);
                            ps->PutCmd(cmd);

                            int eventIndex;
                            for (eventIndex = 0; eventIndex < numEvents; eventIndex++)
                            {
                                //--- setanimevent ---
                                const nAnimEvent& animEvent = animEventTrack.GetEvent(eventIndex);
                                const vector3& t = animEvent.GetTranslation();
                                const quaternion& q = animEvent.GetQuaternion();
                                const vector3& s = animEvent.GetScale();
                                cmd = ps->GetCmd(this, 'SAET');
                                cmd->In()->SetI(stateIndex);
                                cmd->In()->SetI(clipIndex);
                                cmd->In()->SetI(animEventTrackIndex);
                                cmd->In()->SetI(eventIndex);
                                cmd->In()->SetF(animEvent.GetTime());
                                cmd->In()->SetF(t.x);
                                cmd->In()->SetF(t.y);
                                cmd->In()->SetF(t.z);
                                cmd->In()->SetF(q.x);
                                cmd->In()->SetF(q.y);
                                cmd->In()->SetF(q.z);
                                cmd->In()->SetF(q.w);
                                cmd->In()->SetF(s.x);
                                cmd->In()->SetF(s.y);
                                cmd->In()->SetF(s.z);
                                ps->PutCmd(cmd);
                            }

                            //--- endanimeventtrack ---
                            cmd = ps->GetCmd(this, 'EATK');
                            cmd->In()->SetI(stateIndex);
                            cmd->In()->SetI(clipIndex);
                            cmd->In()->SetI(animEventTrackIndex);
                            ps->PutCmd(cmd);
                        }

                        //--- endanimeventtracks ---
                        cmd = ps->GetCmd(this, 'EATS');
                        cmd->In()->SetI(stateIndex);
                        cmd->In()->SetI(clipIndex);
                        ps->PutCmd(cmd);
                    }
                }
            }
        }
        return true;
    }
    return false;
}
