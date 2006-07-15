#ifndef N_SHADOWCASTER2_H
#define N_SHADOWCASTER2_H
//------------------------------------------------------------------------------
/**
    @class nShadowCaster2
    @ingroup Shadow2

    Base class for shadow casters.

    (C) 2005 Radon Labs GmbH
*/
#include "resource/nresource.h"
#include "util/nfixedarray.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
class nShadowCaster2 : public nResource
{
public:
    /// shadow caster types
    enum Type
    {
        Static,
        Skinned,

        NumTypes,
        InvalidType,
    };

    /// constructor
    nShadowCaster2();
    /// destructor
    virtual ~nShadowCaster2();
    /// setup the shadow volume for rendering
    virtual void SetupShadowVolume(const nLight& light, const matrix44& invModelLightMatrix);
    /// render the shadow volume
    virtual void RenderShadowVolume();
    /// set the mesh group for this shadow caster
    void SetMeshGroupIndex(int i);
    /// get the mesh group index
    int GetMeshGroupIndex() const;

protected:
    /// allocate shadow index buffer and face flags array from mesh
    bool AllocateBuffers(nMesh2* mesh);
    /// free allocated buffers
    void ReleaseBuffers();
    /// update the face normals and midpoints from a mesh object
    void UpdateFaceNormalsAndMidpoints(nMesh2* mesh);
    /// update the face lit/unlit flags in the faces array
    void UpdateFaceLitFlags(const nLight& light, const matrix44& invModelLight);
    /// begin writing shadow volume indices
    void BeginWriteIndices(nMesh2* mesh);
    /// write triangle indices for shadow volume sides into index buffer
    void WriteSideIndices();
    /// write triangle indices for dark or light cap into index buffer
    void WriteCapIndices(bool lit);
    /// finish writing shadow volume indices
    void EndWriteIndices();
    /// returns the number of valid indices after EndWriteIndices()
    int GetNumDrawIndices() const;
    /// return true if lighting has changed since last evaluation (for lazy updates)
    bool LightingChanged(const nLight& light, const matrix44& invModelLight);

    struct Face
    {
        vector3 normal;
        vector3 point;
        bool lit;
    };
    nRef<nMesh2> refIndexBuffer;    // contains dynamic index buffer for dark cap, light cap and shadow volume
    nFixedArray<Face> faces;        // lit/unlit face status
    int meshGroupIndex;

private:
    friend class nShadowServer2;

    /// write a extruded quad to the index buffer
    void WriteQuad(ushort vIndex0, ushort vIndex1);

    // lazy optimization stuff
    nLight::Type prevLightType;
    vector3 prevLightPosOrDir;

    // temporary draw data
    ushort* indexBufferStart;
    ushort* indexBufferCurrent;
    int drawNumIndices;
    nMesh2* srcMesh;
    ushort* srcIndices;
    nMesh2::Edge* srcEdges;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowCaster2::GetNumDrawIndices() const
{
    return this->drawNumIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowCaster2::SetMeshGroupIndex(int i)
{
    this->meshGroupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowCaster2::GetMeshGroupIndex() const
{
    return this->meshGroupIndex;
}

//------------------------------------------------------------------------------
#endif
