#ifndef N_PARTICLE_H
#define N_PARTICLE_H
//------------------------------------------------------------------------------
/**    
    (C) 2003 RadonLabs GmbH
*/

#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "util/nnode.h"
#include "particle/nparticleemitter.h"

class nParticleEmitter;

//------------------------------------------------------------------------------
class nParticle : public nNode
{
public:
    enum State {
        Unborn,
        Living,
        Dead
    };

    /// constructor
    nParticle();
    /// destructor
    virtual ~nParticle();

    /// initializes all values and sets state to Unborn
    void Initialize(nParticleEmitter* emitter, vector3& position, 
                    vector3& velocity, nTime birthTime, nTime lifeTime, float rotation);

    /// update the particle
    virtual void Trigger(nTime curTime, const vector3& absAccel);

    /// get the current particle position
    vector3 GetCurPosition();
    /// get the current particle rotation
    float GetCurRotation();

    /// set the birth time of the particle
    void SetBirthTime(nTime time);
    /// set the life time of the particle
    void SetLifeTime(nTime time);

    /// get the state of the particle
    State GetState();
    /// set the state of the particle
    void SetState(State state);

    /// returns the age of the particle
    nTime GetAge() const;
    /// returns the relative age of the particle (0.0 .. 1.0), negative if unborn
    float GetRelAge() const;

protected:
    State       state;

    nTime       birthTime;
    nTime       lifeTime;
    nTime       lastTrigger;

    vector3     curVelocity;
    vector3     sideVector;     // vector that is always orthogonal to curVelocity
    vector3     curPosition;
    float       curRotation;

    nParticleEmitter* emitter;
};

//------------------------------------------------------------------------------
/**
*/
inline
nParticle::State nParticle::GetState()
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticle::SetState(State state)
{
    this->state = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticle::SetBirthTime(nTime time)
{
    this->birthTime = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticle::SetLifeTime(nTime time)
{
    this->lifeTime = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3 nParticle::GetCurPosition()
{
    return this->curPosition;
}

//------------------------------------------------------------------------------
/**
*/
inline
float nParticle::GetCurRotation()
{
    return this->curRotation;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime nParticle::GetAge() const
{
    return (this->lastTrigger - this->birthTime);
}

//------------------------------------------------------------------------------
/**
*/
inline
float nParticle::GetRelAge() const
{
    return (float) ((this->lastTrigger - this->birthTime) / this->lifeTime);
}

//------------------------------------------------------------------------------
#endif
