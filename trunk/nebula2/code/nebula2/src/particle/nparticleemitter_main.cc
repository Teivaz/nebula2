//------------------------------------------------------------------------------
//  nparticle_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "particle/nparticleemitter.h"
#include "particle/nparticle.h"
#include "mathlib/vector.h"

int nParticleEmitter::nextKey = 0;

//------------------------------------------------------------------------------
/**
*/
nParticleEmitter::nParticleEmitter():
    refParticleServer("/sys/servers/particle"),
    refGfxServer("/sys/servers/gfx"),
    key(nextKey++),
    alive(true),
    active(true),
    loop(true),
    fatalException(false),
    activityDistance(100.0f),
    emissionDuration(10.0),
    spreadAngle(0.0f),
    birthDelay(0.0f),
    startRotation(0.0f),
    startTime(-1.0),
    lastEmission(0.0),
    randomKey(0),
    lastEmissionVertex(0),
    meshGroupIndex(0),
    curves(CurveTypeCount, 0, nEnvelopeCurve())
{
    int i;
    for (i = 0; i < 4; i++)
    {
        this->curves[ParticleVelocityFactor].keyFrameValues[i] = 1.0;
    }
}


//------------------------------------------------------------------------------
/**
*/
nParticleEmitter::~nParticleEmitter()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
nParticleEmitter::Trigger(nTime curTime)
{
    nParticleServer* particleServer = this->refParticleServer.get();
    const vector3& globalAccel = particleServer->GetGlobalAccel();
    this->SetAlive(true);

    if (this->startTime < 0.0)  // called for the first time
    {
        this->startTime = curTime;
        this->lastEmission = curTime;
        return;
    }
    nTime diffTime  = curTime - this->lastEmission;
    if (diffTime < 0.0)
    {
        // a time exception, throw the emitter away, so that it will restart 
        // in the next frame
        this->SetFatalException(true);
        return;
    }
    float relAge = (float)((curTime - this->startTime) / this->emissionDuration);

    if (this->particleBuffer.IsValid())
    {
        nParticle** bufferElement = 0;

        // free dead particles
        while (!this->particleBuffer.IsEmpty())
        {
            bufferElement = this->particleBuffer.GetTail();
            if (nParticle::Dead != (*bufferElement)->GetState())
                break; //stop when found the 1. not dead element
            this->particleBuffer.DeleteTail();
            particleServer->TakeBackParticle(*bufferElement);
        }
        
        // trigger living particles and update the bounding box
        this->box.begin_extend();
        while(bufferElement)
        {
            (*bufferElement)->Trigger(curTime, globalAccel);
            this->box.extend((*bufferElement)->GetCurPosition());
            bufferElement = this->particleBuffer.GetNext(bufferElement);
        }       
    }

    if (this->AreResourcesValid() && this->alive)
    {
        // do the emission
        if (!this->particleBuffer.IsValid())
        {
            int maxParticles;
            maxParticles = (int) (this->curves[EmissionFrequency].GetMaxPossibleValue() *
                (this->curves[ParticleLifeTime].GetMaxPossibleValue() + this->birthDelay) * 1.3f);
            this->particleBuffer.Initialize(maxParticles);
        }

        const matrix44& viewer = this->refGfxServer->GetTransform(nGfxServer2::InvView);
        vector3 emitterViewer = viewer.pos_component() - this->matrix.pos_component();
        float distance = emitterViewer.len();
        if (distance < this->activityDistance)
        {
            #ifdef __NEBULA_STATS__
            particleServer->numActiveEmitters->SetI(particleServer->numActiveEmitters->GetI() + 1);
            #endif
            if ((curTime - this->startTime) < this->emissionDuration) // check if endTime is reached
            {
                float *emitterVertices = this->refEmitterMesh->LockVertices();
                int vertexWidth = this->refEmitterMesh->GetVertexWidth();
                ushort* srcIndices = this->refEmitterMesh->LockIndices();

                int curEmissionCount = (int) (this->curves[EmissionFrequency].GetValue(relAge) * (float) diffTime);
                if (0 != curEmissionCount)
                {
                    this->lastEmission = curTime;
                    int curEmitted = 0;
                    int curIndex = 0;
                    int curVertex = 0;

                    const nMeshGroup& meshGroup = this->refEmitterMesh->GetGroup(this->meshGroupIndex);
                    int firstIndex  = meshGroup.GetFirstIndex();
                    int lastIndex = firstIndex + meshGroup.GetNumIndices() - 1;

                    while ((curEmitted < curEmissionCount) && (!this->particleBuffer.IsFull()))
                    {
                        nParticle* particle = particleServer->GiveFreeParticle();
                        if (0 == particle)
                        {
                            break;
                        }

                        nParticle** bufferElement = this->particleBuffer.Add();
                        *bufferElement = particle;

                        curVertex = (particleServer->PseudoRandomInt(this->randomKey++) % (1 + lastIndex - firstIndex)) + firstIndex;
                        curIndex = srcIndices[curVertex] * vertexWidth;

                        vector3 position = this->matrix * vector3(emitterVertices[curIndex+0],
                            emitterVertices[curIndex+1], emitterVertices[curIndex+2]);

                        matrix33 m33 = matrix33(this->matrix.M11, this->matrix.M12, this->matrix.M13, 
                            this->matrix.M21, this->matrix.M22, this->matrix.M23, 
                            this->matrix.M31, this->matrix.M32, this->matrix.M33);

                        vector3 normal = m33 * vector3(emitterVertices[curIndex+3], 
                            emitterVertices[curIndex+4], emitterVertices[curIndex+5]);

                        // find orthogonal vectors to spread normal vector
                        vector3 ortho1, ortho2;
                        ortho1 = normal.findortho();
                        ortho1.norm();
                        ortho2 = normal * ortho1;   // cross product
                        ortho2.norm();
                        float ortho1Angle = particleServer->PseudoRandomFloat(this->randomKey++);
                        float ortho2Angle = particleServer->PseudoRandomFloat(this->randomKey++);
                        normal.rotate(ortho1, n_deg2rad(ortho1Angle * this->spreadAngle));
                        normal.rotate(ortho2, n_deg2rad(ortho2Angle * this->spreadAngle));

                        nTime birthTime = curTime + ((particleServer->PseudoRandomFloat(this->randomKey++) + 1.0f) / 2.0f * this->birthDelay);
                        float startVelocity = this->curves[ParticleStartVelocity].GetValue(relAge);
                        float startRotation = particleServer->PseudoRandomFloat(this->randomKey++) * this->startRotation;
                        particle->Initialize(this, position, 
                            normal * startVelocity, 
                            birthTime, 
                            this->curves[ParticleLifeTime].GetValue(relAge),
                            startRotation);

                        curEmitted++;
                    }
                    if (this->particleBuffer.IsFull())
                    {
                        n_printf("nParticleEmitter::Trigger: particle ring buffer full!\n");
                    }
                }
                this->refEmitterMesh->UnlockVertices();
                this->refEmitterMesh->UnlockIndices();
            }
            else
            {
                if (this->loop) this->startTime = curTime;
            }
        }
        else
        {   // not active
            this->lastEmission = curTime;
        }
    }
    else
    {   // not ready to emit
        this->lastEmission = curTime;
    }
}

