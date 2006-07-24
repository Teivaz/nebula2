//------------------------------------------------------------------------------
//  nparticleshapenode_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/nparticleshapenode2.h"
#include "kernel/npersistserver.h"

static void n_p2setinvisible(void* slf, nCmd* cmd);
static void n_p2setemissionduration(void* slf, nCmd* cmd);
static void n_p2setloop(void* slf, nCmd* cmd);
static void n_p2setactivitydistance(void* slf, nCmd* cmd);
static void n_p2setrenderoldestfirst(void* slf, nCmd* cmd);
static void n_p2setstartrotationmin(void* slf, nCmd* cmd);
static void n_p2setstartrotationmax(void* slf, nCmd* cmd);
static void n_p2setgravity(void* slf, nCmd* cmd);
static void n_p2setparticlestretch(void* slf, nCmd* cmd);
static void n_p2settiletexture(void* slf, nCmd* cmd);
static void n_p2setstretchtostart(void* slf, nCmd* cmd);
static void n_p2setprecalctime(void* slf, nCmd* cmd);
static void n_p2setstretchdetail(void* slf, nCmd* cmd);
static void n_p2setviewanglefade(void* slf, nCmd* cmd);
static void n_p2setstartdelay(void* slf, nCmd* cmd);

static void n_p2setparticlevelocityrandomize(void* slf, nCmd* cmd);
static void n_p2setparticlerotationrandomize(void* slf, nCmd* cmd);
static void n_p2setparticlesizerandomize(void* slf, nCmd* cmd);
static void n_p2setrandomrotdir(void* slf, nCmd* cmd);
static void n_p2setemissionfrequency(void* slf, nCmd* cmd);
static void n_p2setparticlelifetime(void* slf, nCmd* cmd);
static void n_p2setparticlespreadmin(void* slf, nCmd* cmd);
static void n_p2setparticlespreadmax(void* slf, nCmd* cmd);
static void n_p2setparticlestartvelocity(void* slf, nCmd* cmd);
static void n_p2setparticlerotationvelocity(void* slf, nCmd* cmd);
static void n_p2setparticlesize(void* slf, nCmd* cmd);
static void n_p2setparticlemass(void* slf, nCmd* cmd);
static void n_p2settimemanipulator(void* slf, nCmd* cmd);
static void n_p2setparticlevelocityfactor(void* slf, nCmd* cmd);
static void n_p2setparticlergb(void* slf, nCmd* cmd);
static void n_p2setparticlealpha(void* slf, nCmd* cmd);
static void n_p2setparticleairresistance(void* slf, nCmd* cmd);


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

    cl->AddCmd("v_setinvisible_b", 'SINV', n_p2setinvisible);
    cl->AddCmd("v_setemissionduration_f", 'SEMD', n_p2setemissionduration);
    cl->AddCmd("v_setloop_b", 'SLOP', n_p2setloop);
    cl->AddCmd("v_setactivitydistance_f", 'SACD', n_p2setactivitydistance);
    cl->AddCmd("v_setrenderoldestfirst_b", 'SROF', n_p2setrenderoldestfirst);
    cl->AddCmd("v_setstartrotationmin_f", 'SRMN', n_p2setstartrotationmin);
    cl->AddCmd("v_setstartrotationmax_f", 'SRMX', n_p2setstartrotationmax);
    cl->AddCmd("v_setgravity_f", 'SGRV', n_p2setgravity);
    cl->AddCmd("v_setparticlestretch_f", 'SPST', n_p2setparticlestretch);
    cl->AddCmd("v_settiletexture_i", 'STTX', n_p2settiletexture);
    cl->AddCmd("v_setstretchtostart_b", 'SSTS', n_p2setstretchtostart);
    cl->AddCmd("v_setprecalctime_f", 'SPCT', n_p2setprecalctime);
    cl->AddCmd("v_setstretchdetail_i", 'SSDT', n_p2setstretchdetail);
    cl->AddCmd("v_setviewanglefade_b", 'SVAF', n_p2setviewanglefade);
    cl->AddCmd("v_setstartdelay_f", 'STDL', n_p2setstartdelay);

    cl->AddCmd("v_setemissionfrequency_ffffffffi", 'SCVA', n_p2setemissionfrequency);
    cl->AddCmd("v_setparticlelifetime_ffffffffi", 'SCVB', n_p2setparticlelifetime);
    cl->AddCmd("v_setparticlergb_ffffffffffffff", 'SCVC', n_p2setparticlergb);
    cl->AddCmd("v_setparticlespreadmin_ffffffffi", 'SCVD', n_p2setparticlespreadmin);
    cl->AddCmd("v_setparticlespreadmax_ffffffffi", 'SCVE', n_p2setparticlespreadmax);
    cl->AddCmd("v_setparticlestartvelocity_ffffffffi", 'SCVF', n_p2setparticlestartvelocity);
    cl->AddCmd("v_setparticlerotationvelocity_ffffffffi", 'SCVH', n_p2setparticlerotationvelocity);
    cl->AddCmd("v_setparticlesize_ffffffffi", 'SCVJ', n_p2setparticlesize);
    cl->AddCmd("v_setparticlemass_ffffffffi", 'SCVL', n_p2setparticlemass);
    cl->AddCmd("v_settimemanipulator_ffffffffi", 'STMM', n_p2settimemanipulator);
    cl->AddCmd("v_setparticlealpha_ffffffffi", 'SCVM', n_p2setparticlealpha);
    cl->AddCmd("v_setparticlevelocityfactor_ffffffffi", 'SCVN', n_p2setparticlevelocityfactor);
    cl->AddCmd("v_setparticleairresistance_ffffffffi", 'SCVQ', n_p2setparticleairresistance);

    cl->AddCmd("v_setparticlevelocityrandomize_f", 'SCVR', n_p2setparticlevelocityrandomize);
    cl->AddCmd("v_setparticlerotationrandomize_f", 'SCVS', n_p2setparticlerotationrandomize);
    cl->AddCmd("v_setparticlesizerandomize_f", 'SCVT', n_p2setparticlesizerandomize);
    cl->AddCmd("v_setrandomrotdir_b", 'SCVU', n_p2setrandomrotdir);

    cl->EndCmds();
}

