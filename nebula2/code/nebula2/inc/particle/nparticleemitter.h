#ifndef N_PARTICLEEMITTER_H
#define N_PARTICLEEMITTER_H
//------------------------------------------------------------------------------
/**
    @class nParticleEmitter
    @ingroup NebulaParticleSystem
    @brief Emit particles.

    This class manages and renders particles which represented by nParticle.

    (C) 2003 RadonLabs GmbH
*/

#include "particle/nparticleserver.h"
#include "particle/nparticle.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ndynamicmesh.h"
#include "scene/nshapenode.h"
#include "util/nringbuffer.h"
#include "mathlib/envelopecurve.h"
#include "mathlib/bbox.h"
#include "mathlib/vector3envelopecurve.h"

class nParticleServer;

//------------------------------------------------------------------------------
class nParticleEmitter
{
public:
    // enumeration of the envelope curves (not including the color)
    enum CurveType 
    {
        EmissionFrequency = 0,
        ParticleLifeTime,
        ParticleStartVelocity,
        ParticleRotationVelocity,
        ParticleScale,
        ParticleWeight,
        ParticleSideVelocity1,
        ParticleSideVelocity2,
        ParticleAlpha,
        ParticleAirResistance,
        ParticleVelocityFactor,

        CurveTypeCount,
    };

    /// constructor
    nParticleEmitter();
    /// destructor
    virtual ~nParticleEmitter();

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
    /// set the angle of particle spreading
    void SetSpreadAngle(float f);
    /// get the maximum spread angle
    float GetSpreadAngle() const;
    /// set the maximum particle birth delay
    void SetBirthDelay(float f);
    /// get the maximum birth delay
    float GetBirthDelay() const;
    /// set the maximum particle start rotation angle
    void SetStartRotation(float f);
    /// get the maximum particle start rotation angle
    float GetStartRotation() const;
    /// set whether to render oldest or youngest particles first
    void SetRenderOldestFirst(bool b);
    /// get whether to render oldest or youngest particles first
    bool GetRenderOldestFirst() const;
    /// scales the effect as a whole
    void SetScale(float f);
    /// scales the effect as a whole
    float GetScale() const;
    /// set whether particles, once created, follow the emitter if it moves
    void SetParticlesFollowEmitter(bool b);
    /// get whether particles, once created, follow the emitter if it moves
    bool GetParticlesFollowEmitter() const;

    /// set one of the envelope curves
    void SetCurve(CurveType curveType, const nEnvelopeCurve& curve);
    /// get one of the envelope curves
    const nEnvelopeCurve& GetCurve(CurveType curveType) const;

    /// set the rgb color envelope curve
    void SetRGBCurve(const nVector3EnvelopeCurve& curve);
    /// get the rgb color envelope curve
    const nVector3EnvelopeCurve& GetRGBCurve() const;

    /// get the key that uniquely identifies the emitter in the server's pool
    int GetKey() const;

    /// get the particle rotation from the particle rotation envelope curve
    float GetParticleRotationVelocity(float) const;
    /// get the particle scale from the particle scale envelope curve
    float GetParticleScale(float) const;
    /// get the particle scale from the particle scale envelope curve
    float GetParticleWeight(float) const;
    /// get the particle rgb color from the particle color envelope curve
    const vector3& GetParticleRGB(float) const;
    /// get the particle alpha from the particle alpha envelope curve
    float GetParticleAlpha(float) const;
    /// get the first particle side velocity from the envelope curve
    float GetParticleSideVelocity1(float) const;
    /// get the second particle side velocity from the envelope curve
    float GetParticleSideVelocity2(float) const;
    /// get the particle air resistance from the envelope curve
    float GetParticleAirResistance(float) const;
    /// get the particle velocity factor from the envelope curve
    float GetParticleVelocityFactor(float) const;