//------------------------------------------------------------------------------
/**
*/
void nParticleEmitter::Render()
{
    nGfxServer2* gfxServer = this->refGfxServer.get();
    #ifdef __NEBULA_STATS__
    nParticleServer*    particleServer = this->refParticleServer.get();
    int numDrawnParticles  = particleServer->numDrawnParticles->GetI();
    int numDrawnPrimitives = particleServer->numDrawnPrimitives->GetI();
    #endif

    if (!this->dynMesh.IsValid())
    {
        this->dynMesh.Initialize(nGfxServer2::TriangleList,
            nMesh2::Coord  | nMesh2::Uv0 | nMesh2::Uv1 | nMesh2::Uv2 |
            nMesh2::Color, nMesh2::WriteOnly | nMesh2::NeedsVertexShader, false);
        n_assert(this->dynMesh.IsValid());
    }

    float* dstVertices;
    int    maxVertices;

    vector2 spriteCorners[4] = {vector2(-1.0, -1.0),
                                vector2(-1.0,  1.0),
                                vector2(1.0,   1.0),
                                vector2(1.0,  -1.0)};

    nParticle** curParticle;
    if (this->renderOldestFirst)
    {
        curParticle = this->particleBuffer.GetTail();
    }
    else
    {
        curParticle = this->particleBuffer.GetHead();
    }
    int curIndex  = 0;
    int curVertex = 0;

    this->dynMesh.Begin(dstVertices, maxVertices);

    while (0 != curParticle)
    {
        if (nParticle::Living == (*curParticle)->GetState())
        {
            curVertex += 6;

            float   relParticleAge  = (*curParticle)->GetRelAge();
            if (relParticleAge > 1.0)
                relParticleAge = 1.0f;
            else if (relParticleAge <= 0.0)
                relParticleAge = 1e-6f;
            vector3 curPosition     = (*curParticle)->GetCurPosition();
            float   curRotation     = (*curParticle)->GetCurRotation();
            float   curScale        = this->GetParticleScale(relParticleAge);
            vector3 curRGB          = this->GetParticleRGB(relParticleAge);
            float   curAlpha        = this->GetParticleAlpha(relParticleAge);

            // point 1
            dstVertices[curIndex++] = curPosition.x;
            dstVertices[curIndex++] = curPosition.y;
            dstVertices[curIndex++] = curPosition.z;

            dstVertices[curIndex++] = 0.0f;
            dstVertices[curIndex++] = 1.0f;

            dstVertices[curIndex++] = spriteCorners[0].x;
            dstVertices[curIndex++] = spriteCorners[0].y;

            dstVertices[curIndex++] = curRotation;
            dstVertices[curIndex++] = curScale;

            dstVertices[curIndex++] = curRGB.x;
            dstVertices[curIndex++] = curRGB.y;
            dstVertices[curIndex++] = curRGB.z;
            dstVertices[curIndex++] = curAlpha;

            // point 2
            dstVertices[curIndex++] = curPosition.x;
            dstVertices[curIndex++] = curPosition.y;
            dstVertices[curIndex++] = curPosition.z;

            dstVertices[curIndex++] = 0.0f;
            dstVertices[curIndex++] = 0.0f;

            dstVertices[curIndex++] = spriteCorners[1].x;
            dstVertices[curIndex++] = spriteCorners[1].y;

            dstVertices[curIndex++] = curRotation;
            dstVertices[curIndex++] = curScale;

            dstVertices[curIndex++] = curRGB.x;
            dstVertices[curIndex++] = curRGB.y;
            dstVertices[curIndex++] = curRGB.z;
            dstVertices[curIndex++] = curAlpha;

            // point 3
            dstVertices[curIndex++] = curPosition.x;
            dstVertices[curIndex++] = curPosition.y;
            dstVertices[curIndex++] = curPosition.z;

            dstVertices[curIndex++] = 1.0f;
            dstVertices[curIndex++] = 0.0f;

            dstVertices[curIndex++] = spriteCorners[2].x;
            dstVertices[curIndex++] = spriteCorners[2].y;

            dstVertices[curIndex++] = curRotation;
            dstVertices[curIndex++] = curScale;

            dstVertices[curIndex++] = curRGB.x;
            dstVertices[curIndex++] = curRGB.y;
            dstVertices[curIndex++] = curRGB.z;
            dstVertices[curIndex++] = curAlpha;

            // point 4
            dstVertices[curIndex++] = curPosition.x;
            dstVertices[curIndex++] = curPosition.y;
            dstVertices[curIndex++] = curPosition.z;

            dstVertices[curIndex++] = 0.0f;
            dstVertices[curIndex++] = 1.0f;

            dstVertices[curIndex++] = spriteCorners[0].x;
            dstVertices[curIndex++] = spriteCorners[0].y;

            dstVertices[curIndex++] = curRotation;
            dstVertices[curIndex++] = curScale;

            dstVertices[curIndex++] = curRGB.x;
            dstVertices[curIndex++] = curRGB.y;
            dstVertices[curIndex++] = curRGB.z;
            dstVertices[curIndex++] = curAlpha;

            // point 5
            dstVertices[curIndex++] = curPosition.x;
            dstVertices[curIndex++] = curPosition.y;
            dstVertices[curIndex++] = curPosition.z;

            dstVertices[curIndex++] = 1.0f;
            dstVertices[curIndex++] = 0.0f;

            dstVertices[curIndex++] = spriteCorners[2].x;
            dstVertices[curIndex++] = spriteCorners[2].y;
 
            dstVertices[curIndex++] = curRotation;
            dstVertices[curIndex++] = curScale;

            dstVertices[curIndex++] = curRGB.x;
            dstVertices[curIndex++] = curRGB.y;
            dstVertices[curIndex++] = curRGB.z;
            dstVertices[curIndex++] = curAlpha;

            // point 6
            dstVertices[curIndex++] = curPosition.x;
            dstVertices[curIndex++] = curPosition.y;
            dstVertices[curIndex++] = curPosition.z;

            dstVertices[curIndex++] = 1.0f;
            dstVertices[curIndex++] = 1.0f;

            dstVertices[curIndex++] = spriteCorners[3].x;
            dstVertices[curIndex++] = spriteCorners[3].y;

            dstVertices[curIndex++] = curRotation;
            dstVertices[curIndex++] = curScale;

            dstVertices[curIndex++] = curRGB.x;
            dstVertices[curIndex++] = curRGB.y;
            dstVertices[curIndex++] = curRGB.z;
            dstVertices[curIndex++] = curAlpha;


            if (curVertex > maxVertices-6)
            {
                this->dynMesh.Swap(curVertex, dstVertices);
                #ifdef __NEBULA_STATS__
                numDrawnPrimitives += curVertex/3;
                #endif
                curIndex  = 0;
                curVertex = 0;
            }
        }
        #ifdef __NEBULA_STATS__
        numDrawnParticles++;
        #endif
        if (this->renderOldestFirst)
        {
        curParticle = this->particleBuffer.GetNext(curParticle);
        }
        else
        {
            curParticle = this->particleBuffer.GetPrev(curParticle);
        }
    }

    // Draw
    this->dynMesh.End(curVertex);
    #ifdef __NEBULA_STATS__
    particleServer->numDrawnParticles->SetI(numDrawnParticles);
    particleServer->numDrawnPrimitives->SetI(numDrawnPrimitives + curVertex/3);
    #endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticleEmitter::AreResourcesValid()
{
    return this->refEmitterMesh.isvalid();
}