void
P2SetCurveCmdInput(nCmd* cmd, const nEnvelopeCurve& curve)
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


static void
n_p2setemissionduration(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetEmissionDuration(cmd->In()->GetF());
}
static void
n_p2setparticlevelocityrandomize(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetParticleVelocityRandomize(cmd->In()->GetF());
}
static void
n_p2setparticlerotationrandomize(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetParticleRotationRandomize(cmd->In()->GetF());
}
static void
n_p2setparticlesizerandomize(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetParticleSizeRandomize(cmd->In()->GetF());
}
static void
n_p2setprecalctime(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetPrecalcTime(cmd->In()->GetF());
}

static void
n_p2setstartdelay(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetStartDelay(cmd->In()->GetF());
}

static void
n_p2setrandomrotdir(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetRandomRotDir(cmd->In()->GetB());
}


static void
n_p2setparticlestretch(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetParticleStretch(cmd->In()->GetF());
}
static void
n_p2settiletexture(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetTileTexture(cmd->In()->GetI());
}
static void
n_p2setstretchtostart(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetStretchToStart(cmd->In()->GetB());
}
static void
n_p2setinvisible(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetInvisible(cmd->In()->GetB());
}
static void
n_p2setstartrotationmin(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetStartRotationMin(cmd->In()->GetF());
}
static void
n_p2setstartrotationmax(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetStartRotationMax(cmd->In()->GetF());
}
static void
n_p2setgravity(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetGravity(cmd->In()->GetF());
}

static void
n_p2setloop(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetLoop(cmd->In()->GetB());
}

static void
n_p2setactivitydistance(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetActivityDistance(cmd->In()->GetF());
}

static void
n_p2setrenderoldestfirst(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetRenderOldestFirst(cmd->In()->GetB());
}

static void
n_p2setstretchdetail(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetStretchDetail(cmd->In()->GetI());
}

static void
n_p2setviewanglefade(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    self->SetViewAngleFade(cmd->In()->GetB());
}

