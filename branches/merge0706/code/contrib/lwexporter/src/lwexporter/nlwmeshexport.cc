//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwmeshexport.h"
#include "lwexporter/nlwexportersettings.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "lwwrapper/nlwlayoutmonitor.h"
#include "lwwrapper/nlwglobals.h"
#include "lwwrapper/nlwmeshinfo.h"
#include "util/nhashmap.h"
#include "util/nkeyarray.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "tools/nskinpartitioner.h"
#include "lwexporter/nlwexportnodetype.h"

//----------------------------------------------------------------------------
/**
*/
nLWMeshExport::nLWMeshExport(const nString& meshName,
                             nLWObjectExportSettings* objSettings, 
                             nLWLayoutMonitor* monitor) :
    meshName(meshName),
    objSettings(objSettings),
    monitor(monitor),
    curMeshInfo(0),
    surfaceHashMap(0),
    meshPolygons(0, 0),
    polyScanFoundNonTriPoly(false),
    numMeshGroups(0),
    skinMeshBuilder(0),
    groupFragmentsArray(0, 0),
    fragmentJointsArray(0, 0)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
nLWMeshExport::~nLWMeshExport()
{
    for (int i = 0; i < this->fragmentJointsArray.Size(); i++)
    {
        if (this->fragmentJointsArray[i])
        {
            n_delete(this->fragmentJointsArray[i]);
            this->fragmentJointsArray[i] = 0;
        }
    }

    for (int i = 0; i < this->groupFragmentsArray.Size(); i++)
    {
        if (this->groupFragmentsArray[i])
        {
            n_delete(this->groupFragmentsArray[i]);
            this->groupFragmentsArray[i] = 0;
        }
    }

    if (this->skinMeshBuilder)
    {
        n_delete(this->skinMeshBuilder);
        this->skinMeshBuilder = 0;
    }
}

