//------------------------------------------------------------------------------
//  nparticleserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticleserver.h"
#include "kernel/ntimeserver.h"
#include <stdlib.h>
#include <time.h>

nNebulaClass(nParticleServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nParticleServer::nParticleServer() :
    #ifdef __NEBULA_STATS__
    numEmitters(        "parNumEmitters"            ,nArg::Int),
    numActiveEmitters(  "parNumActiveEmitters"      ,nArg::Int),   
    numParticles(       "parNumParticles"           ,nArg::Int),
    numDrawnParticles(  "parNumDrawnParticles"      ,nArg::Int),
    numDrawnPrimitives( "parNumDrawnPrimitives"     ,nArg::Int),
    #endif
    particlePool(MaxParticles, 0, nParticle()),
    freeParticlePool(0, MaxParticles, 0),
    emitterPool(0, 10),
    floatRandomPool(FloatRandomCount, 0, 0.0f),
    intRandomPool(IntRandomCount, 0, 0),
    globalAccel(0.0f, -1.0f, 0.0f)
{
    ParticlePool::iterator particleIter = particlePool.Begin();
    while (particleIter != particlePool.End())
    {
        // store pointers to the free particles in the free particle pool
        freeParticlePool.PushBack(particleIter);
        ++particleIter;
    }

    srand((unsigned int) time(NULL));

    IntRandomPool::iterator intRandomIter = this->intRandomPool.Begin();
    while (intRandomIter != this->intRandomPool.End())
    {
        *intRandomIter = rand();
        intRandomIter ++;
     }

    FloatRandomPool::iterator floatRandomIter = this->floatRandomPool.Begin();
    while (floatRandomIter != this->floatRandomPool.End())
    {
        // get a random number between -1.0f and 1.0f
        float f0 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
        float f1 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
        float f2 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
        float f3 = (2.0f*((float)rand())/((float)RAND_MAX))-1.0f;
 
        float l = n_sqrt(f0*f0 + f1*f1 + f2*f2);
        if (l > 0.0f)
        {
            f0/=l; f1/=l; f2/=l;
        }

        *floatRandomIter = f0;
        floatRandomIter ++;
        *floatRandomIter = f1;
        floatRandomIter ++;
        *floatRandomIter = f2;
        floatRandomIter ++;
        *floatRandomIter = f3;
        floatRandomIter ++;
     }
}

//------------------------------------------------------------------------------
/**
*/
nParticleServer::~nParticleServer()
{
    EmitterPool::iterator emitterIter = this->emitterPool.Begin();
    while (emitterIter != this->emitterPool.End())
    {
        delete *emitterIter;
        ++emitterIter;
    }
}


//------------------------------------------------------------------------------
/**
*/
nParticleEmitter*
nParticleServer::GetParticleEmitter(int key)
{
    nParticleEmitter* emitter = NULL;

    if (!this->emitterPool.Empty())
    {
        for (EmitterPool::iterator emitterIter = this->emitterPool.Begin();
            emitterIter != this->emitterPool.End(); ++emitterIter)
        {
            if ((*emitterIter)->GetKey() == key)
            {
                emitter = *emitterIter;
                break;
            }
        }
    }
    return emitter;
}


//------------------------------------------------------------------------------
/**
*/
nParticleEmitter*
nParticleServer::NewParticleEmitter()
{
    nTime curTime = this->kernelServer->GetTimeServer()->GetTime();

    nParticleEmitter* particleEmitter = new nParticleEmitter();
    this->emitterPool.PushBack(particleEmitter);

    return particleEmitter;
}

//------------------------------------------------------------------------------
/**
*/
void nParticleServer::Trigger()
{
    nTime curTime = this->kernelServer->GetTimeServer()->GetTime();
    #ifdef __NEBULA_STATS__
    int numEmitters  = 0;
    int numParticles = 0;
    //rest stats that are updated extern
    this->numActiveEmitters->SetI(0);
    this->numDrawnParticles->SetI(0);
    this->numDrawnPrimitives->SetI(0);
    #endif
    
    // Trigger active emitters and delete unused
    if (!this->emitterPool.Empty())
    {
        EmitterPool::iterator emitterIter = this->emitterPool.Begin();
        while (emitterIter != this->emitterPool.End())
        {
            nParticleEmitter* emitter = *emitterIter;
            if (!emitter->GetAlive() && !emitter->HasParticles())
            {
                delete emitter;
                emitterIter = emitterPool.EraseQuick(emitterIter);
            }
            else
            {
                emitter->Trigger(curTime);
                ++emitterIter;
                #ifdef __NEBULA_STATS__
                numEmitters++;
                #endif
            }
        }
    }

    // Update all particles
    ParticlePool::iterator particleIter = this->particlePool.Begin();
    ParticlePool::iterator particlePoolEnd = this->particlePool.End();
    while (particleIter != particlePoolEnd)
    {
        if ((nParticle::Unborn == particleIter->GetState()) ||
            (nParticle::Living == particleIter->GetState()))
        {
            particleIter->Trigger(curTime, this->globalAccel);
            #ifdef __NEBULA_STATS__
            numParticles++;
            #endif
        }
        ++particleIter;
    }
    #ifdef __NEBULA_STATS__
    //update stats
    this->numEmitters->SetI(numEmitters);
    this->numParticles->SetI(numParticles);
    #endif 
}

//------------------------------------------------------------------------------
/**
*/
nParticle* nParticleServer::GiveFreeParticle()
{
    if (this->freeParticlePool.Empty())
    {
        return 0;
    }
    else
    {
        nParticle* particle = *(this->freeParticlePool.EraseQuick(this->freeParticlePool.End()-1));
        return particle;
    }
}

//------------------------------------------------------------------------------
/**
*/
void nParticleServer::TakeBackParticle(nParticle* particle)
{
    particle->SetState(nParticle::Unborn);
    this->freeParticlePool.PushBack(particle);
}