static void
n_p2setemissionfrequency(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::EmissionFrequency, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlelifetime(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleLifeTime, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlespreadmin(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleSpreadMin, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlespreadmax(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleSpreadMax, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlestartvelocity(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleStartVelocity, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlerotationvelocity(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleRotationVelocity, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlesize(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleScale, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

static void
n_p2setparticlergb(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[14];
    for(int i = 0; i < 14; i++) values[i] = cmd->In()->GetF();
    self->SetRGBCurve(nVector3EnvelopeCurve(
        vector3(values[0], values[1], values[2]),
        vector3(values[3], values[4], values[5]),
        vector3(values[6], values[7], values[8]),
        vector3(values[9], values[10], values[11]),
        values[12], values[13]));
}

static void
n_p2setparticlealpha(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleAlpha, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}
static void
n_p2setparticlevelocityfactor(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleVelocityFactor, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}
static void
n_p2setparticlemass(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleMass, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}
static void
n_p2settimemanipulator(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::TimeManipulator, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}
static void
n_p2setparticleairresistance(void* slf, nCmd* cmd)
{
    nParticleShapeNode2* self = (nParticleShapeNode2*) slf;
    float values[8];
    for(int i = 0; i < 8; i++) values[i] = cmd->In()->GetF();
    self->SetCurve(nParticle2Emitter::ParticleAirResistance, nEnvelopeCurve(
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticleShapeNode2::SaveCmds(nPersistServer* ps)
{
    if (nShapeNode::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setemissionfrequency ---
        cmd = ps->GetCmd(this, 'SCVA');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::EmissionFrequency]);
        ps->PutCmd(cmd);

        //--- setparticlelifetime ---
        cmd = ps->GetCmd(this, 'SCVB');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleLifeTime]);
        ps->PutCmd(cmd);

        //--- set spread min ---
        cmd = ps->GetCmd(this, 'SCVD');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleSpreadMin]);
        ps->PutCmd(cmd);

        //--- set spread max ---
        cmd = ps->GetCmd(this, 'SCVE');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleSpreadMax]);
        ps->PutCmd(cmd);

        //--- set start velocity ---
        cmd = ps->GetCmd(this, 'SCVF');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleStartVelocity]);
        ps->PutCmd(cmd);

        //--- set rotation velocity ---
        cmd = ps->GetCmd(this, 'SCVH');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleRotationVelocity]);
        ps->PutCmd(cmd);

        //--- set scale ---
        cmd = ps->GetCmd(this, 'SCVJ');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleScale]);
        ps->PutCmd(cmd);

        //--- set alpha ---
        cmd = ps->GetCmd(this, 'SCVM');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleAlpha]);
        ps->PutCmd(cmd);

        //--- set mass ---
        cmd = ps->GetCmd(this, 'SCVL');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleMass]);
        ps->PutCmd(cmd);

        //--- set time manipulator ---
        cmd = ps->GetCmd(this, 'STMM');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::TimeManipulator]);
        ps->PutCmd(cmd);

        //--- set velocity factor ---
        cmd = ps->GetCmd(this, 'SCVN');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleVelocityFactor]);
        ps->PutCmd(cmd);

        //--- set air resistance ---
        cmd = ps->GetCmd(this, 'SCVQ');
        P2SetCurveCmdInput(cmd, curves[nParticle2Emitter::ParticleAirResistance]);
        ps->PutCmd(cmd);

        //--- setparticlergb ---
        cmd = ps->GetCmd(this, 'SCVC');
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

        //--- setrenderoldestfirst ---
        cmd = ps->GetCmd(this, 'SROF');
        cmd->In()->SetB(this->GetRenderOldestFirst());
        ps->PutCmd(cmd);

        //--- set rotation min ---
        cmd = ps->GetCmd(this, 'SRMN');
        cmd->In()->SetF((float) this->startRotationMin);
        ps->PutCmd(cmd);

        //--- set rotation max ---
        cmd = ps->GetCmd(this, 'SRMX');
        cmd->In()->SetF((float) this->startRotationMax);
        ps->PutCmd(cmd);

        //--- set gravity ---
        cmd = ps->GetCmd(this, 'SGRV');
        cmd->In()->SetF((float) this->gravity);
        ps->PutCmd(cmd);

        //--- set stretch ---
        cmd = ps->GetCmd(this, 'SPST');
        cmd->In()->SetF((float) this->particleStretch);
        ps->PutCmd(cmd);

        //--- set tile texture ---
        cmd = ps->GetCmd(this, 'STTX');
        cmd->In()->SetI((int) this->tileTexture);
        ps->PutCmd(cmd);

        //--- set stretch to start ---
        cmd = ps->GetCmd(this, 'SSTS');
        cmd->In()->SetB((bool) this->stretchToStart);
        ps->PutCmd(cmd);

        //--- set velocity randomization  ---
        cmd = ps->GetCmd(this, 'SCVR');
        cmd->In()->SetF((float) this->particleVelocityRandomize);
        ps->PutCmd(cmd);

        //--- set rotation randomization ---
        cmd = ps->GetCmd(this, 'SCVS');
        cmd->In()->SetF((float) this->particleRotationRandomize);
        ps->PutCmd(cmd);

        //--- set size randomization ---
        cmd = ps->GetCmd(this, 'SCVT');
        cmd->In()->SetF((float) this->particleSizeRandomize);
        ps->PutCmd(cmd);

        //--- set precalculation time ---
        cmd = ps->GetCmd(this, 'SPCT');
        cmd->In()->SetF((float) this->precalcTime);
        ps->PutCmd(cmd);

        //--- set random rotation dir ---
        cmd = ps->GetCmd(this, 'SCVU');
        cmd->In()->SetB((bool) this->randomRotDir);
        ps->PutCmd(cmd);

        //--- set stretch detail ---
        cmd = ps->GetCmd(this, 'SSDT');
        cmd->In()->SetI((int) this->stretchDetail);
        ps->PutCmd(cmd);

        //--- set view angle fade ---
        cmd = ps->GetCmd(this, 'SVAF');
        cmd->In()->SetB((bool) this->viewAngleFade);
        ps->PutCmd(cmd);

        //--- set start delay ---
        cmd = ps->GetCmd(this, 'STDL');
        cmd->In()->SetF((float) this->startDelay);
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