    /// set the keep-alive flag
    void SetAlive(bool b);
    /// is the emitter alive ?
    bool IsAlive() const;
    /// set the fatal exception flag
    void SetFatalException(bool b);
    /// get the fatal exception flag
    bool GetFatalException() const;
    /// set whether the emitter is allowed to emit new particles
    void SetActive(bool b);
    /// get whether the emitter is allowed to emit new particles
    bool IsActive() const;

    /// erase dead particles, create new
    virtual void Trigger(nTime curTime);
    /// called by particle server
    void Render(nTime curTime);

    /// initializes particle ring buffer; may only be called once
    void SetParticleCount(int count);
    /// return true if particle buffer has elements 
    bool HasParticles() const;

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

protected:
    nRingBuffer<nParticle*>    particleBuffer;
    nDynamicMesh               dynMesh;
    nRef<nMesh2>               refEmitterMesh;
    int                        meshGroupIndex;
    bbox3                      box;
    nFloat4                    wind;

    matrix44        matrix;                 ///< the world space matrix
    float           scale;                  ///< the scale of the effect itself
 
    bool            alive;                  ///< is alive ?
    bool            active;                 ///< still emitting ?
    bool            fatalException;         ///< a fatal exception occured (emitter will be removed)                 
    int             lastEmissionVertex;     ///< last vertex that emitted
    int             randomKey;              ///< random number key

    nTime           startTime;              ///< timestamp of creation
    nTime           lastEmission;           ///< timestamp of last emission in visual time frame
    nTime           prevTime;               ///< timestamp of preview frame

    // emitter settings
    nTime           emissionDuration;       ///< how long shall be emitted ?
    bool            loop;                   ///< loop emitter ?
    float           activityDistance;       ///< distance between viewer and emitter beyond which emitter is inactive
    float           spreadAngle;            ///< angle of emitted particle cone
    float           birthDelay;             ///< maximum delay until particle starts to live
    float           startRotation;          ///< maximum start rotation angle of a new particle
    bool            renderOldestFirst;      ///< whether to render the oldest particles first or the youngest
    bool            particlesFollowEmitter; ///< whether particles, once born, follow the emitter

    int             key;                    ///< unique key identifying the emitter
    static int      nextKey;

    nArray<nEnvelopeCurve> curves;
    nVector3EnvelopeCurve rgbCurve;     ///< curve for the color modulation of the particles

private:
    /// not implemented operator to prevent '=' - assignment
    nParticleEmitter& operator=(const nParticleEmitter &);
};


