//------------------------------------------------------------------------------
//  nparticle_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticle.h"
#include "particle/nparticleemitter.h"

//------------------------------------------------------------------------------
/**
*/
nParticle::nParticle() :
    state(Unborn),
    birthTime(0.0),
    lastTrigger(0.0),
    lifeTime(0.0),
    curPosition(0.0f, 0.0f, 0.0f),
    curVelocity(0.0f, 0.0f, 0.0f),
    sideVector(1.0f, 0.0f, 0.0f),
    curRotation(0.0f),
    emitter(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nParticle::~nParticle()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void nParticle::Initialize(nParticleEmitter* emitter, const vector3& position, const vector3& velocity,
                           nTime birthTime, nTime lifeTime, float rotation)
{
    this->emitter = emitter;
    this->curPosition = position;
    this->curVelocity = velocity;
    this->sideVector = velocity.findortho();
    this->sideVector.norm();
    this->curRotation = rotation;
    this->birthTime = birthTime;
    this->lastTrigger = birthTime;
    this->lifeTime = lifeTime;
    this->state = Unborn;
}

//------------------------------------------------------------------------------
/**
*/
void
nParticle::Trigger(nTime curTime, const vector3& absAccel)
{
    static const vector3 zVector(0.0, 0.0, 1.0); 
    nTime frameTime = curTime - this->lastTrigger;
    nTime curAge = curTime - birthTime;
    float relAge = (float) (curAge / lifeTime); 

    switch (this->state)
    {
        case Unborn:
            if (curAge >= 0.0)
            {
                this->state = Living;
            }
            break;
        
        case Living:
            if (relAge >= 1.0)
            {
                this->state = Dead;
            }
            else
            {
                this->curRotation += this->emitter->GetParticleRotationVelocity(relAge) * (float) frameTime;

                // update velocity
                vector3 oldVelocity = this->curVelocity;
                this->curVelocity += absAccel * this->emitter->GetParticleWeight(relAge) * (float) frameTime;

                // absolute acceleration
                float airResistance = this->emitter->GetParticleAirResistance(relAge);
                const nFloat4& wind = this->emitter->GetWind();
                vector3 windVelocity = vector3(wind.x, wind.y, wind.z) * wind.w * airResistance;
                vector3 finalVelocity = this->curVelocity + windVelocity;
                finalVelocity *= this->emitter->GetParticleVelocityFactor(relAge);
                this->curPosition += finalVelocity * (float) frameTime;

                // keep sideVector orthogonal to velocity
                float len = oldVelocity.len() * this->curVelocity.len();
                if (!n_fequal(len, 0.0f, 0.00001f))
                {
                    float turnAngle = (float) acos(n_clamp((oldVelocity % this->curVelocity) / len, -1.0, 1.0));
                    if (!n_fequal(turnAngle, 0.0f, 0.001f)) 
                    {
                        vector3 turnAxis = oldVelocity * this->curVelocity;
                        turnAxis.norm();
                        this->sideVector.rotate(turnAxis, turnAngle);
                        sideVector.norm();
                    }
                }

                // relative acceleration
                vector3 leftVector = this->sideVector;
                vector3 upVector = leftVector * this->curVelocity;
                upVector.norm();

                this->curPosition += leftVector * this->emitter->GetParticleSideVelocity1(relAge) * (float) frameTime;
                this->curPosition += upVector * this->emitter->GetParticleSideVelocity2(relAge) * (float) frameTime;
            }
            break;

        default:
            break;
    }
    this->lastTrigger = curTime;
}
