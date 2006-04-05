//------------------------------------------------------------------------------
//  nparticleshapenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nparticleshapenode.h"
#include "kernel/npersistserver.h"

static void n_setemissionduration(void* slf, nCmd* cmd);
static void n_getemissionduration(void* slf, nCmd* cmd);
static void n_setloop(void* slf, nCmd* cmd);
static void n_getloop(void* slf, nCmd* cmd);
static void n_setactivitydistance(void* slf, nCmd* cmd);
static void n_getactivitydistance(void* slf, nCmd* cmd);
static void n_setspreadangle(void* slf, nCmd* cmd);
static void n_getspreadangle(void* slf, nCmd* cmd);
static void n_setbirthdelay(void* slf, nCmd* cmd);
static void n_getbirthdelay(void* slf, nCmd* cmd);
static void n_setstartrotation(void* slf, nCmd* cmd);
static void n_getstartrotation(void* slf, nCmd* cmd);
static void n_setrenderoldestfirst(void* slf, nCmd* cmd);
static void n_getrenderoldestfirst(void* slf, nCmd* cmd);
static void n_setglobalscale(void* slf, nCmd* cmd);
static void n_getglobalscale(void* slf, nCmd* cmd);
static void n_setparticlesfollownode(void* slf, nCmd* cmd);
static void n_getparticlesfollownode(void* slf, nCmd* cmd);