//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetFatalException(bool b)
{
    this->fatalException = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleEmitter::GetFatalException() const
{
    return this->fatalException;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nParticleEmitter::SetAlive(bool b)
{
    this->alive = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nParticleEmitter::IsAlive() const
{
    return this->alive;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nParticleEmitter::SetActive(bool b)
{
    this->active = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
nParticleEmitter::IsActive() const
{
    return this->active;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetTransform(const matrix44& transform)
{
    this->matrix.set(transform);
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetStartTime(nTime time)
{
    this->startTime = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetParticleCount(int count)
{
    this->particleBuffer.Initialize(count);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool nParticleEmitter::HasParticles() const
{
    return (!this->particleBuffer.IsEmpty());
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetEmitterMesh(nMesh2* newMesh)
{
    this->refEmitterMesh = newMesh;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMesh2* nParticleEmitter::GetEmitterMesh() const
{
    return this->refEmitterMesh.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
int nParticleEmitter::GetKey() const
{
    return this->key;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime nParticleEmitter::GetEmissionDuration() const
{
    return this->emissionDuration;
}


//------------------------------------------------------------------------------
/**
*/
inline
bool nParticleEmitter::GetLoop() const
{
    return this->loop;
}


//------------------------------------------------------------------------------
/**
*/
inline
float nParticleEmitter::GetActivityDistance() const
{
    return this->activityDistance;
}


//------------------------------------------------------------------------------
/**
*/
inline
float nParticleEmitter::GetSpreadAngle() const
{
    return this->spreadAngle;
}

//------------------------------------------------------------------------------
/**
*/
inline
float nParticleEmitter::GetBirthDelay() const
{
    return this->birthDelay;
}

//------------------------------------------------------------------------------
/**
*/
inline
float nParticleEmitter::GetStartRotation() const
{
    return this->startRotation;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetEmissionDuration(nTime time)
{
    this->emissionDuration = time;
}


//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetLoop(bool b)
{
    this->loop = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetActivityDistance(float f)
{
    this->activityDistance = f;
}


//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetSpreadAngle(float f)
{
    this->spreadAngle = f;
}


//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetBirthDelay(float f)
{
    this->birthDelay = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void nParticleEmitter::SetStartRotation(float f)
{
    this->startRotation = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetRenderOldestFirst(bool b)
{
    this->renderOldestFirst = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleEmitter::GetRenderOldestFirst() const
{
    return this->renderOldestFirst;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetScale() const
{
    return this->scale;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetScale(float f)
{
    this->scale = f;
}

//------------------------------------------------------------------------------
/**
    @brief Whether particles, once born, follow the emitter.

    Particles move about between their birth and death; this flag indicates
    whether the movement occurs relative to their starting position, or
    relative to the emitter's current position.  If the emitter is motionless,
    these are equivalent and so the particlesFollowEmitter state is irrelevant.

*/
inline
bool
nParticleEmitter::GetParticlesFollowEmitter() const
{
    return this->particlesFollowEmitter;
}

//------------------------------------------------------------------------------
/**
    @brief Set whether particles follow the emitter around.

    See GetParticlesFollowEmitter for details.
*/
inline
void
nParticleEmitter::SetParticlesFollowEmitter(bool b)
{
    this->particlesFollowEmitter = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetCurve(CurveType curveType, const nEnvelopeCurve& curve)
{
    n_assert(curveType < CurveTypeCount);
    n_assert(curveType >= 0);
    this->curves[curveType].SetParameters(curve);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nEnvelopeCurve&
nParticleEmitter::GetCurve(CurveType curveType) const
{
    n_assert(curveType < CurveTypeCount);
    n_assert(curveType >= 0);
    return this->curves[curveType];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetRGBCurve(const nVector3EnvelopeCurve& curve)
{
    this->rgbCurve.SetParameters(curve);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nVector3EnvelopeCurve&
nParticleEmitter::GetRGBCurve() const
{
    return this->rgbCurve;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleRotationVelocity(float pos) const
{
    return this->curves[ParticleRotationVelocity].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleScale(float pos) const
{
    return this->curves[ParticleScale].GetValue(pos) * this->scale;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleWeight(float pos) const
{
    return this->curves[ParticleWeight].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nParticleEmitter::GetParticleRGB(float pos) const
{
    return this->rgbCurve.GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleAlpha(float pos) const
{
    return this->curves[ParticleAlpha].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleSideVelocity1(float pos) const
{
    return this->curves[ParticleSideVelocity1].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleSideVelocity2(float pos) const
{
    return this->curves[ParticleSideVelocity2].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleAirResistance(float pos) const
{
    return this->curves[ParticleAirResistance].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleEmitter::GetParticleVelocityFactor(float pos) const
{
    return this->curves[ParticleVelocityFactor].GetValue(pos);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetMeshGroupIndex(int index)
{
    this->meshGroupIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nParticleEmitter::GetMeshGroupIndex() const
{
    return this->meshGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetBoundingBox(const bbox3& b)
{
    this->box = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
nParticleEmitter::GetBoundingBox() const
{
    return this->box;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleEmitter::SetWind(const nFloat4& wind)
{
    this->wind = wind;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nFloat4&
nParticleEmitter::GetWind() const
{
    return this->wind;
}

//------------------------------------------------------------------------------
#endif
