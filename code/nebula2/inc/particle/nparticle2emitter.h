#ifndef N_PARTICLE2EMITTER_H
#define N_PARTICLE2EMITTER_H
//------------------------------------------------------------------------------
/**    
    @class nParticle2Emitter
    @ingroup Particle

    The particle2 emitter class.

    (C) 2003 RadonLabs GmbH
*/

#include "particle/nparticleserver2.h"
#include "particle/nparticle2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ndynamicmesh.h"
#include "scene/nshapenode.h"
#include "util/nringbuffer.h"
#include "mathlib/envelopecurve.h"
#include "mathlib/bbox.h"
#include "mathlib/vector3envelopecurve.h"

// sorry, need a define
#define PARTICLE_TIME_DETAIL    100

class nParticleServer2;

//------------------------------------------------------------------------------
class nParticle2Emitter
{
public:
    // enumeration of the envelope curves
    enum CurveType 
    {
        EmissionFrequency = 0,
        ParticleLifeTime,
        ParticleStartVelocity,
        ParticleRotationVelocity,
        ParticleScale,
        ParticleSpreadMin,
        ParticleSpreadMax,
        ParticleAlpha,
        ParticleAirResistance,
        StaticRGBCurve,
        ParticleVelocityFactor,
        ParticleMass,
        TimeManipulator,
        Filler1,            // filler for getting a CurveTypeCount that's power of two
        Filler2,            // filler for getting a CurveTypeCount that's power of two
        Filler3,            // filler for getting a CurveTypeCount that's power of two
        CurveTypeCount,
    };

    // helper structure for writing into the vertex stream fast
    typedef struct tParticleVertex
    {
        vector3 pos;
        vector3 vel;
        float   u;
        float   v;
        float   alpha;      // alpha and uv-offsets (coded)
        float   rotation;
        float   scale;
        float   color;      // colors get coded into 1 float
    } tParticleVertex;


    /// constructor
    nParticle2Emitter();
    /// destructor
    virtual ~nParticle2Emitter();

    /// set transform matrix
    void SetTransform(const matrix44& transform);

    /// set the start time
    void SetStartTime(nTime time);
    /// set the end time
    void SetEmissionDuration(nTime time);
    /// get the emission duration
    nTime GetEmissionDuration() const;
    /// set if loop emitter or not
    void SetLoop(bool b);
    /// is it a loop emitter ?
    bool GetLoop(void) const;
    /// set the activity distance
    void SetActivityDistance(float f);
    /// get the distance to the viewer beyond which the emitter stops emitting
    float GetActivityDistance() const;
    /// set wether to render oldest or youngest particles first
    void SetRenderOldestFirst(bool b);
    /// get wether to render oldest or youngest particles first
    bool GetRenderOldestFirst() const;

    /// erase dead particles, create new
    virtual void Trigger(nTime curTime);
    /// called by particle server
    void Render(nTime curTime);

    /// Set the mesh that emittes particles
    void SetEmitterMesh(nMesh2*);
    /// get mesh that emits
    nMesh2* GetEmitterMesh() const;

    /// set mesh group index
    void SetMeshGroupIndex(int index);
    /// get mesh group index
    int GetMeshGroupIndex() const;
    /// set bounding box
    void SetBoundingBox(const bbox3& b);
    /// get bounding box
    const bbox3& GetBoundingBox() const;
    /// set the wind
    void SetWind(const nFloat4& wind);
    /// get the wind
    const nFloat4& GetWind() const;

    /// returns true if emitter is ready for emitting
    bool AreResourcesValid();
    
    // Initializing, call once after setting parameters
    void Open();

    // Signal that all necessary values have been set and emitting can begin
    void SetIsSetUp(bool isSetUp);
    // Return if emitter is set up
    bool IsSetUp() const;

    /// set static - curve - references
    void    SetStaticCurvePtr(float* ptr);
    /// set gravity force
    void    SetGravity(float gravity);
    /// set minimum rotation angle at emission
    void    SetStartRotationMin(float value);
    /// set maximum rotation angle at emission
    void    SetStartRotationMax(float value);
    /// set amount (time) of stretching
    void    SetParticleStretch(float value);
    /// set texture tiling parts
    void    SetTileTexture(int value);
    /// set if particles should be stretched to the emission startpoint 
    void    SetStretchToStart(bool value);
    /// set Velocity Randomize
    void    SetParticleVelocityRandomize(float value);
    /// set Rotation Randomize
    void    SetParticleRotationRandomize(float value);
    /// set Size Randomize
    void    SetParticleSizeRandomize(float value);
    /// set random rotation direction
    void    SetRandomRotDir(bool value);
    /// set Precalculation time
    void    SetPrecalcTime(float value);
    /// set random rotation direction
    void    SetStretchDetail(int value);
    /// set random rotation direction
    void    SetViewAngleFade(bool value);
    /// set start delay
    void    SetStartDelay(float value);
   
    /// called when remotecontrol (maya) changes the curves
    void    CurvesChanged();

protected:
    nDynamicMesh dynMesh;
    nMesh2*  meshPtr;
    int meshGroupIndex;
    bbox3 box;
    nFloat4 wind;

