//-----------------------------------------------------------------------------
//  nmaxmesh_main.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxnullview.h"
#include "util/nstring.h"
#include "export2/nmaxoptions.h"
#include "tools/nmeshbuilder.h"

#include "export2/nmaxmesh.h"
#include "export2/nmaxbones.h"
#include "export2/nmaxvertexinfluence.h"
#include "export2/nmaxmaterial.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxcustattrib.h"
#include "export2/nmaxskinpartitioner.h"

#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/nfileserver2.h"
#include "tinyxml/tinyxml.h"

#include "scene/nattachmentnode.h"
#include "scene/nskinshapenode.h"
#include "scene/nshadowskinshapenode.h"
#include "nature/nswingshapenode.h"

//-----------------------------------------------------------------------------
/**
*/
nMaxMesh::nMaxMesh() : 
    maxNode(0),
    refMaker (NULL),
    mesh(NULL),
    deleteMesh(FALSE),
    beginSkin(false),
    isSkinned(false),
    isPhysique(false),
    meshType (Shape),
    isAttachedNode (false)
{
    vertexFlag = VertexNormal | VertexColor | VertexUvs;
}

//-----------------------------------------------------------------------------
/**
*/
nMaxMesh::~nMaxMesh()
{
}

//-----------------------------------------------------------------------------
/**
     Retrieves a Mesh from given INode.

     Call after ReferenceMaker::RenderBegin(0);

     @param inode
     @param needDelete true, if the primitive already has cached mesh.
                       (it should be deleted)
*/
Mesh* nMaxMesh::GetMeshFromRenderMesh(INode* inode, BOOL &needDelete)
{
    n_assert(inode);

    TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();

    Object* obj = nMaxUtil::GetBaseObject(inode, animStart);

    GeomObject* geomObj = (GeomObject*)obj;

    nMaxNullView nullView;
    needDelete = false;

    Mesh* mesh = 0;

    // Primitives that already have a mesh cached can just return a pointer to it 
    // (and set needDelete to false).
    mesh = geomObj->GetRenderMesh(animStart, inode, nullView, needDelete);

    return mesh;
}

//-----------------------------------------------------------------------------
/**
    Retrieves a TriObject from a given INode.

    The following shows that how to get a Mesh from retrieved TriObject.

    @param inode the node which we want to retrieve TriObject from
    @param needDelete true, if the retrieved TriObject should be deleted.

    @return pointer to the TriObject. Null, if the given node can not be
            converted to TriObject type.
*/
Mesh* nMaxMesh::GetMeshFromTriObject(INode* inode, BOOL &needDelete)
{
    n_assert(inode);

    TimeValue animStart = nMaxInterface::Instance()->GetAnimStartTime();
    
    Object* obj = nMaxUtil::GetBaseObject(inode, animStart);

    if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
    {
        TriObject* tri = (TriObject*)obj->ConvertToType(animStart, 
                                                Class_ID(TRIOBJ_CLASS_ID, 0));

        needDelete = false;
        // if the pointer to the object is not equal to the retrieved pointer,
        // the TriObject should be deleted after.
        if (obj != tri)
            needDelete = true;

        Mesh* mesh = &(tri->mesh);
        this->triObj = tri;

        return mesh;
    }
    else
    {
        n_maxlog(Error, "Could not get mesh from TriObject of %s", inode->GetName());
        return NULL;
    }
}

//-----------------------------------------------------------------------------
/**
    Retrieves mesh from the given node.

    @param inode 3dsmax node
    @return pointer to Mesh, if there's no mesh, return NULL.
*/
Mesh* nMaxMesh::LockMesh(INode* inode)
{
    n_assert(refMaker == NULL);

    refMaker = inode;
    refMaker->RenderBegin(0);

    this->mesh = GetMeshFromRenderMesh(inode, this->deleteMesh);
    //this->mesh = GetMeshFromTriObject(inode, this->deleteMesh);

    return mesh;
}

//-----------------------------------------------------------------------------
/**
    Delete retrieved mesh file is other than original one.
*/
void nMaxMesh::UnlockMesh()
{
    //n_assert(refMaker != NULL);

    if (this->deleteMesh)
    {
        this->mesh->DeleteThis();
        //this->triObj->DeleteThis();
    }

    refMaker->RenderEnd(0);
    refMaker = NULL;

    this->triObj = 0;
    this->mesh = 0;
    this->deleteMesh = false;
}

