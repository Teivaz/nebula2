//------------------------------------------------------------------------------
//  nparticleserver_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticleserver2.h"
#include "kernel/ntimeserver.h"
#include <stdlib.h>
#include <time.h>

nNebulaScriptClass(nParticleServer2, "nroot");
nParticleServer2* nParticleServer2::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nParticleServer2::nParticleServer2() :
    enabled(true),
    floatRandomPool(FloatRandomCount, 0, 0.0f),
    intRandomPool(IntRandomCount, 0, 0),
    globalAccel(0.0f, -1.0f, 0.0f)
{
    n_assert(0 == Singleton);
    Singleton = this;

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
        floatRandomIter++;
        *floatRandomIter = f1;
        floatRandomIter++;
        *floatRandomIter = f2;
        floatRandomIter++;
        *floatRandomIter = f3;
        floatRandomIter++;
     }
}

//------------------------------------------------------------------------------
/**
*/
nParticleServer2::~nParticleServer2()
{
    // delete emitters
    n_assert(emitters.Empty());
    n_assert(Singleton);
    Singleton = 0;
}


//------------------------------------------------------------------------------
/**
*/
nParticle2Emitter*
nParticleServer2::NewParticleEmitter()
{
    nParticle2Emitter* particleEmitter = n_new(nParticle2Emitter);
    this->emitters.PushBack(particleEmitter);
    // n_printf("nParticleServer: particle emitter created!\n");
    return particleEmitter;
}

//------------------------------------------------------------------------------
/**
*/
void nParticleServer2::Trigger()
{

    nTime currentTime = nTimeServer::Instance()->GetTime();

    // trigger emitters
    nArray<nParticle2Emitter*>::iterator emitterIter = this->emitters.Begin();
    while(emitterIter != emitters.End())
    {
        (*emitterIter)->Trigger(currentTime);
        emitterIter++;
    };

};

//------------------------------------------------------------------------------
/**
*/
void nParticleServer2::DeleteParticleEmitter(nParticle2Emitter* emitter)
{
    nArray<nParticle2Emitter*>::iterator emitterIter = this->emitters.Begin();
    while(emitterIter != emitters.End())
    {
        if(*emitterIter == emitter)
        {
            delete emitter;
            emitters.Erase(emitterIter);
            return;
        };
        emitterIter++;
    };
};