    matrix44 matrix;                // the world space matrix

    int  lastEmissionVertex;        // last vertex that emitted

    nTime startTime;                // timestamp of creation
    nTime lastEmission;             // timestamp of last emission in visual time frame

    // emitter settings
    nTime           emissionDuration;         // how long shall be emitted ?
    bool            loop;                     // loop emitter ?
    float           activityDistance;         // distance between viewer and emitter on witch emitter is active
    float           particleStretch;
    float           precalcTime;             
    int             tileTexture;
    bool            renderOldestFirst;         // wether to render the oldest particles first or the youngest
    bool            stretchToStart;
    bool            randomRotDir;
    bool            hasLooped;
    bool            frameWasRendered;
    float           invisibleTime;
    bool            isSleeping;
    int             stretchDetail;
    bool            viewAngleFade;
    float           startDelay;

    float*          pStaticCurve;
    float           gravity;
    float           startRotationMin;
    float           startRotationMax;
    float           particleVelocityRandomize;
    float           particleRotationRandomize;
    float           particleSizeRandomize;

    nParticle2*     particles;          // array of particles
    int             particleCount;      // count of particles
    int             maxParticleCount;   // maximum number of particles

    nTime           remainingTime;      // remaining time from last frame (for correct emitting)

    bool            isOpen;
    bool            isSetUp;

private:
    /// not implemented operator to prevent '=' - assignment
    nParticle2Emitter& operator=(const nParticle2Emitter &);

    void CalculateStep(float fdTime);
    int RenderPure(float* dstVertices,int maxVertices);
    int RenderStretched(float* dstVertices,int maxVertices);
    int RenderStretchedSmooth(float* dstVertices,int maxVertices);
};


//------------------------------------------------------------------------------
/**
*/
inline
void nParticle2Emitter::SetTransform(const matrix44& transform)
{
    this->matrix.set(transform);
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticle2Emitter::SetStartTime(nTime time)
{
    this->startTime = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticle2Emitter::SetEmitterMesh(nMesh2* newMesh)
{
    this->meshPtr = newMesh;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMesh2* nParticle2Emitter::GetEmitterMesh() const
{
//    return this->refEmitterMesh.get();
    return this->meshPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime nParticle2Emitter::GetEmissionDuration() const
{
    return this->emissionDuration;
}


//------------------------------------------------------------------------------
/**
*/
inline
bool nParticle2Emitter::GetLoop() const
{
    return this->loop;
}


//------------------------------------------------------------------------------
/**
*/
inline
float nParticle2Emitter::GetActivityDistance() const
{
    return this->activityDistance;
}



//------------------------------------------------------------------------------
/**
*/
inline
void nParticle2Emitter::SetEmissionDuration(nTime time)
{
    this->emissionDuration = time;
}


//------------------------------------------------------------------------------
/**
*/
inline
void nParticle2Emitter::SetLoop(bool b)
{
    this->loop = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticle2Emitter::SetActivityDistance(float f)
{
    this->activityDistance = f;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nParticle2Emitter::SetRenderOldestFirst(bool b)
{
    this->renderOldestFirst = b;
}
//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticle2Emitter::GetRenderOldestFirst() const
{
    return this->renderOldestFirst;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticle2Emitter::SetMeshGroupIndex(int index)
{
    this->meshGroupIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nParticle2Emitter::GetMeshGroupIndex() const
{
    return this->meshGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticle2Emitter::SetBoundingBox(const bbox3& b)
{
    this->box = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
nParticle2Emitter::GetBoundingBox() const
{
    return this->box;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticle2Emitter::SetWind(const nFloat4& wind)
{
    this->wind = wind;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nFloat4&
nParticle2Emitter::GetWind() const
{
    return this->wind;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nParticle2Emitter::SetIsSetUp(bool isSetUp)
{
    this->isSetUp = isSetUp;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticle2Emitter::IsSetUp() const
{
    return this->isSetUp;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetStaticCurvePtr(float* ptr)
{
    this->pStaticCurve = ptr;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetGravity(float gravity)
{
    this->gravity = gravity;
};


//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetStartRotationMin(float value)
{
    this->startRotationMin = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetStartRotationMax(float value)
{
    this->startRotationMax = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetParticleStretch(float value)
{
    this->particleStretch = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetTileTexture(int value)
{
    this->tileTexture = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetStretchToStart(bool value)
{
    this->stretchToStart = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetParticleVelocityRandomize(float value)
{
    this->particleVelocityRandomize = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetParticleRotationRandomize(float value)
{
    this->particleRotationRandomize = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetParticleSizeRandomize(float value)
{
    this->particleSizeRandomize = value;
};
//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetPrecalcTime(float value)
{
    this->precalcTime = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetRandomRotDir(bool value)
{
    this->randomRotDir = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetStretchDetail(int value)
{
    this->stretchDetail = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetViewAngleFade(bool value)
{
    this->viewAngleFade = value;
};

//------------------------------------------------------------------------------
/**
*/
inline
void    
nParticle2Emitter::SetStartDelay(float value)
{
    this->startDelay = value;
};

//------------------------------------------------------------------------------
#endif