//----------------------------------------------------------------------------
/**
    @return true if the mesh was exported successfully, false otherwise.
*/
bool 
nLWMeshExport::Run()
{
    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (!exporterSettings)
        return false;

    nString monitorMsg;

    // get the mesh from Lightwave into our mesh builder
    this->ExtractGeometry();

    bool noGeometry = false;
    if (this->NeedUV())
    {
        noGeometry = (0 == this->numMeshGroups);
    }
    else
    {
        noGeometry = (0 == this->meshBuilder.GetNumTriangles());
    }

    if (noGeometry)
    {
        monitorMsg = "[ERROR] No geometry to export.\n";
        if (this->monitor)
        {
            this->monitor->Step(0, monitorMsg);
        }
        n_printf(monitorMsg.Get());
        return false;
    }

    // FIXME: would be nice to be able to generate normals without having to also
    // generate bi-normals and tangents and vice versa.
    if (this->NeedNormals())
    {
        const nMeshBuilder::Vertex& firstVertex = this->meshBuilder.GetVertexAt(0);
        if (!firstVertex.HasComponent(nMeshBuilder::Vertex::UV0))
        {
            monitorMsg = "[ERROR] Missing UV0 - required to build tangents.\n";
            if (this->monitor)
            {
                this->monitor->Step(0, monitorMsg);
            }
            n_printf(monitorMsg.Get());
            return false;
        }

        // build normals, tangents and binormals
        this->meshBuilder.BuildTriangleNormals();

        if (!firstVertex.HasComponent(nMeshBuilder::Vertex::NORMAL))
        {
            // build vertex normals by averaging triangle normals
            this->meshBuilder.BuildVertexNormals();
        }

        // build vertex tangents by averaging triangle tangents
        this->meshBuilder.BuildVertexTangents(true);
    }

    // check for geometry errors
    nArray<nString> geometryErrors = this->meshBuilder.CheckForGeometryError();
    if (!geometryErrors.Empty())
    {
        if (this->monitor)
        {
            this->monitor->Step(0, "[ERROR] Geometry errors found, check log file.\n");
        }

        for (int i = 0; i < geometryErrors.Size(); i++)
        {
            monitorMsg = "[ERROR] ";
            monitorMsg += geometryErrors[i];
            n_printf(monitorMsg.Get());
        }
    }

    // scale the geometry
    float geomScale = exporterSettings->GetGeometryScale();
    if (geomScale != 1.0f)
    {
        matrix44 scaleMat;
        scaleMat.scale(vector3(geomScale, geomScale, geomScale));
        this->meshBuilder.Transform(scaleMat);
    }

    // partition the mesh if necessary (for skinning)
    this->PartitionMesh();
    
    this->ConstructMeshFileName();
    
    return this->Save();
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWMeshExport::NeedUV() const
{
    nLWExporterSettings* exps = nLWExporterSettings::Instance();
    n_assert(exps);
    if (exps)
    {
        nLWExportNodeType* exportType = 0;
        exportType = exps->GetExportNodeType(this->objSettings->GetExportNodeType());
        int vertexMask = exportType->GetVertexComponentMask();
        return ((vertexMask & nMeshBuilder::Vertex::UV0) != 0) ||
               ((vertexMask & nMeshBuilder::Vertex::UV1) != 0) ||
               ((vertexMask & nMeshBuilder::Vertex::UV2) != 0) ||
               ((vertexMask & nMeshBuilder::Vertex::UV3) != 0);
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWMeshExport::NeedNormals() const
{
    nLWExporterSettings* exps = nLWExporterSettings::Instance();
    n_assert(exps);
    if (exps)
    {
        nLWExportNodeType* exportType = 0;
        exportType = exps->GetExportNodeType(this->objSettings->GetExportNodeType());
        int vertexMask = exportType->GetVertexComponentMask();
        return (vertexMask & nMeshBuilder::Vertex::NORMAL) != 0;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
*/
bool 
nLWMeshExport::Save()
{
    nKernelServer* ks = nKernelServer::Instance();
    n_assert(ks);
    if (!ks)
    {
        return false;
    }

    // if the mesh was partitioned we need to save the partitioned mesh 
    // builder otherwise just save the unpartitioned one
    nMeshBuilder* meshBuilderToSave = &this->meshBuilder;
    if (this->skinMeshBuilder)
    {
        meshBuilderToSave = this->skinMeshBuilder;
    }

    if (meshBuilderToSave->Save(ks->GetFileServer(), this->meshFileName.Get()))
    {
        nString monitorMsg;
        monitorMsg.Format("[INFO] Saved %s\n", this->meshFileName.Get());
        if (this->monitor)
        {
            this->monitor->Step(0, monitorMsg);
        }
        n_printf(monitorMsg.Get());
    }
    else
    {
        nString monitorMsg;
        monitorMsg.Format("[ERROR] Failed to save %s\n", this->meshFileName.Get());
        if (this->monitor)
        {
            this->monitor->Step(0, monitorMsg);
        }
        n_printf(monitorMsg.Get());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderExportSettings* 
nLWMeshExport::GetShaderForGroup(int groupIdx) const
{
    nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
    n_assert(registry);
    if (registry)
    {
        return registry->Get(this->usedSurfaceArray[groupIdx]);
    }
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWMeshExport::ConstructMeshFileName()
{
    nLWGlobals::ObjectInfo objectInfo;
    n_assert(objectInfo.IsValid());
    if (!objectInfo.IsValid())
        return;

    n_assert(this->objSettings);
    if (!this->objSettings)
        return;

    nLWExporterSettings* exporterSettings = nLWExporterSettings::Instance();
    n_assert(exporterSettings);
    if (!exporterSettings)
        return;

    // the absolute path to the object file
    nString objFile = objectInfo.Get()->filename(this->objSettings->GetItemId());
    nString objFileDir = objFile.ExtractToLastSlash();
    objFileDir.ToLower();
    objFileDir.ConvertBackslashes();
    objFileDir.StripTrailingSlash();
    objFileDir += "/";
    
    // the absolute path to the current Lightwave Objects directory.
    nLWGlobals::DirInfoFunc dirInfo;
    nString lwObjectsDir = dirInfo.GetObjectsDir();
    lwObjectsDir.ToLower();
    lwObjectsDir.ConvertBackslashes();
    lwObjectsDir.StripTrailingSlash();
    lwObjectsDir += "/";

    this->meshFileName = "meshes:";
    // figure out what the category dir is and append it to the filename
    if (objFileDir.Length() > lwObjectsDir.Length())
    {
        if (strncmp(objFileDir.Get(), lwObjectsDir.Get(), lwObjectsDir.Length()) == 0)
        {
            this->meshFileName += &(objFileDir.Get()[lwObjectsDir.Length()]);
        }
        else
        {
            nString msg;
            msg.Format("[ERROR] Object file %s was not found in the current "
                       "Lightwave Content directory.\n", objFile.Get());
            if (this->monitor)
            {
                this->monitor->Step(0, msg);
            }
            n_printf(msg.Get());
        }
    }
    this->meshFileName += this->meshName;
    if (exporterSettings->GetOutputBinaryMesh())
    {
        this->meshFileName += ".nvx2";
    }
    else
    {
        this->meshFileName += ".n3d2";
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::ExtractAllMeshSurfaces(nArray<MeshSurface>& surfaceArray)
{
    n_assert(this->surfaceHashMap);
    if (!this->surfaceHashMap)
        return;

    nLWGlobals::ObjectInfo objectInfo;
    n_assert(objectInfo.IsValid());
    if (!objectInfo.IsValid())
    {
        n_printf("[ERROR] Invalid nLWGlobals::ObjectInfo in %s.\n", __FUNCTION__);
        return;
    }

    nLWGlobals::SurfaceFuncs surfaceFuncs;
    n_assert(surfaceFuncs.IsValid());
    if (!surfaceFuncs.IsValid())
    {
        n_printf("[ERROR] Invalid nLWGlobals::ObjectInfo in %s.\n", __FUNCTION__);
        return;
    }

    nString objName = objectInfo.Get()->filename(this->objSettings->GetItemId());
    LWSurfaceID* surfIterator = surfaceFuncs.Get()->byObject(objName.Get());
    if (surfIterator)
    {
        MeshSurface meshSurf;
        while (*surfIterator)
        {
            meshSurf.id = *surfIterator;
            meshSurf.name = surfaceFuncs.Get()->name(meshSurf.id);
            meshSurf.used = false; // will be set after the polygon scan
            n_assert(!meshSurf.name.IsEmpty());
            if (!meshSurf.name.IsEmpty())
            {
                surfaceArray.PushBack(meshSurf);
                this->surfaceHashMap->AddString(meshSurf.name.Get());
            }
            ++surfIterator;
        }
    }
}

//----------------------------------------------------------------------------
/**
    @brief Called for each polygon by nLWMeshInfo during polygon scanning.
    @return Non-zero value to stop the scan, zero otherwise.
*/
XCALL_(int)
nLWMeshExport::PolygonScan(void* boxedObject, LWPolID polId)
{
    const int continueScan = 0;
    const int stopScan = 1;

    n_assert(boxedObject);
    if (!boxedObject)
        return stopScan;

    nLWMeshExport* self = (nLWMeshExport*)boxedObject;

    // silently ignore non-face polygons
    if (self->curMeshInfo->GetPolygonType(polId) != LWPOLTYPE_FACE)
        return continueScan;

    // silently ignore faces that aren't triangles?
    if (self->curMeshInfo->GetPolygonSize(polId) != 3)
    {
        self->polyScanFoundNonTriPoly = true;
        return continueScan; // TODO: should we stop the scan?
    }

    MeshPolygon poly;
    poly.id = polId;
    poly.surfaceKey = (*self->surfaceHashMap)[self->curMeshInfo->GetSurfaceName(polId)];
    self->meshPolygons.PushBack(poly);

    return continueScan;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::ExtractMeshTriangles()
{
    n_assert(this->curMeshInfo);
    if (!this->curMeshInfo)
        return;

    nLWGlobals::ObjectInfo objectInfo;
    n_assert(objectInfo.IsValid());
    if (!objectInfo.IsValid())
    {
        n_printf("[ERROR] Invalid nLWGlobals::ObjectInfo in %s.\n", __FUNCTION__);
        return;
    }

    LWItemID curItemId = this->objSettings->GetItemId();
    int maxPolygons = objectInfo.Get()->numPolygons(curItemId);
    if (maxPolygons > 0)
    {    
        // allocate storage to be used during the polygon scan,
        // some polygons may be discarded during the scan so the final 
        // number of polygons may be less than expected
        this->meshPolygons.Reallocate(maxPolygons, 0);
        // scan all polygons
        this->polyScanFoundNonTriPoly = false;
        this->curMeshInfo->ScanPolygons(nLWMeshExport::PolygonScan, this);

        if (this->polyScanFoundNonTriPoly)
        {
            nString msg = "[ERROR] Found polygons that aren't triangles.\n";
            if (this->monitor)
            {
                this->monitor->Step(0, msg);
            }
            n_printf(msg.Get());
        }
    }
    else
    {
        nString msg = "[ERROR] No polygons found.\n";
        if (this->monitor)
        {
            this->monitor->Step(0, msg);
        }
        n_printf(msg.Get());
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::CollectUsedSurfaces(nArray<MeshSurface>& allSurfaces)
{
    n_assert(this->surfaceHashMap);
    if (!this->surfaceHashMap)
        return;

    for (int surfIdx = 0; surfIdx < allSurfaces.Size(); surfIdx++)
    {
        ushort meshSurfKey = (*this->surfaceHashMap)[allSurfaces[surfIdx].name.Get()];
        for (int polyIdx = 0; polyIdx < this->meshPolygons.Size(); polyIdx++)
        {
            if (meshSurfKey == this->meshPolygons[polyIdx].surfaceKey)
            {
                allSurfaces[surfIdx].used = true;
                break; // look for the next used surface
            }
        }
        // copy the used surface id to the surface id array passed in
        if (allSurfaces[surfIdx].used)
        {
            this->usedSurfaceArray.PushBack(allSurfaces[surfIdx].id);        
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::CollectTriangles()
{
    n_assert(this->curMeshInfo);
    if (!this->curMeshInfo)
        return;

    // store the vertex coords
    for (int polyIdx = 0; polyIdx < this->meshPolygons.Size(); polyIdx++)
    {
        LWPolID polyId = this->meshPolygons[polyIdx].id;
        for (int vIdx = 0; vIdx < 3; vIdx++)
        {
            LWPntID pointId = this->curMeshInfo->GetPolygonVertex(polyId, vIdx);
            LWFVector pointPos;
            this->curMeshInfo->GetPointBasePos(pointId, pointPos);
            nMeshBuilder::Vertex meshVertex;
            meshVertex.SetCoord(vector3(pointPos[0], pointPos[1], pointPos[2]));
            this->meshBuilder.AddVertex(meshVertex);
        }
        nMeshBuilder::Triangle meshTri;
        meshTri.SetVertexIndices(polyIdx * 3, polyIdx * 3 + 1, polyIdx * 3 + 2);
        meshTri.SetUsageFlags(nMesh2::WriteOnce);
        this->meshBuilder.AddTriangle(meshTri);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::GroupTrianglesBySurface(nArray<MeshSurface>& allSurfaces)
{
    n_assert(this->surfaceHashMap);
    if (!this->surfaceHashMap)
        return;

    // assign group ids (each surface gets it's own group)
    // assumption: if the mesh has surfaces every polygon belongs to a surface
    for (int surfIdx = 0; surfIdx < allSurfaces.Size(); surfIdx++)
    {
        if (!allSurfaces[surfIdx].used)
            continue; // skip the surface

        ushort meshSurfKey = (*this->surfaceHashMap)[allSurfaces[surfIdx].name.Get()];
        for (int polyIdx = 0; polyIdx < this->meshPolygons.Size(); polyIdx++)
        {
            if (meshSurfKey == this->meshPolygons[polyIdx].surfaceKey)
            {
                this->meshBuilder.GetTriangleAt(polyIdx).SetGroupId(this->numMeshGroups);
            }
        }

        this->numMeshGroups++;
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::CollectUV()
{
    n_assert(this->curMeshInfo);
    if (!this->curMeshInfo)
        return;

    nLWGlobals::SurfaceFuncs surfaceFuncs;
    n_assert(surfaceFuncs.IsValid());
    if (!surfaceFuncs.IsValid())
    {
        n_printf("[ERROR] Invalid nLWGlobals::SurfaceFuncs in %s.\n", __FUNCTION__);
        return;
    }

    nLWGlobals::TextureFuncs textureFuncs;
    n_assert(textureFuncs.IsValid());
    if (!textureFuncs.IsValid())
    {
        n_printf("[ERROR] Invalid nLWGlobals::TextureFuncs in %s.\n", __FUNCTION__);
        return;
    }

    for (int surfIdx = 0; surfIdx < this->usedSurfaceArray.Size(); surfIdx++)
    {
        LWTextureID txtId = surfaceFuncs.Get()->getTex(this->usedSurfaceArray[surfIdx], SURF_COLR);
        if (!txtId)
            continue; // skip the surface TODO: log an error

        // handle up to 4 UV layers
        int curUVLayer = 0;
        LWTLayerID txtLayerId = textureFuncs.Get()->firstLayer(txtId);
        if (!txtLayerId)
            continue; // skip the surface TODO: log an error?

        bool firstTxtLayer = true;
        while (curUVLayer < 4)
        {
            if (firstTxtLayer)
            {
                firstTxtLayer = false;
            }
            else
            {
                txtLayerId = textureFuncs.Get()->nextLayer(txtId, txtLayerId);
                if (!txtLayerId)
                    break; // no more texture layers
            }

            int layerType = textureFuncs.Get()->layerType(txtLayerId);
            if (TLT_IMAGE != layerType)
                continue; // skip the texture layer

            int proj = 0;
            if (!textureFuncs.Get()->getParam(txtLayerId, TXTAG_PROJ, &proj))
                continue; // skip the texture layer

            if (TXPRJ_UVMAP != proj)
                continue; // skip the texture layer

            void* vmap;
            if (!textureFuncs.Get()->getParam(txtLayerId, TXTAG_VMAP, &vmap))
                continue; // skip the texture layer

            // select the vmap (dimension should be 2)
            int dim = this->curMeshInfo->pntVSelect(vmap);
            n_assert(2 == dim);
            if (2 != dim)
                continue; // skip the texture layer

            bool validUVs = true;
            float uv[2];
            for (int polyIdx = 0; polyIdx < this->meshPolygons.Size(); polyIdx++)
            {
                const MeshPolygon& poly = this->meshPolygons[polyIdx];
                for (int vIdx = 0; vIdx < 3; vIdx++)
                {
                    LWPntID pointId = this->curMeshInfo->GetPolygonVertex(poly.id, vIdx);
                    if (!this->curMeshInfo->pntVPGet(pointId, poly.id, uv))
                    {
                        if (!this->curMeshInfo->pntVGet(pointId, uv))
                        {
                            uv[0] = 0.0f;
                            uv[1] = 0.0f;
                            validUVs = false;
                        }
                    }
                    nMeshBuilder::Vertex& v = this->meshBuilder.GetVertexAt(polyIdx * 3 + vIdx);
                    // the v coord needs to be flipped for Nebula 2
                    v.SetUv(curUVLayer, vector2(uv[0], 1.0f - uv[1]));
                }
            }
            if (!validUVs)
            {
                nString msg;
                msg.Format("[ERROR] Found vertices with no UV%d.\n", curUVLayer);
                if (this->monitor)
                {
                    this->monitor->Step(0, msg);
                }
                n_printf(msg.Get());
            }
            ++curUVLayer;
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::CollectWeightMaps(LWItemID boneId, 
                                 nArray<nString>& weightMapNames,
                                 LWItemInfo* itemInfo,
                                 LWBoneInfo* boneInfo)
{
    nString weightMapName(boneInfo->weightMap(boneId));
    if (weightMapName.IsEmpty())
    {
        nString msg;
        msg.Format("[WARNING] Bone %s doesn't have a weight-map!\n", 
                   itemInfo->name(boneId));
        if (this->monitor)
        {
            this->monitor->Step(0, msg);
        }
        n_printf(msg.Get());
    }
    else
    {
        weightMapNames.PushBack(weightMapName);
    }

    // recurse for child bones
    LWItemID childId = itemInfo->firstChild(boneId);
    while (childId != LWITEM_NULL)
    {
        if (itemInfo->type(childId) == LWI_BONE)
        {
            this->CollectWeightMaps(childId, weightMapNames, itemInfo, 
                                    boneInfo);
        }
        childId = itemInfo->nextChild(boneId, childId);
    }
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWMeshExport::MapWeightMapsToJointIndices(LWItemID boneId, 
                                           const nArray<nString>& weightMapNames, 
                                           nKeyArray<int>& weightMapIndexToJointIndex,
                                           const nLWBoneJointMap& boneJointMap,
                                           LWItemInfo* itemInfo,
                                           LWBoneInfo* boneInfo)
{
    nString weightMapName(boneInfo->weightMap(boneId));
    // the bone may not have a weight map (assumption is that in that case it 
    // doesn't actually influence any vertices, just other joints)
    if (!weightMapName.IsEmpty())
    {
        for (int i = 0; i < weightMapNames.Size(); i++)
        {
            if (weightMapName == weightMapNames[i])
            {
                weightMapIndexToJointIndex.Add(i, boneJointMap.GetJointIndex(boneId));
                break;
            }
        }
    }

    // recurse for child bones
    LWItemID childId = itemInfo->firstChild(boneId);
    while (childId != LWITEM_NULL)
    {
        if (itemInfo->type(childId) == LWI_BONE)
        {
            this->MapWeightMapsToJointIndices(childId, weightMapNames, 
                                              weightMapIndexToJointIndex, 
                                              boneJointMap, 
                                              itemInfo, boneInfo);
        }
        childId = itemInfo->nextChild(boneId, childId);
    }
}

//----------------------------------------------------------------------------
/**
*/
bool
nLWMeshExport::MapWeightMapsToPointers(const nArray<nString>& weightMapNames, 
                                       nKeyArray<void*>& weightMapIndexToPtr)
{
    n_assert(this->curMeshInfo);
    if (!this->curMeshInfo)
        return false;

    void* vmap = 0;
    for (int i = 0; i < weightMapNames.Size(); i++)
    {
        vmap = this->curMeshInfo->pntVLookup(LWVMAP_WGHT, weightMapNames[i].Get());
        n_assert(vmap);
        if (!vmap)
        {
            nString msg;
            msg.Format("[ERROR] Failed to find weight map %s\n", 
                       weightMapNames[i].Get());
            if (this->monitor)
            {
                this->monitor->Step(0, msg);
            }
            n_printf(msg.Get());
            return false;
        }
        weightMapIndexToPtr.Add(i, vmap);
    }

    return true;
}

//----------------------------------------------------------------------------
/**
    @brief Check if joint weights should be extracted from Lightwave.
    @return true if joint weights need to be extracted, false otherwise.
*/
bool
nLWMeshExport::NeedJointWeights() const
{
    nLWExporterSettings* exps = nLWExporterSettings::Instance();
    n_assert(exps);
    if (exps)
    {
        nLWExportNodeType* nt = 0;
        nt = exps->GetExportNodeType(this->objSettings->GetExportNodeType());
        return (nt->GetVertexComponentMask() & nMeshBuilder::Vertex::WEIGHTS) != 0;
    }
    return false;
}

//----------------------------------------------------------------------------
/**
    @brief Extract joint weights from Lightwave.
*/
void
nLWMeshExport::CollectJointWeights()
{
    n_assert(this->curMeshInfo);
    if (!this->curMeshInfo)
        return;

    nLWGlobals::BoneInfo boneInfo;
    n_assert(boneInfo.IsValid());
    if (!boneInfo.IsValid())
        return;

    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (!itemInfo.IsValid())
        return;

    nLWGlobals::ObjectInfo objectInfo;
    n_assert(objectInfo.IsValid());
    if (!objectInfo.IsValid())
        return;

    // the bones that deform the object may actually belong to a different
    // object so need to find the rightful owner
    LWItemID bonesOwnerId = objectInfo.Get()->boneSource(this->objSettings->GetItemId());
    LWItemID rootBoneId = itemInfo.Get()->first(LWI_BONE, bonesOwnerId);

    // get a list of all bone weight map names
    nArray<nString> weightMapNames;
    this->CollectWeightMaps(rootBoneId, weightMapNames, 
                            itemInfo.Get(), boneInfo.Get());
    int numWeightMaps = weightMapNames.Size();

    /*
    n_printf("Weightmaps Begins\n");
    for (int i = 0; i < numWeightMaps; i++)
    {
        n_printf("[%d] %s\n", i, weightMapNames[i].Get());
    }
    n_printf("Weightmaps Ends\n");
    */
    
    // create a mapping of bone ids to joint indices
    nLWBoneJointMap boneJointMap;
    boneJointMap.Fill(rootBoneId);

    // create a mapping of weight map indices to joint indices
    nKeyArray<int> weightMapIndexToJointIndex;
    this->MapWeightMapsToJointIndices(rootBoneId, weightMapNames, 
                                      weightMapIndexToJointIndex, 
                                      boneJointMap, itemInfo.Get(), 
                                      boneInfo.Get());

    /*
    n_printf("Weight Map To Joint Mapping Begins\n");
    for (int i = 0; i < weightMapIndexToJointIndex.Size(); i++)
    {
        n_printf("Map: %s    Joint %d\n",
                 weightMapNames[weightMapIndexToJointIndex.GetKeyAt(i)].Get(), 
                 weightMapIndexToJointIndex.GetElementAt(i));
    }
    n_printf("Weight Map To Joint Mapping Ends\n");
    */

    // create a mapping of weight map indices to weight map pointers
    nKeyArray<void*> weightMapIndexToPtr;
    this->MapWeightMapsToPointers(weightMapNames, weightMapIndexToPtr);

    /*
    n_printf("Weight Map To Pointers Mapping Begins\n");
    for (int i = 0; i < weightMapIndexToPtr.Size(); i++)
    {
        n_printf("Map: %s    Pointer %x\n",
                 weightMapNames[weightMapIndexToPtr.GetKeyAt(i)].Get(), 
                 weightMapIndexToPtr.GetElementAt(i));
    }
    n_printf("Weight Map To Pointers Mapping Ends\n");
    */
    
    bool negativeWeightFound = false;
    bool overFourWeightsFound = false;
    bool weightsNormalized = false;
    
    // copy joint indices and weights into the mesh builder
    for (int polyIdx = 0; polyIdx < this->meshPolygons.Size(); polyIdx++)
    {
        const MeshPolygon& poly = this->meshPolygons[polyIdx];
        for (int vIdx = 0; vIdx < 3; vIdx++)
        {
            LWPntID pointId = this->curMeshInfo->GetPolygonVertex(poly.id, vIdx);
            int vWeightMapCount = 0;
            int jointIndices[4] = { 0 };
            float jointWeights[4] = { 0.0f };
            for (int wMapIdx = 0; wMapIdx < numWeightMaps; wMapIdx++)
            {
                void* weightMap = 0;
                if (!weightMapIndexToPtr.Find(wMapIdx, weightMap))
                {
                    // should never get here
                    n_assert(false);
                    break;
                }

                int weightMapDim = this->curMeshInfo->pntVSelect(weightMap);
                if (1 != weightMapDim)
                {
                    // should never get here
                    n_assert(false);
                    break;
                }
                
                float vWeight = 0.0f;
                if (this->curMeshInfo->pntVGet(pointId, &vWeight)) 
                {
                    if (vWeight < 0.0f) 
                    {
                        negativeWeightFound = true;
                    }
                    else if (vWeight > 0.0f)
                    {
                        if (vWeightMapCount < 4)
                        {
                            jointIndices[vWeightMapCount] = weightMapIndexToJointIndex.GetElement(wMapIdx);
                            jointWeights[vWeightMapCount] = vWeight;
                        }
                        else
                        {
                            overFourWeightsFound = true;
                        }
                        ++vWeightMapCount;
                    }
                }
            }
            vector4 jointsVec((float)jointIndices[0], (float)jointIndices[1], 
                              (float)jointIndices[2], (float)jointIndices[3]);
            vector4 weightsVec(jointWeights[0], jointWeights[1], 
                               jointWeights[2], jointWeights[3]);
            float totalWeight = weightsVec.x + weightsVec.y + weightsVec.z + weightsVec.w;
            // normalize weights if necessary so the sum of all weights for a vertex is 1.0
            if (!n_fequal(totalWeight, 1.0f, 0.01f))
            {
                weightsNormalized = true;
                weightsVec *= (1.0f / totalWeight);
            }
            nMeshBuilder::Vertex& v = this->meshBuilder.GetVertexAt(polyIdx * 3 + vIdx);
            v.SetJointIndices(jointsVec);
            v.SetWeights(weightsVec);
        }
    }

    if (negativeWeightFound)
    {
        nString msg("[WARNING] Negative weights found.");
        if (this->monitor)
        {
            this->monitor->Step(0, msg);
        }
        n_printf(msg.Get());
    }

    if (overFourWeightsFound)
    {
        nString msg("[WARNING] Over four weights per vertex found.");
        if (this->monitor)
        {
            this->monitor->Step(0, msg);
        }
        n_printf(msg.Get());
    }

    if (weightsNormalized)
    {
        nString msg("[WARNING] Weights had to be normalized.");
        if (this->monitor)
        {
            this->monitor->Step(0, msg);
        }
        n_printf(msg.Get());
    }

    weightMapNames.Clear();
}

//----------------------------------------------------------------------------
/**
*/
void 
nLWMeshExport::ExtractGeometry()
{
    nLWGlobals::ObjectInfo objectInfo;
    if (!objectInfo.IsValid())
    {
        n_printf("[ERROR] Invalid nLWGlobals::ObjectInfo %s.\n", __FUNCTION__);
        return;
    }

    LWItemID curItemId = this->objSettings->GetItemId();
    this->curMeshInfo = n_new(nLWMeshInfo(objectInfo.Get()->meshInfo(curItemId, 0)));
    this->surfaceHashMap = n_new(nHashMap());

    // get all the surfaces for the current item (some may be unused)
    nArray<MeshSurface> surfaceArray;
    this->ExtractAllMeshSurfaces(surfaceArray);
    // get all the triangles from Lightwave (don't put them into the mesh builder yet)
    this->ExtractMeshTriangles();
    
    if (!this->meshPolygons.Empty())
    {
        // figure out which surfaces are actually used
        this->CollectUsedSurfaces(surfaceArray);
        // put the triangles into the mesh builder
        this->CollectTriangles();

        if (this->NeedUV())
        {
            // group the triangles in the mesh builder by surface
            this->GroupTrianglesBySurface(surfaceArray);
            // add the UV components to the triangles in the mesh builder
            this->CollectUV();
        }
        
        if (this->NeedJointWeights())
        {
            // add the joint indices and weights to the triangles in the mesh builder
            this->CollectJointWeights();
        }
        
        // get the mesh builder ready for the next stage of processing
        this->meshBuilder.SortTriangles();
        // remove redundant vertices
        this->meshBuilder.Cleanup(0);
    }

    // free stuff we won't need from here on

    this->meshPolygons.Clear();

    if (this->surfaceHashMap)
    {
        n_delete(this->surfaceHashMap);
        this->surfaceHashMap = 0;
    }

    if (this->curMeshInfo)
    {
        n_delete(this->curMeshInfo);
        this->curMeshInfo = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWMeshExport::PartitionMesh()
{
    if (this->meshBuilder.HasVertexComponent(nMeshBuilder::Vertex::WEIGHTS))
    {
        this->skinMeshBuilder = n_new(nMeshBuilder);
        n_assert(this->skinMeshBuilder);
        if (!this->skinMeshBuilder)
        {
            return;
        }

        nSkinPartitioner skinPartitioner;
        const int maxJointPaletteSize = 24; // TODO: make this adjustable via exporter settings

        if (skinPartitioner.PartitionMesh(this->meshBuilder, 
                                          *this->skinMeshBuilder, 
                                          maxJointPaletteSize))
        {
            // store the mapping of fragments to pre-partitioning mesh groups
            // each fragment is mapped to exactly one mesh group
            const nArray<int>& fragmentGroupArray = skinPartitioner.GetGroupMappingArray();

            // pre-allocate some space
            this->groupFragmentsArray.SetFixedSize(this->numMeshGroups);
            for (int groupIdx = 0; groupIdx < this->numMeshGroups; groupIdx++)
            {
                this->groupFragmentsArray[groupIdx] = n_new(nArray<int>());
            }
            
            // map pre-partitioning mesh groups to fragments
            // each group will be mapped to one or more fragments
            for (int groupIdx = 0; groupIdx < this->numMeshGroups; groupIdx++)
            {
                for (int fragIdx = 0; fragIdx < fragmentGroupArray.Size(); fragIdx++)
                {
                    if (groupIdx == fragmentGroupArray[fragIdx])
                    {
                        this->groupFragmentsArray[groupIdx]->PushBack(fragIdx);
                    }
                }
            }

            // pre-allocate some space
            this->fragmentJointsArray.SetFixedSize(skinPartitioner.GetNumPartitions());
            this->fragmentJointsArray.Fill(0, this->fragmentJointsArray.Size(), 0);
            
            // map fragments to joint indices
            // each fragment will be mapped to one or more joint indices
            for (int fragIdx = 0; fragIdx < fragmentGroupArray.Size(); fragIdx++)
            {
                this->fragmentJointsArray[fragIdx] = n_new(nArray<int>(skinPartitioner.GetJointPalette(fragIdx)));
            }
        }
    }
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
