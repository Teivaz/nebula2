#ifndef N_SHADOWCASTER_H
#define N_SHADOWCASTER_H
//------------------------------------------------------------------------------
/**
    @class nShadowCaster
    @ingroup Shadow

    A generic shadow caster class in the stencil buffer shadow subsystem.

    (C) 2004 RadonLabs GmbH
*/
#include "shadow/nshadowserver.h"
#include "resource/nresource.h"

#include "gfx2/nmesh2.h"
#include "gfx2/nmesharray.h"
#include "util/narray.h"

#include "mathlib/vector.h"
#include "mathlib/nmath.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
class nShadowCaster : public nResource
{
public:
    /// constructor
    nShadowCaster();
    /// destructor
    virtual ~nShadowCaster();
    
    /// setup shadow
    virtual bool ApplyShadow();
    /// render shadow
    virtual void RenderShadow(const matrix44& modelMatrix, int groupIndex);

    /// compute shadow volume
    void ComputeSilhouetteEdges(const nLight::Type lightType, const vector3& modelSpaceLight, int groupIndex);
    /// write the transformed and extruded verticies for the sides of the shadow volume as worldspace coord into the dynamic mesh
	void DrawSides(nShadowServer::DrawType type, const nLight::Type lightType, const vector3& worldLightPosition, const matrix44& modelMatrix);
    void DrawDarkCap(nShadowServer::DrawType type, const nLight::Type lightType, int groupIndex, const vector3& worldLightPosition, const matrix44& modelMatrix);
    void DrawLightCap(nShadowServer::DrawType type, int groupIndex, const vector3& worldLightPosition, const matrix44& modelMatrix);

protected:
    /// perform actual resource loading
    virtual bool LoadResource();
    /// perform actual resource unloading
    virtual void UnloadResource();
    
    /// implement in subclass: get the face normals array
    virtual vector3* GetFaceNormals() const;
    /// implement in subclass: get the coordiantes array
    virtual vector3* GetCoords() const;
    /// get num coordiantes
    virtual int GetNumCoords() const;

    /// perform data load from source mesh
    virtual void LoadShadowData(nMesh2* sourceMesh);
    
    /// load face data from source mesh
    void LoadFaces(nMesh2* sourceMesh);
    /// load edge data from source mesh
    void LoadEdges(nMesh2* sourceMesh);
    /// load group information from source mesh
    void LoadGroups(nMesh2* sourceMesh);

    /// debug: setup the current shadow mesh geometry in the GfxServer - true if there is something to render
    bool DebugSetupGeometry(bool unlitFaces, int groupIndex);
    /// debug: create wireframe model of the edges
    bool DebugSetupEdges(bool openEdges, int groupIndex);
    /// debug: create wireframe model of the shilouette edges
    bool DebugSetupSilhouetteEdges();

    struct Face
    {
        bool lightFacing;
        int lastUpdate;
        int lightCapCycle;
        int index[3];
    };

    Face* faces;
    int numFaces;
    nMesh2::Edge* edges;
    int numEdges;

    /// indicies of the current silhouette
    nArray<ushort> silhouetteIndicies;
    int numSilhouetteIndicies;

    nArray<nMeshGroup> meshGroups;
    
    vector3 lightPosition;
    int lastGroupIndex;
    bool dirty; // set true to force a update

    nRef<nMesh2> dbgMesh;

    static const float shadowOffset;
};

//------------------------------------------------------------------------------
/**
*/
inline
vector3*
nShadowCaster::GetCoords() const
{
    n_error("Pure Virual function 'nShadowCaster::GetCoords()' called!\n");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3*
nShadowCaster::GetFaceNormals() const
{
    n_error("Pure Virual function 'nShadowCaster::GetFaceNormals()' called!\n");
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShadowCaster::GetNumCoords() const
{
    n_error("Pure Virual function 'nShadowCaster::GetNumCoords()' called!\n");
    return 0;
}

//------------------------------------------------------------------------------
#endif