static void n_setemissionfrequency(void* slf, nCmd* cmd);
static void n_getemissionfrequency(void* slf, nCmd* cmd);
static void n_setparticlelifetime(void* slf, nCmd* cmd);
static void n_getparticlelifetime(void* slf, nCmd* cmd);
static void n_setparticlestartvelocity(void* slf, nCmd* cmd);
static void n_getparticlestartvelocity(void* slf, nCmd* cmd);
static void n_setparticlerotationvelocity(void* slf, nCmd* cmd);
static void n_getparticlerotationvelocity(void* slf, nCmd* cmd);
static void n_setparticlescale(void* slf, nCmd* cmd);
static void n_getparticlescale(void* slf, nCmd* cmd);
static void n_setparticleweight(void* slf, nCmd* cmd);
static void n_getparticleweight(void* slf, nCmd* cmd);
static void n_setparticlergb(void* slf, nCmd* cmd);
static void n_getparticlergb(void* slf, nCmd* cmd);
static void n_setparticlealpha(void* slf, nCmd* cmd);
static void n_getparticlealpha(void* slf, nCmd* cmd);
static void n_setparticlesidevelocity1(void* slf, nCmd* cmd);
static void n_getparticlesidevelocity1(void* slf, nCmd* cmd);
static void n_setparticlesidevelocity2(void* slf, nCmd* cmd);
static void n_getparticlesidevelocity2(void* slf, nCmd* cmd);
static void n_setparticleairresistance(void* slf, nCmd* cmd);
static void n_getparticleairresistance(void* slf, nCmd* cmd);
static void n_setparticlevelocityfactor(void* slf, nCmd* cmd);
static void n_getparticlevelocityfactor(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nparticleshapenode

    @cppclass
    nParticleShapeNode

    @superclass
    nshapenode

    @classinfo
    Take input mesh and render particles coming out of each vertex
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setemissionduration_f", 'SEMD', n_setemissionduration);
    cl->AddCmd("f_getemissionduration_v", 'GEMD', n_getemissionduration);
    cl->AddCmd("v_setloop_b", 'SLOP', n_setloop);
    cl->AddCmd("b_getloop_v", 'GLOP', n_getloop);
    cl->AddCmd("v_setactivitydistance_f", 'SACD', n_setactivitydistance);
    cl->AddCmd("f_getactivitydistance_v", 'GACD', n_getactivitydistance);
    cl->AddCmd("v_setspreadangle_f", 'SSPA', n_setspreadangle);
    cl->AddCmd("f_getspreadangle_v", 'GSPA', n_getspreadangle);
    cl->AddCmd("v_setbirthdelay_f", 'SBRD', n_setbirthdelay);
    cl->AddCmd("f_getbirthdelay_v", 'GBRD', n_getbirthdelay);
    cl->AddCmd("v_setstartrotation_f", 'SSTR', n_setstartrotation);
    cl->AddCmd("f_getstartrotation_v", 'GSTR', n_getstartrotation);
    cl->AddCmd("v_setrenderoldestfirst_b", 'SROF', n_setrenderoldestfirst);
    cl->AddCmd("b_getrenderoldestfirst_v", 'GROF', n_getrenderoldestfirst);
    cl->AddCmd("v_setglobalscale_f", 'SGSC', n_setglobalscale);
    cl->AddCmd("f_getglobalscale_v", 'GGSC', n_getglobalscale);
    cl->AddCmd("v_setparticlesfollownode_b", 'SPFN', n_setparticlesfollownode);
    cl->AddCmd("b_getparticlesfollownode_v", 'GPFN', n_getparticlesfollownode);

    cl->AddCmd("v_setemissionfrequency_ffffffffi", 'SEFQ', n_setemissionfrequency);
    cl->AddCmd("ffffffffi_getemissionfrequency_v", 'GEFQ', n_getemissionfrequency);
    cl->AddCmd("v_setparticlelifetime_ffffffffi", 'SPLT', n_setparticlelifetime);
    cl->AddCmd("ffffffffi_getparticlelifetime_v", 'GPLT', n_getparticlelifetime);
    cl->AddCmd("v_setparticlestartvelocity_ffffffffi", 'SPSV', n_setparticlestartvelocity);
    cl->AddCmd("ffffffffi_getparticlestartvelocity_v", 'GPSV', n_getparticlestartvelocity);
    cl->AddCmd("v_setparticlerotationvelocity_ffffffffi", 'SPRV', n_setparticlerotationvelocity);
    cl->AddCmd("ffffffffi_getparticlerotationvelocity_v", 'GPRV', n_getparticlerotationvelocity);
    cl->AddCmd("v_setparticlescale_ffffffffi", 'SPSL', n_setparticlescale);
    cl->AddCmd("ffffffffi_getparticlescale_v", 'GPSL', n_getparticlescale);
    cl->AddCmd("v_setparticleweight_ffffffffi", 'SPWG', n_setparticleweight);
    cl->AddCmd("ffffffffi_getparticleweight_v", 'GPWG', n_getparticleweight);
    cl->AddCmd("v_setparticlergb_ffffffffffffff", 'SPCL', n_setparticlergb);
    cl->AddCmd("ffffffffffffff_getparticlergb_v", 'GPCL', n_getparticlergb);
    cl->AddCmd("v_setparticlealpha_ffffffffi", 'SPAL', n_setparticlealpha);
    cl->AddCmd("ffffffffi_getparticlealpha_v", 'GPAL', n_getparticlealpha);
    cl->AddCmd("v_setparticlesidevelocity1_ffffffffi", 'SPS1', n_setparticlesidevelocity1);
    cl->AddCmd("ffffffffi_getparticlesidevelocity1_v", 'GPS1', n_getparticlesidevelocity1);
    cl->AddCmd("v_setparticlesidevelocity2_ffffffffi", 'SPS2', n_setparticlesidevelocity2);
    cl->AddCmd("ffffffffi_getparticlesidevelocity2_v", 'GPS2', n_getparticlesidevelocity2);
    cl->AddCmd("v_setparticleairresistance_ffffffffi", 'SPAR', n_setparticleairresistance);
    cl->AddCmd("ffffffffi_getparticleairresistance_v", 'GPAR', n_getparticleairresistance);
    cl->AddCmd("v_setparticlevelocityfactor_ffffffffi", 'SPVF', n_setparticlevelocityfactor);
    cl->AddCmd("ffffffffi_getparticlevelocityfactor_v", 'GPVF', n_getparticlevelocityfactor);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
void
SetCurveCmdInput(nCmd* cmd, const nEnvelopeCurve& curve)
{
    cmd->In()->SetF(curve.keyFrameValues[0]);
    cmd->In()->SetF(curve.keyFrameValues[1]);
    cmd->In()->SetF(curve.keyFrameValues[2]);
    cmd->In()->SetF(curve.keyFrameValues[3]);
    cmd->In()->SetF(curve.keyFramePos1);
    cmd->In()->SetF(curve.keyFramePos2);
    cmd->In()->SetF(curve.frequency);
    cmd->In()->SetF(curve.amplitude);
    cmd->In()->SetI(curve.modulationFunc);
}

//------------------------------------------------------------------------------
/**
*/
void
SetCurveCmdOutput(nCmd* cmd, const nEnvelopeCurve& curve)
{
    cmd->Out()->SetF(curve.keyFrameValues[0]);
    cmd->Out()->SetF(curve.keyFrameValues[1]);
    cmd->Out()->SetF(curve.keyFrameValues[2]);
    cmd->Out()->SetF(curve.keyFrameValues[3]);
    cmd->Out()->SetF(curve.keyFramePos1);
    cmd->Out()->SetF(curve.keyFramePos2);
    cmd->Out()->SetF(curve.frequency);
    cmd->Out()->SetF(curve.amplitude);
    cmd->Out()->SetI(curve.modulationFunc);
}

//------------------------------------------------------------------------------
/**
    @cmd
    setemissionduration
    @input
    f
    @output
    v
    @info
    Set emitter emission duration.
*/
static void
n_setemissionduration(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetEmissionDuration(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setloop
    @input
    b
    @output
    v
    @info
    Set if emitter loops.
*/
static void
n_setloop(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetLoop(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setactivitydistance
    @input
    f
    @output
    v
    @info
    Set distance beyond which emitter stops emitting.
*/
static void
n_setactivitydistance(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetActivityDistance(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setspreadangle
    @input
    f
    @output
    v
    @info
    Set angle of particle spreading.
*/
static void
n_setspreadangle(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetSpreadAngle(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setbirthdelay
    @input
    f
    @output
    v
    @info
    Set birth delay of the particles.
*/
static void
n_setbirthdelay(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetBirthDelay(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setstartrotation
    @input
    f
    @output
    v
    @info
    Set maximum start rotation angle of the particles.
*/
static void
n_setstartrotation(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetStartRotation(n_deg2rad(cmd->In()->GetF()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setrenderoldestfirst
    @input
    b
    @output
    v
    @info
    Set whether to render oldest particles first.
*/
static void
n_setrenderoldestfirst(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetRenderOldestFirst(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getrenderoldestfirst
    @input
    v
    @output
    b
    @info
    Get whether to render oldest particles first.
*/
static void
n_getrenderoldestfirst(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetB(self->GetRenderOldestFirst());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setglobalscale
    @input
    f
    @output
    v
    @info
    Set global scale value.
*/
static void n_setglobalscale(void* slf, nCmd* cmd) {
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetGlobalScale(cmd->In()->GetF());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getglobalscale
    @input
    f
    @output
    v
    @info
    Get global scale value.
*/
static void n_getglobalscale(void* slf, nCmd* cmd) {
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetF(self->GetGlobalScale());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlesfollownode
    @input
    b
    @output
    v
    @info
    Set global scale value.
*/
static void n_setparticlesfollownode(void* slf, nCmd* cmd) {
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    self->SetParticlesFollowNode(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlesfollownode
    @input
    v
    @output
    b
    @info
    Get global scale value.
*/
static void n_getparticlesfollownode(void* slf, nCmd* cmd) {
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetB(self->GetParticlesFollowNode());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setemissionfrequency
    @input
    ffffffffi(EmissionFrequency)
    @output
    v
    @info
    Set the particle emission frequency curve.
*/
static void
n_setemissionfrequency(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::EmissionFrequency, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlelifetime
    @input
    ffffffffi(ParticleLifeTime)
    @output
    v
    @info
    Set the particle life time curve.
*/
static void
n_setparticlelifetime(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleLifeTime, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlestartvelocity
    @input
    ffffffffi(ParticleStartVelocity)
    @output
    v
    @info
    Set the particle start velocity curve.
*/
static void
n_setparticlestartvelocity(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleStartVelocity, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlerotationvelocity
    @input
    ffffffffi(ParticleRotationVelocity)
    @output
    v
    @info
    Set the particle rotation velocity curve.
*/
static void
n_setparticlerotationvelocity(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleRotationVelocity, nEnvelopeCurve(
        n_deg2rad(values[0]), n_deg2rad(values[1]), n_deg2rad(values[2]), n_deg2rad(values[3]),
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlescale
    @input
    ffffffffi(ParticleScale)
    @output
    v
    @info
    Set the particle scale curve.
*/
static void
n_setparticlescale(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleScale, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticleweight
    @input
    ffffffffi(ParticleWeight)
    @output
    v
    @info
    Set the particle weight.
*/
static void
n_setparticleweight(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleWeight, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlergb
    @input
    ffffffffffffff(ParticleRGB)
    @output
    v
    @info
    Set the particle rgb curve.
*/
static void
n_setparticlergb(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[14];
    for(int i = 0; i < 14; i++) values[i] = cmd->In()->GetF();
    self->SetRGBCurve(nVector3EnvelopeCurve(
        vector3(values[0], values[1], values[2]),
        vector3(values[3], values[4], values[5]),
        vector3(values[6], values[7], values[8]),
        vector3(values[9], values[10], values[11]),
        values[12], values[13]));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlealpha
    @input
    ffffffffi(ParticleAlpha)
    @output
    v
    @info
    Set the particle alpha curve.
*/
static void
n_setparticlealpha(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleAlpha, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}


//------------------------------------------------------------------------------
/**
    @cmd
    setparticlesidevelocity1
    @input
    ffffffffi(ParticleSideVelocity1)
    @output
    v
    @info
    Set the first particle side velocity curve.
*/
static void
n_setparticlesidevelocity1(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleSideVelocity1, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}


//------------------------------------------------------------------------------
/**
    @cmd
    setparticlesidevelocity2
    @input
    ffffffffi(ParticleSideVelocity2)
    @output
    v
    @info
    Set the second particle side velocity curve.
*/
static void
n_setparticlesidevelocity2(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleSideVelocity2, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticleairresistance
    @input
    ffffffffi(ParticleAirResistance)
    @output
    v
    @info
    Set the particle air resistance curve.
*/
static void
n_setparticleairresistance(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleAirResistance, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setparticlevelocityfactor
    @input
    ffffffffi(ParticleVelocityFactor)
    @output
    v
    @info
    Set the particle velocity factor curve.
*/
static void
n_setparticlevelocityfactor(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticleEmitter::ParticleVelocityFactor, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getemissionduration
    @input
    v
    @output
    f
    @info
    Get emitter emission duration.
*/
static void
n_getemissionduration(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetF((float) self->GetEmissionDuration());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getloop
    @input
    v
    @output
    b
    @info
    Get if emitter loops.
*/
static void
n_getloop(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetB(self->GetLoop());
}



//------------------------------------------------------------------------------
/**
    @cmd
    getactivitydistance
    @input
    v
    @output
    f
    @info
    Get distance beyond which the emitter stops emitting.
*/
static void
n_getactivitydistance(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetF(self->GetActivityDistance());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getspreadangle
    @input
    v
    @output
    f
    @info
    Get angle of particle spreading.
*/
static void
n_getspreadangle(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetF(self->GetSpreadAngle());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getbirthdelay
    @input
    v
    @output
    f
    @info
    Get particle birth delay.
*/
static void
n_getbirthdelay(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetF(self->GetBirthDelay());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getstartrotation
    @input
    v
    @output
    f
    @info
    Get maximum particle start rotation angle.
*/
static void
n_getstartrotation(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    cmd->Out()->SetF(n_rad2deg(self->GetStartRotation()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getemissionfrequency
    @input
    v
    @output
    ffffffffi(EmissionFrequency)
    @info
    Get the particle emission frequency curve.
*/
static void
n_getemissionfrequency(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::EmissionFrequency));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlelifetime
    @input
    v
    @output
    ffffffffi(ParticleLifeTime)
    @info
    Get the particle life time curve.
*/
static void
n_getparticlelifetime(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleLifeTime));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlestartvelocity
    @input
    v
    @output
    ffffffffi(ParticleStartVelocity)
    @info
    Get the particle start velocity curve.
*/
static void
n_getparticlestartvelocity(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleStartVelocity));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlerotationvelocity
    @input
    v
    @output
    ffffffffi(ParticleRotationVelocity)
    @info
    Get the particle rotation velocity curve.
*/
static void
n_getparticlerotationvelocity(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    const nEnvelopeCurve& curve = self->GetCurve(nParticleEmitter::ParticleRotationVelocity);
    cmd->Out()->SetF(n_rad2deg(curve.keyFrameValues[0]));
    cmd->Out()->SetF(n_rad2deg(curve.keyFrameValues[1]));
    cmd->Out()->SetF(n_rad2deg(curve.keyFrameValues[2]));
    cmd->Out()->SetF(n_rad2deg(curve.keyFrameValues[3]));
    cmd->Out()->SetF(curve.keyFramePos1);
    cmd->Out()->SetF(curve.keyFramePos2);
    cmd->Out()->SetF(curve.frequency);
    cmd->Out()->SetF(curve.amplitude);
    cmd->Out()->SetI(curve.modulationFunc);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlescale
    @input
    v
    @output
    ffffffffi(ParticleScale)
    @info
    Get the particle scale curve.
*/
static void
n_getparticlescale(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleScale));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticleweight
    @input
    v
    @output
    ffffffffi(ParticleWeight)
    @info
    Get the particle weight curve.
*/
static void
n_getparticleweight(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleWeight));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlergb
    @input
    v
    @output
    ffffffffffffff(ParticleRGB)
    @info
    Get the particle rgb curve.
*/
static void
n_getparticlergb(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    const nVector3EnvelopeCurve& curve = self->GetRGBCurve();
    int keyFrame;
    for (keyFrame = 0; keyFrame < 4; keyFrame++)
    {
        cmd->Out()->SetF(curve.keyFrameValues[keyFrame].x);
        cmd->Out()->SetF(curve.keyFrameValues[keyFrame].y);
        cmd->Out()->SetF(curve.keyFrameValues[keyFrame].z);
    }
    cmd->Out()->SetF(curve.keyFramePos1);
    cmd->Out()->SetF(curve.keyFramePos2);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlealpha
    @input
    v
    @output
    ffffffffi(ParticleAlpha)
    @info
    Get the particle alpha curve.
*/
static void
n_getparticlealpha(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleAlpha));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlesidevelocity1
    @input
    v
    @output
    ffffffffi(ParticleSideVelocity1)
    @info
    Get the first particle side velocity curve.
*/
static void
n_getparticlesidevelocity1(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleSideVelocity1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlesidevelocity2
    @input
    v
    @output
    ffffffffi(ParticleSideVelocity2)
    @info
    Get the second particle side velocity curve.
*/
static void
n_getparticlesidevelocity2(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleSideVelocity2));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticleairresistance
    @input
    v
    @output
    ffffffffi(ParticleAirResistance)
    @info
    Get the particle air resistance.
*/
static void
n_getparticleairresistance(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleAirResistance));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getparticlevelocityfactor
    @input
    v
    @output
    ffffffffi(ParticleVelocityFactor)
    @info
    Get the particle velocity factor curve.
*/
static void
n_getparticlevelocityfactor(void* slf, nCmd* cmd)
{
    nParticleShapeNode* self = (nParticleShapeNode*) slf;
    SetCurveCmdOutput(cmd, self->GetCurve(nParticleEmitter::ParticleVelocityFactor));
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticleShapeNode::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setemissionfrequency ---
        cmd = ps->GetCmd(this, 'SEFQ');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::EmissionFrequency]);
        ps->PutCmd(cmd);

        //--- setparticlelifetime ---
        cmd = ps->GetCmd(this, 'SPLT');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleLifeTime]);
        ps->PutCmd(cmd);

        //--- setparticlestartvelocity ---
        cmd = ps->GetCmd(this, 'SPSV');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleStartVelocity]);
        ps->PutCmd(cmd);

        //--- setparticlerotationvelocity ---
        cmd = ps->GetCmd(this, 'SPRV');
        const nEnvelopeCurve& curve0 = curves[nParticleEmitter::ParticleRotationVelocity];
        cmd->In()->SetF(n_rad2deg(curve0.keyFrameValues[0]));
        cmd->In()->SetF(n_rad2deg(curve0.keyFrameValues[1]));
        cmd->In()->SetF(n_rad2deg(curve0.keyFrameValues[2]));
        cmd->In()->SetF(n_rad2deg(curve0.keyFrameValues[3]));
        cmd->In()->SetF(curve0.keyFramePos1);
        cmd->In()->SetF(curve0.keyFramePos2);
        cmd->In()->SetF(curve0.frequency);
        cmd->In()->SetF(curve0.amplitude);
        cmd->In()->SetI(curve0.modulationFunc);
        ps->PutCmd(cmd);

        //--- setparticlescale ---
        cmd = ps->GetCmd(this, 'SPSL');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleScale]);
        ps->PutCmd(cmd);

        //--- setparticleweight ---
        cmd = ps->GetCmd(this, 'SPWG');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleWeight]);
        ps->PutCmd(cmd);

        //--- setparticlergb ---
        cmd = ps->GetCmd(this, 'SPCL');
        const nVector3EnvelopeCurve& curve = this->rgbCurve;
        int idx;
        for (idx = 0; idx < 4; idx++)
        {
            cmd->In()->SetF(curve.keyFrameValues[idx].x);
            cmd->In()->SetF(curve.keyFrameValues[idx].y);
            cmd->In()->SetF(curve.keyFrameValues[idx].z);
        }
        cmd->In()->SetF(curve.keyFramePos1);
        cmd->In()->SetF(curve.keyFramePos2);
        ps->PutCmd(cmd);

        //--- setparticlealpha ---
        cmd = ps->GetCmd(this, 'SPAL');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleAlpha]);
        ps->PutCmd(cmd);

        //--- setparticlesidevelocity1 ---
        cmd = ps->GetCmd(this, 'SPS1');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleSideVelocity1]);
        ps->PutCmd(cmd);

        //--- setparticlesidevelocity2 ---
        cmd = ps->GetCmd(this, 'SPS2');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleSideVelocity2]);
        ps->PutCmd(cmd);

        //--- setparticleairresistance ---
        cmd = ps->GetCmd(this, 'SPAR');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleAirResistance]);
        ps->PutCmd(cmd);

        //--- setparticleairresistance ---
        cmd = ps->GetCmd(this, 'SPVF');
        SetCurveCmdInput(cmd, curves[nParticleEmitter::ParticleVelocityFactor]);
        ps->PutCmd(cmd);

        //--- setemissionduration ---
        cmd = ps->GetCmd(this, 'SEMD');
        cmd->In()->SetF((float) this->GetEmissionDuration());
        ps->PutCmd(cmd);

        //--- setloop ---
        cmd = ps->GetCmd(this, 'SLOP');
        cmd->In()->SetB(this->GetLoop());
        ps->PutCmd(cmd);

        //--- setactivitydistance ---
        cmd = ps->GetCmd(this, 'SACD');
        cmd->In()->SetF(this->GetActivityDistance());
        ps->PutCmd(cmd);

        //--- setspreadangle ---
        cmd = ps->GetCmd(this, 'SSPA');
        cmd->In()->SetF(this->GetSpreadAngle());
        ps->PutCmd(cmd);

        //--- setbirthdelay ---
        cmd = ps->GetCmd(this, 'SBRD');
        cmd->In()->SetF(this->GetBirthDelay());
        ps->PutCmd(cmd);

        //--- setstartrotation ---
        cmd = ps->GetCmd(this, 'SSTR');
        cmd->In()->SetF(n_rad2deg(this->GetStartRotation()));
        ps->PutCmd(cmd);

        //--- setrenderoldestfirst ---
        cmd = ps->GetCmd(this, 'SROF');
        cmd->In()->SetB(this->GetRenderOldestFirst());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