//-----------------------------------------------------------------------------
/**
    Retrieves mesh options from node custom attribute and create xml elements
    based on it. 

    @return 
*/
bool nMaxMesh::GetCustAttrib(INode* inode)
{
    // xml document which created xml elements are linked.
    TiXmlDocument xmlDoc;

    Object* obj = nMaxUtil::GetBaseObject(inode, 0);
    if (!obj)
    {
        n_maxlog(Midium, "The node %s has no object.", inode->GetName());
        return false;
    }

    // convert node custom attributes to xml data.
    nMaxCustAttrib custAttrib;
    if (!custAttrib.Convert(obj, xmlDoc))
    {
        n_maxlog(High, "The node %s has no custom attribute.", inode->GetName());
        return false;
    }

//#ifdef _DEBUG
//    xmlDoc.SaveFile("c:\\custattrb.xml");
//#endif

    TiXmlHandle xmlHandle(&xmlDoc);

    TiXmlElement* e;

    // parameter name of mesh types
    const char* mtParamName = "MeshTypes";

    e = xmlHandle.FirstChild(mtParamName).Element();
    if (e)
    {
        TiXmlElement* child;

        // get the mesh type
        child = xmlHandle.FirstChild(mtParamName).FirstChild("meshtype").Child("", 0).Element();
        if (child)
        {
            int type;
            child->Attribute("value", &type);
        
            // radiobutton of max script ui start with '1'.
            switch(type)
            {
            case 1:
                this->meshType = Shape;
                break;
            case 2:
                this->meshType = Swing;
                break;
            case 3:
                this->meshType = Shadow;
                break;
            case 4:
                this->meshType = Collision;
                break;
            case 0:
            default:
                break;
            }
        }

        // see the node is attached node.
        child = xmlHandle.FirstChild(mtParamName).FirstChild("attachednode").Child("", 0).Element();
        if (child)
        {
            int flag;
            child->Attribute("value", &flag);
            if (flag)
                this->isAttachedNode = true;
            else
                this->isAttachedNode = false;
        }
    }

    // parameter name of vertex options
    const char* voParamName = "VertexOptions";

    e = xmlHandle.FirstChild(voParamName).Element();
    if (e)
    {
        // retrieves mesh vertex options from xml.
        int flag;

        TiXmlElement* child;
        //for (e; e; e = e->NextSiblingElement())
        {
            child = xmlHandle.FirstChild(voParamName).FirstChild("normal").Child("", 0).Element();
            if (child)
            {
                child->Attribute("value", &flag);
                if (flag)
                    this->vertexFlag |= VertexNormal;
            }

            child = xmlHandle.FirstChild(voParamName).FirstChild("color").Child("", 0).Element();
            if (child)
            {
                child->Attribute("value", &flag);
                if (flag)
                    this->vertexFlag |= VertexColor;
            }

            child = xmlHandle.FirstChild(voParamName).FirstChild("uvs").Child("", 0).Element();
            if (child)
            {
                child->Attribute("value", &flag);
                if (flag)
                    this->vertexFlag |= VertexUvs;
            }

            child = xmlHandle.FirstChild(voParamName).FirstChild("tangent").Child("", 0).Element();
            if (child)
            {
                child->Attribute("value", &flag);
                if (flag)
                    this->vertexFlag |= VertexTangent;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Create proper nebula scene node object.

    @param inode 3dsmax geometry node for the mesh.
    @param nodename A name for this node, proper name is passed if the the mesh
                    has multi material.

    @return created nebula scene node object.
*/
nSceneNode* nMaxMesh::CreateShapeNode(INode* inode, nString &nodename)
{
    nSceneNode* sceneNode = 0;

    // check that the given node is skinned mesh or static mesh.
    bool skinnedMesh = false;
    if (this->IsSkinned() || this->IsPhysique())
        skinnedMesh = true;
    else
        skinnedMesh = false;

    if (!skinnedMesh)
    {
        // the given node is not a skinned mesh.

        if (nMaxBoneManager::Instance()->GetNumBones() > 0)
        {
            // we have skinned mesh elsewhere in the scene,
            // so this node may be shape node of a attachment node or collision mesh.

            if (this->isAttachedNode)
            {
                INode* parent = inode->GetParentNode();
                int boneID = nMaxBoneManager::Instance()->FindBoneIDByNode(parent);
                if (boneID >= 0)
                {
                    // the parent node is any one of the bones,
                    // so create nAttachmentNode for this shape node
                    nAttachmentNode* attachNode = (nAttachmentNode*)CreateNebulaObject("nattachmentnode", nodename.Get());

                    //specify attachment node.
                    //FIXME: WIP
                    attachNode->SetSkinAnimator("../skinanimator");
                    attachNode->SetJointByIndex(boneID);
                }

                if (this->meshType == Shadow)
                    sceneNode =  (nShapeNode*)CreateNebulaObject("nshadowshapenode", nodename.Get());
                else
                if (this->meshType == Shape)
                    sceneNode =  (nShapeNode*)CreateNebulaObject("nshapenode", nodename.Get());
                else
                {
                    sceneNode = 0; // collision mesh
                }
            }
            else
            {
                // error
                n_maxlog(Error, "The node '%s' is suspected to be attached node but does not have 'attached' option. \n \
                                 Be sure that it is attached node.", inode->GetName());
            }
        }
        else
        {   // we do not have any skinned mesh in the scene
            // and the given mesh is shape node which has static mesh.
            if (this->meshType == Shadow)
                sceneNode =  (nShapeNode*)CreateNebulaObject("nshadowshapenode", nodename.Get());
            else
            if (this->meshType == Shape)
                sceneNode =  (nShapeNode*)CreateNebulaObject("nshapenode", nodename.Get());
            else
            if (this->meshType == Swing)
                sceneNode = (nSwingShapeNode*)CreateNebulaObject("nswingshapenode", nodename.Get());
            else
            {
                n_assert(this->isAttachedNode != false);
                sceneNode = 0; // collision mesh
            }
        }
    }
    else
    {
        // the given node is skinned mesh.
        if (this->meshType == Shadow)
            sceneNode = (nShadowSkinShapeNode*)CreateNebulaObject("nshadowskinshapenode", nodename.Get());
        else
        if (this->meshType == Shape)
            sceneNode = (nShapeNode*)CreateNebulaObject("nskinshapenode", nodename.Get());
        else
        {
            n_assert(this->isAttachedNode != false);
            sceneNode = 0; // collision mesh
        }
    }

    return sceneNode;
}

//-----------------------------------------------------------------------------
/**
    Get mesh and material data from the given node.

    @param inode
    @param globalMeshBuilder mesh builder to merge each of mesh data into it.
    @param useIndivisualMesh save mesh data per 3dsmax geometry node if it is true.

    @return return created nebula object. 
*/
nSceneNode* nMaxMesh::Export(INode* inode, nMeshBuilder* globalMeshBuilder, bool useIndivisualMesh)
{
    this->maxNode = inode;

    nSceneNode* createdNode = 0;

    // get custom attributes of the node if it exist.
    if (GetCustAttrib(inode))
    {
        n_maxlog(Midium, "%s node has custom attributes.", inode->GetName());
    }

    // check for that this mesh is skinned(or physique). 
    // if it does, do some initializations to acess max skin(or physique) interface 
    // to extract vertex weight.
    //bool skinnedMesh = false;
    //skinnedMesh = BeginSkin(inode);
    BeginSkin(inode);

    // Get the number of materials to check this mesh uses single material or multi material.
    // The reason to use GetNumMaterial instead of using nMaxMaterial::GetType() is that
    // there's multi-sub material type which only has one sub material.
    // In this case, we process this mesh as same way of a single material mesh.
    int numMaterials = nMaxUtil::GetNumMaterials(inode);
    if (numMaterials == 1)
    {
        // we have single material.
     
        nString nodename(inode->GetName());

        createdNode = this->CreateShapeNode(inode, nodename);

        nMeshBuilder* meshBuilder = (useIndivisualMesh ? &this->localMeshBuilder : globalMeshBuilder);

        int baseGroupIndex = GetMesh(inode, meshBuilder, -1, 1);

        // save group index for skin partitioning and mesh fragments.
        if (createdNode)
        {
            SetShapeGroup((nShapeNode*)createdNode, baseGroupIndex, numMaterials);

            // specifies local bounding box.
            bbox3 localBox;
            if (useIndivisualMesh)
                localBox = this->localMeshBuilder.GetGroupBBox(baseGroupIndex);
            else
                localBox = globalMeshBuilder->GetGroupBBox(baseGroupIndex);

            ((nShapeNode*)createdNode)->SetLocalBox(localBox);
        }

        // build material
        if (this->meshType == Shape || this->meshType == Swing)
            GetMaterial(inode, (nShapeNode*)createdNode, 0);

        // save mesh file and specifies it to the shape node.
        SetMeshFile((nShapeNode*)createdNode, nodename, useIndivisualMesh);

        //createdNode = shapeNode;
    }
    else
    {
        // we have multi materials

        // Create nTransform node for a mesh has multi material.
        // Guess that, a multi material mesh has some other child mesh in its hierarchy.
        // The multi material mesh to be splited by its number of the material and create
        // nShapeNode(or nSkinShapeNode) as that number. 
        // And so, where its child mesh to be located? We put its child under nTransformNode.
        // e.g) 
        //    nTransformNode parent // parent mesh
        //        nShapeNode material0
        //        nShapeNode material1
        //        nTransformNode child0 //  child mesh
        //            nShapeNode child_material0
        //            nShapeNode child_material1
        nTransformNode* transformNode = 0;

        nString transformNodeName;
        //transformNodeName = nMaxUtil::CorrectName(inode->GetName());
        //transformNode = (nTransformNode*)CreateNebulaObject("ntransformnode", transformNodeName.Get());
        transformNode = (nTransformNode*)CreateNebulaObject("ntransformnode", inode->GetName());

        bbox3 parentLocalBox;

        for (int matIdx=0; matIdx<numMaterials; matIdx++)
        {
            // create node name. we append material index to the end of the node name.
            nString nodename;
            nodename += inode->GetName();
            nodename += "_";
            nodename.AppendInt(matIdx);

            createdNode = this->CreateShapeNode(inode, nodename);

            // build mesh.
            nMeshBuilder* meshBuilder = (useIndivisualMesh ? &this->localMeshBuilder : globalMeshBuilder);

            int baseGroupIndex = GetMesh(inode, meshBuilder, matIdx, numMaterials);

            // save group index for skin partitioning and mesh fragments.
            if (createdNode)
            {
                SetShapeGroup((nShapeNode*)createdNode, baseGroupIndex, numMaterials);

                // specifies local bounding box.
                bbox3 localBox;
                if (useIndivisualMesh)
                    localBox = this->localMeshBuilder.GetGroupBBox(baseGroupIndex);
                else
                    localBox = globalMeshBuilder->GetGroupBBox(baseGroupIndex);

                ((nShapeNode*)createdNode)->SetLocalBox(localBox);

                parentLocalBox.extend(localBox);
            }

            // build material.
            if (this->meshType == Shape || this->meshType == Swing)
                GetMaterial(inode, (nShapeNode*)createdNode, matIdx);

            // save mesh file and specifies it to the shape node.
            SetMeshFile((nShapeNode*)createdNode, nodename, useIndivisualMesh);
            
            // set cwd to the parent to put the shape in the same hierarchy.
            // (cause all these shapes belong to same mesh)
            nKernelServer::Instance()->PopCwd();
        }

        // specifies local bouding box of multi-sub transform node's.
        transformNode->SetLocalBox(parentLocalBox);
        
        //nKernelServer::Instance()->PopCwd();

        createdNode = transformNode;
    }

    if (useIndivisualMesh)
    {
        //PartitionMesh();
        nArray<nMaxMesh*> meshArray;
        meshArray.Append(this);

        nMaxSkinPartitioner skinPartitioner;
        skinPartitioner.Partitioning(meshArray, this->localMeshBuilder);
    }

    //if(skinnedMesh)
    if (this->IsSkinned() || this->IsPhysique())
    {
        EndSkin();
    }

    //FIXME:
    if (this->isAttachedNode)
    {
        // if the mesh is attachement node, we should pop cwd once more for attachment node.
        nKernelServer::Instance()->PopCwd();
    }

    return createdNode;
}

//-----------------------------------------------------------------------------
/**
    Check the given mesh uses per vertex alpha or not.
*/
bool nMaxMesh::UsePerVertexAlpha(Mesh* mesh)
{
    return (mesh->mapSupport(MAP_ALPHA) ? true : false);
}

//-----------------------------------------------------------------------------
/**
    Get mesh from given 3dsmax node.

    @param inode 3dsmax's node.
    @param meshBuilder mesh builder which to be used to save mesh.
    @param matidx material index
    @param numMats number of the material of given node
    @param baseGroup
*/
int nMaxMesh::GetMesh(INode* inode, nMeshBuilder* meshBuilder, const int matIdx, const int numMats)
{
    n_assert(meshBuilder);

    Mesh* mesh = this->LockMesh(inode);
    if (!mesh)
    {
        n_maxlog(-1, "The node '%s' does not have mesh.", inode->GetName());
        return -1;
    }

    int baseVertexIndex = meshBuilder->GetNumVertices();
    int baseTriangleIndex = meshBuilder->GetNumTriangles();

//begin calc group index
    int baseGroupIndex;
    nArray<nMeshBuilder::Group> groupMap;
    meshBuilder->BuildGroupMap(groupMap);
    baseGroupIndex = groupMap.Size();
//end calc group index    

    int iVertCount = 0;

    //TODO: check if we need vertex normal.
    // build vertex normal of the given mesh
    mesh->buildNormals();
    mesh->checkNormals(TRUE);

    this->pivMat.IdentityMatrix();
    if (!IsPhysique() && !IsSkinned())
    {
        pivMat = (inode->GetObjectTM(0)*Inverse(inode->GetNodeTM(0))) * pivMat;
    }
    else
    {
        pivMat = inode->GetObjectTM(0);
    }

    int i;
    int numFaces = mesh->getNumFaces();

    int v1, v2, v3;

    for (i=0; i<numFaces; i++)
    {
        if (!HasSameMaterial(mesh, i, matIdx, numMats))
            continue;

        Face& face = mesh->faces[i];

        if (HasNegativeScale(inode))
        {
            v1 = 0; v2 = 2; v3 = 1;
        }
        else
        {
            v1 = 0; v2 = 1; v3 = 2;
        }

        // get the actual index of vertex in a vertex array of a Mesh.
        int vIdx1 = face.v[v1];
        int vIdx2 = face.v[v2];
        int vIdx3 = face.v[v3];

        nMeshBuilder::Vertex vertex1, vertex2, vertex3;

        vertex1 = GetVertex(mesh, face, i, v1, vIdx1);
        vertex2 = GetVertex(mesh, face, i, v2, vIdx2);
        vertex3 = GetVertex(mesh, face, i, v3, vIdx3);

        meshBuilder->AddVertex(vertex1);
        meshBuilder->AddVertex(vertex2);
        meshBuilder->AddVertex(vertex3);

        nMeshBuilder::Triangle triangle;
        triangle.SetVertexIndices(baseVertexIndex + iVertCount,       //FIXME: use v1, v2, v3?
                                  baseVertexIndex + iVertCount + 1,
                                  baseVertexIndex + iVertCount + 2 );

        if (IsPhysique() || IsSkinned())
            triangle.SetUsageFlags(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);
        else
            triangle.SetUsageFlags(nMesh2::WriteOnce);

        //triangle.SetGroupId(baseGroupIndex+matIdx+1);
        triangle.SetGroupId(baseGroupIndex);

        if (matIdx >= 0)
            triangle.SetMaterialId(matIdx);

        meshBuilder->AddTriangle(triangle);

        iVertCount += 3;
    }

    this->UnlockMesh();


#ifdef _DEBUG
    n_maxlog(Midium, "Number of vertices of %s node are %d", inode->GetName(), 
                                                             meshBuilder->GetNumVertices());
#endif
    //return true;
    return baseGroupIndex;
}

//-----------------------------------------------------------------------------
/**
    Get per vertex component from 3dsmax's Mesh and specifies it to 
    mesh builder's vertex.

    @return mesh builder's vertex.
*/
nMeshBuilder::Vertex nMaxMesh::GetVertex(Mesh* mesh, Face& face, int faceNo, 
                                         int vtxIdx, int index)
{
    nMeshBuilder::Vertex vertex;

    vector3 pos = GetVertexPosition(mesh, index);
    vertex.SetCoord(pos);

    nMaxOptions* option = nMaxOptions::Instance();

    // vertex normal.
    if (option->ExportNormals())
    {
        vector3 norm = GetVertexNormal(mesh, face, faceNo, vtxIdx);
        vertex.SetNormal(norm);
    }

    // vertex color.
    if (option->ExportColors())
    {
        vector4 col = GetVertexColor(mesh, faceNo, vtxIdx);
        vertex.SetColor(col);
    }

    // vertex uvs.
    if (option->ExportUvs())
    {
        //FIXME: is 'm' identical to map channel?
        int layer = 0;
        for (int m=1; m<MAX_MESHMAPS-1; m++)
        {
            if (mesh->mapSupport(m))
            {
                vector2 uvs = GetVertexUv(mesh, faceNo, vtxIdx, m);
                vertex.SetUv(layer++, uvs);
            }
        }
    }

    if (IsPhysique() || IsSkinned())
    {
        vector4 joints, weights;

        this->GetVertexWeight(index, joints, weights);

        vertex.SetJointIndices(joints);
        vertex.SetWeights(weights);
    }
    return vertex;
}

//-----------------------------------------------------------------------------
/**
    Get vertex position.
*/
vector3 nMaxMesh::GetVertexPosition(Mesh* mesh, int index)
{
    vector3 pos;

    Point3& v = mesh->verts[index] * pivMat;

    pos.set(-v.x, v.z, v.y);

    return pos;
}

//-----------------------------------------------------------------------------
/**
    Get vertex normal.
*/
vector3 nMaxMesh::GetVertexNormal(Mesh* mesh, Face& face, int faceNo, int vtxIdx)
{
    vector3 normal;
    Point3& vn = GetVertexNormal(mesh, faceNo, mesh->getRVertPtr((face.getVert(vtxIdx))));

    normal.set(-vn.x, vn.z, vn.y);

    return normal;
}

//-----------------------------------------------------------------------------
/**
   Get vertex normal.
*/
Point3 nMaxMesh::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
    Face* f = &mesh->faces[faceNo];
    DWORD smGroup = f->smGroup;
    int numNormals;
    Point3 vertexNormal;

    // Is normal specified
    // SPCIFIED is not currently used, but may be used in future versions.
    if (rv->rFlags & SPECIFIED_NORMAL)
    {
        vertexNormal = rv->rn.getNormal();
    }
    // If normal is not specified it's only available if the face belongs
    // to a smoothing group
    else 
        if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
        {
            // If there is only one vertex is found in the rn member.
            if (numNormals == 1) 
            {
                vertexNormal = rv->rn.getNormal();
            }
            else
            {
                // If two or more vertices are there you need to step through them
                // and find the vertex with the same smoothing group as the current face.
                // You will find multiple normals in the ern member.
                for (int i = 0; i < numNormals; i++)
                {
                    if (rv->ern[i].getSmGroup() & smGroup)
                    {
                        vertexNormal = rv->ern[i].getNormal();
                    }
                }
            }
        }
        else
        {
            // Get the normal from the Face if no smoothing groups are there
            vertexNormal = mesh->getFaceNormal(faceNo);
        }

        return vertexNormal;
}

//-----------------------------------------------------------------------------
/**
    Get vertex color.
*/
vector4 nMaxMesh::GetVertexColor(Mesh* mesh, int faceNo, int vtxIdx)
{
    AColor color;

    if (mesh->getNumVertCol())
    {
        Point3 col = mesh->vertCol[mesh->vcFace[faceNo].t[vtxIdx]];

        color.r = col.x; color.g = col.y; color.b = col.z; color.a = 1.0f;
    }
    else
    {
        color.White();
    }

    if (UsePerVertexAlpha(mesh))
    {
        UVVert* uvVerts = mesh->mapVerts(MAP_ALPHA);
        if (uvVerts)
        {
            TVFace* tvFace = &mesh->mapFaces(MAP_ALPHA)[faceNo];
            color.a = uvVerts[tvFace->t[vtxIdx]].x;
        }
    }

    vector4 vertexCol;
    vertexCol.set(color.r, color.g, color.b, color.a);

    return vertexCol;
}

//-----------------------------------------------------------------------------
/**
    Get vertex uv.
*/
vector2 nMaxMesh::GetVertexUv(Mesh* mesh, int faceNo, int vtxIdx, int m)
{
    vector2 uvs;

    int numTV = mesh->getNumMapVerts(m);
    if (numTV)
    {
        int a = mesh->mapFaces(m)[faceNo].t[vtxIdx];

        Point3 pt1 = mesh->mapVerts(m)[a];

        uvs.set(pt1.x, 1.0f - pt1.y);
    }
    else
    {
        switch(vtxIdx)
        {
        case 0:
            uvs.set(0.0f, 0.0f);
            break;
        case 1:
            uvs.set(1.0f, 0.0f);
            break;
        case 2:
            uvs.set(0.0f, 1.0f);
            break;
        }
    }

    return uvs;
}

//-----------------------------------------------------------------------------
/**
    Extract vertex weight value from given vertex

    @param vertexIdx vertex index.
*/
void nMaxMesh::GetVertexWeight(int vertexIdx, vector4 &jointIndices, vector4 &weights)
{
    nMaxVertexInfluence vertexInfluence;

    if (IsPhysique())
    {
        IPhyVertexExport* vertexExport = GetVertexInterface(vertexIdx);
        if (vertexExport)
        {
            int vertexType = vertexExport->GetVertexType();

            switch(vertexType)
            {
            case RIGID_BLENDED_TYPE:
                {
                    IPhyBlendedRigidVertex* blended;
                    blended = static_cast<IPhyBlendedRigidVertex*>(vertexExport);

                    for (int i=0; i<blended->GetNumberNodes(); i++)
                    {
                        INode* bone = blended->GetNode(i);
                        float weight = blended->GetWeight(i);

                        if (bone)
                        {
                            nString boneName = nMaxUtil::CorrectName(bone->GetName());
                            int boneID = nMaxBoneManager::Instance()->FindBoneIDByName(boneName);

                            if (boneID >= 0)
                            {
                                vertexInfluence.AddBoneInfluence(boneID, weight);
                            }
                            else
                            {
                                n_maxlog(Error, "Bone Influence Error: bone %s", bone->GetName());
                            }
                        }
                        else
                        {
                            n_maxlog(High, "No bone influence for this vertex: bone '%s' vertex Index [%d]", 
                                bone->GetName(), vertexIdx);
                        }
                    }
                }
                break;
            case RIGID_TYPE:
                {
                    IPhyRigidVertex* rigid;
                    rigid = static_cast<IPhyRigidVertex*>(vertexExport);

                    INode* bone  = rigid->GetNode();
                    float weight = 1.0f;

                    if (bone)
                    {
                        nString boneName = nMaxUtil::CorrectName(bone->GetName());
                        int boneID = nMaxBoneManager::Instance()->FindBoneIDByName(boneName);

                        if (boneID >= 0)
                        {
                            vertexInfluence.AddBoneInfluence(boneID, weight);
                        }
                        else
                        {
                            n_maxlog(Error, "Bone Influence Error: bone %s", bone->GetName());
                        }
                    }
                    else
                    {
                        n_maxlog(High, "No bone influence for this vertex: bone '%s' vertex Index [%d]", 
                            bone->GetName(), vertexIdx);
                    }
                }
                break;
            }//end of switch

            ReleaseVertexInterface(vertexExport);
            vertexExport = 0;

        }
        else
        {
            n_maxlog(Error, "Invalid IPhyVertexExport.");
            return;
        }
    }
    else
    if (IsSkinned())
    {
        ISkinContextData* skinContext = GetSkinContextData();

        const int numBones = skinContext->GetNumAssignedBones(vertexIdx);
        for (int i=0; i<numBones; i++)
        {
            // retrieves the index of the bone that affecting the vertex.
            int boneIdx = skinContext->GetAssignedBone(vertexIdx, i);
            if (boneIdx < 0)
            {
                // bone index should not be less than zero.
                boneIdx = 0;
                continue;
            }

            INode* bone = GetSkin()->GetBone(boneIdx);
            float weight = skinContext->GetBoneWeight(vertexIdx, i);

            if (bone)
            {
                nString boneName = nMaxUtil::CorrectName(bone->GetName());
                int boneID = nMaxBoneManager::Instance()->FindBoneIDByName(boneName);

                if (boneID >= 0)
                {
                    vertexInfluence.AddBoneInfluence(boneID, weight);
                }
                else
                {
                    n_maxlog(Error, "Bone Influence Error: bone ID[%d]", boneID);
                }
            }
            else
            {
                n_maxlog(High, "No bone influence for this vertex: bone [%s] vertex Index [%d]", 
                    bone->GetName(), vertexIdx);
            }
        }
    }
    else
    {
        // should not reach here!
        n_maxlog(Error, "Failed to get the physique or skin context for the given node %s", 
            this->maxNode->GetName());
    }

//#if CURRENT_VERSION > 3 // CS version : 313 in 3dsmax7
//    // export floating bones
//    IPhyFloatingVertex* floatVertex;
//    floatVertex = static_cast<IPhyFloatingVertex*>(GetVertexInterface(vertexIdx));
//
//    if (floatVertex)
//    {
//        for(int i=0; i<floatVertex->GetNumberNodes(); i++)
//        {
//            INode* bone = floatVertex->GetNode(i);
//            float totalWeight;
//            float weight = floatVertex->GetWeight(i, totalWeight);
//
//            if (bone)
//            {
//                nString boneName = nMaxUtil::CorrectName(bone->GetName());
//                int boneID = nMaxBoneManager::Instance()->FindBoneIDByName(boneName);
//
//                if (boneID >= 0)
//                {
//                    vertexInfluence.AddBoneInfluence(boneID, weight);
//                }
//                else
//                {
//                    n_maxlog(Error, "Bone Influence Error: bone ID[%d]", boneID);
//                }
//            }
//            else
//            {
//                n_maxlog(High, "No bone influence for this vertex: bone '%s' vertex Index [%d]", 
//                bone->GetName(), vertexIdx);
//            }
//        }
//    }
//#endif

    // calculate bone influences for most important four bones.
    float weightTrashHold = nMaxOptions::Instance()->GetWeightTrashold();
    vertexInfluence.AdjustBonesInfluence(weightTrashHold);

    // specify weight and joint indices.
    jointIndices = vertexInfluence.GetIndicesVector();
    weights = vertexInfluence.GetWeightsVector();
}

//-----------------------------------------------------------------------------
/**
    Get material from given node.

    @param inode 3dsmax node to retrieve material.
    @param matID -1, if the given node has single material.
*/
void nMaxMesh::GetMaterial(INode* inode, nShapeNode* shapeNode, int matID)
{
    n_assert(shapeNode);

    nMaxMaterial material;

    material.Export(inode->GetMtl(), shapeNode, matID);
}

//-----------------------------------------------------------------------------
/**
    Determine given mesh's face has same material.

    @param mesh point to the Mesh
    @param faceIdx face index
    @param matIdx material index
    @param numMats number of material of the given Mesh.
*/
bool nMaxMesh::HasSameMaterial(Mesh* mesh, int faceIdx, const int matIdx, const int numMats)
{
    // Retrieves the zero based material index of the 'i-th' face.
    MtlID matID = mesh->getFaceMtlIndex(faceIdx) % numMats;

    return ((matIdx == -1 && numMats == 1) || matID == matIdx);
}

//-----------------------------------------------------------------------------
/**
    Negatively scaled node has inverted order of vertices. (counter clockwise)

    For example, consider exporting mirrored objects. 
    The mirrored objects has negative scale factor and the vertices of that 
    should be exported with counter clockwise order not to be inverted.

    @param matrix of a node.
    @return true if the given matrix has negative scaling.
*/
bool nMaxMesh::HasNegativeScale(Matrix3& m)
{
    Point3 cp = CrossProd(m.GetRow(0),m.GetRow(1));
    if (DotProd(cp, m.GetRow(2)) < 0.0f)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
/**
    'parity' of the matrix is set if one axis of the matrix is scaled negatively.

    @return return parity of the given node's world space transform.
*/
bool nMaxMesh::HasNegativeScale(INode* inode)
{
    n_assert(inode);
    return (inode->GetNodeTM(0).Parity() ? true : false);
}

//-----------------------------------------------------------------------------
/**
    Retrieves used map channels from given mesh.

    @param mesh pointer to the given Mesh.
    @return array of used map channel, size of the array is the number of map 
            channel which used by this the mesh.
*/
nArray<int> nMaxMesh::GetUsedMapChannels(Mesh* mesh)
{
    nArray<int> mapChannelArray;

    // m = 0 : used for vertex color.
    for (int m=1; m<MAX_MESHMAPS-1; m++) 
    {
        if (mesh->mapSupport(m)) 
        {
            int numTVx = mesh->getNumMapVerts(m);
            if (numTVx) 
            {
                mapChannelArray.Append(m);
            }
        }
    }

    return mapChannelArray;
}

//-----------------------------------------------------------------------------
/**
*/
void nMaxMesh::SetShapeGroup(nShapeNode* createdNode, int baseGroupIndex, int numMaterials)
{
    if (this->IsSkinned() || this->IsPhysique())
    {
        nSkinShapeNode* skinShapeNode = static_cast<nSkinShapeNode*>(createdNode);

        if (numMaterials > 1)
            skinShapeNode->SetSkinAnimator("../../skinanimator");
        else
            skinShapeNode->SetSkinAnimator("../skinanimator");

        nMaxSkinMeshData groupMesh;
        groupMesh.node       = skinShapeNode;
        groupMesh.groupIndex = baseGroupIndex;

        this->skinmeshArray.Append(groupMesh);
    }
    else
    {
        nShapeNode* shapeNode = static_cast<nShapeNode*>(createdNode);
        shapeNode->SetGroupIndex(baseGroupIndex);
    }
}

//-----------------------------------------------------------------------------
/**
    Specify mesh filename to the shape node.
    If each shape node uses its own mesh file, it set its node name for its mesh
    filename otherwise uses save name.

*/
void nMaxMesh::SetMeshFile(nShapeNode* shapeNode, nString &nodeName, bool useIndivisualMesh)
{
    if (useIndivisualMesh)
    {
        this->localMeshBuilder.Cleanup(0);

        BuildMeshTangentNormals(this->localMeshBuilder);

        nString filename;
        filename += nMaxOptions::Instance()->GetMeshesPath();
        filename += nMaxUtil::CorrectName(nodeName);
        filename += nMaxOptions::Instance()->GetMeshFileType();

        this->localMeshBuilder.Optimize(); //TODO!
        this->localMeshBuilder.Save(nKernelServer::Instance()->GetFileServer(), filename.Get());

        if (shapeNode)
        {
            // specify shape node's name.
            nString meshname;
            meshname += nMaxOptions::Instance()->GetMeshesAssign();
            meshname += nMaxUtil::CorrectName(nodeName);
            meshname += nMaxOptions::Instance()->GetMeshFileType();
            shapeNode->SetMesh(meshname.Get());
        }
    }
    else
    {
        if (shapeNode)
        {
            // specify shape node's name.
            nString meshname;
            meshname += nMaxOptions::Instance()->GetMeshesAssign();
            meshname += nMaxOptions::Instance()->GetSaveFileName();
            meshname += nMaxOptions::Instance()->GetMeshFileType();
            shapeNode->SetMesh(meshname.Get());
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Build tangents for the given mesh.

    If the mesh already has vertex normals, it builds triangle normals and tangent
    by calling nMeshBuilder::BuildTriangleTangents() otherwise it builds
    triangle normals then vertex normals. Finally, it builds vertex tangents.

    Be sure that the given mesh has uv0 texture coordinate. Tangent needs it
    for its calculations.

    @param meshBuilder input to build its tangents.
    @return true, if it success.
*/
bool nMaxMesh::BuildMeshTangentNormals(nMeshBuilder &meshBuilder)
{
    const nMeshBuilder::Vertex& v = meshBuilder.GetVertexAt(0);

    if (nMaxOptions::Instance()->ExportTangents())
    {
        // build triangle normals, vertex normals and tangents.
        n_maxlog(Low, "Build tangents...");

        if (v.HasComponent(nMeshBuilder::Vertex::UV0))
        {
            if (v.HasComponent(nMeshBuilder::Vertex::NORMAL))
            {
                // we already have vertex normal, so only need triangle normal and tangent.
                n_maxlog(Low, "  - Build triangle tangents...");
                meshBuilder.BuildTriangleTangents();
            }
            else
            {
                // there's no vertex normals. so we first build triangle normals then,
                n_maxlog(Low, "  - Build triangle normals...");
                meshBuilder.BuildTriangleNormals();

                // build vertex normals by averaging triangle normals.
                n_maxlog(Low, "  - Build vertex normals...");
                meshBuilder.BuildVertexNormals();
            }

            n_maxlog(Low, "  - Build vertex tangents...");
            meshBuilder.BuildVertexTangents();
        }
        else
        {
            n_maxlog(Error, "The tangents require a valid uv-mapping in texcoord layer 0.");
            return false;
        }

        n_maxlog(Low, "Build mesh tangents done.");
    }
    else
    if (nMaxOptions::Instance()->ExportNormals())
    {
        // build triangle normals, vertex normals.
        n_maxlog(Low, "Build normals...");
        
        if (v.HasComponent(nMeshBuilder::Vertex::NORMAL))
        {
            // we already have vertex normal, so only need triangle normal and tangent.
            n_maxlog(Low, "  - Build triangle normals...");
            meshBuilder.BuildTriangleNormals();
        }
        else
        {
            // there's no vertex normals. so we first build triangle normals then,
            n_maxlog(Low, "  - Build triangle normals...");
            meshBuilder.BuildTriangleNormals();

            // build vertex normals by averaging triangle normals.
            n_maxlog(Low, "  - Build vertex normals...");
            meshBuilder.BuildVertexNormals();
        }

        n_maxlog(Low, "Build mesh normals done.");
    }

    return true;
}
