#include "export/nmaxexport.h"
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
*/

//------------------------------------------------------------------------------
/**
    Force vertex components as specified in the task.

    Search a slot with the same VertexComponents in the pool, add it there.
    If not found start a new pool slot.

    @param meshObject   the meshObject holding the group informations from the meshBuilder to add
    @param meshBuilder  the meshBuilder to add
*/
void
nMaxExport::appendDataToPool(MeshObject &meshObject, nMeshBuilder* meshBuilder, nAnimBuilder *animBuilder, bool skinned)
{
    n_assert(meshBuilder);
    n_assert(meshBuilder->GetNumTriangles() > 0 && meshBuilder->GetNumVertices() > 0);
    nMeshBuilder* destMeshBuilder = 0;

    //force vertex components as defined by task
    if (this->task->forcedVextexComponents != 0)
        meshBuilder->ForceVertexComponents(this->task->forcedVextexComponents);

    //make shure that all verticies has the same vertex components
    meshBuilder->ExtendVertexComponents();

    //get the used vertex layout
    const int sourceVC  = meshBuilder->GetVertexAt(0).GetComponentMask();
    const int sourceUsage = meshBuilder->GetTriangleAt(0).GetUsageFlags();
    
    //check if we have to split the group for skinning
    //if ((sourceVC & ( nMeshBuilder::Vertex::JINDICES | nMeshBuilder::Vertex::WEIGHTS )) == ( nMeshBuilder::Vertex::JINDICES | nMeshBuilder::Vertex::WEIGHTS ) )
    if ((sourceVC & nMeshBuilder::Vertex::JINDICES) && (sourceVC & nMeshBuilder::Vertex::WEIGHTS ))
    {
        nSkinPartitioner skinPartitioner;
        destMeshBuilder = n_new nMeshBuilder();
        skinPartitioner.PartitionMesh(*meshBuilder, *destMeshBuilder, this->task->maxJointPaletteSize);
        
        const int numPartitions = skinPartitioner.GetNumPartitions();
        if (numPartitions/* > 1*/) //only do this if the mesh was splitted in more than one partition
        {
            //store the new groups in the meshObject
            meshObject.groupIDs.Clear();
            const nArray<int> &groupMappingArray = skinPartitioner.GetGroupMappingArray();
            const int numGroupMappings = groupMappingArray.Size();

            int i; // to resolve redefinition error in vc6.

            for (i = 0; i < numGroupMappings; i++)
            {
                meshObject.groupIDs.Append(i);
            }

            //store the JointPalettes in the meshObject
            for (i = 0; i < numPartitions; i++)
            {
                meshObject.jointPalettes.Append(skinPartitioner.GetJointPalette(i));
            }

            //clean the data
            n_delete meshBuilder;

            //setup the destMesh as the used mesh
            meshBuilder = destMeshBuilder;
        }
        else
        {
            n_delete destMeshBuilder;
        }
    }
    
    //try to find a vertexpool with the same VC layout
    const int numPools = this->meshPool.Size();
    int poolIndex = 0;
    bool found = false;

    while (!found && poolIndex < numPools)
    {
        PoolEntry &entry = this->meshPool[poolIndex];
        const int destVC    = entry.meshBuilder->GetVertexAt(0).GetComponentMask();
        const int destUsage = entry.meshBuilder->GetTriangleAt(0).GetUsageFlags();
        if (destVC == sourceVC && sourceUsage == destUsage)
        {
            found = true;
        }
        else
        {
            poolIndex++;
        }
    }

    if(found)
    {
        PoolEntry &entry = this->meshPool[poolIndex];

        //append the meshbuilder
        const int baseGroupIndex = entry.meshBuilder->Append(*meshBuilder);

        //cleanup the data
        n_delete meshBuilder;
        
        //update the groupIDs -- WARNING: when something in nMeshBuilder::Append() is changed this will maybe fail!
        const int numGroupIDs = meshObject.groupIDs.Size();
        for (int i = 0; i < numGroupIDs; i++)
        {
            meshObject.groupIDs.Set(i, meshObject.groupIDs[i] + baseGroupIndex);
        }
        entry.meshObjects.Append(meshObject);

        //set the animBuilder and boneIDs for current mesh
        if (skinned)
        {
            entry.skinned = true;
            entry.animBuilder = animBuilder;
            entry.boneIDs = this->boneIDs;

            // clean bones id
            this->boneIDs.Reset();
        }
        else //without animation
        {
            entry.skinned = false;
            entry.animBuilder = NULL;
        }
    }
    else
    {
        //create a new pool entry for this mesh type
        PoolEntry entry;
        entry.meshBuilder = meshBuilder;
        entry.meshObjects.Append(meshObject);
        
        //FIXME: use the dynVertexBuffer naming sheme for the naming of the meshFile.
        nPathString fileName = this->task->meshFileName;
		fileName.AppendInt(sourceVC);
		fileName.AppendInt(sourceUsage);
        fileName += this->task->meshFileExtension;

        entry.meshFileName = fileName;

        //set the animBuilder and boneIDs for current mesh
        if (skinned)
        {
            entry.skinned = true;
            entry.animBuilder = animBuilder;
            entry.boneIDs = this->boneIDs;

            // clean bones id
            this->boneIDs.Reset();
        }
        else //without animation
        {
            entry.skinned = false;
            entry.animBuilder = NULL;
        }

        this->meshPool.Append(entry);
    }
}

//------------------------------------------------------------------------------
/**
    Write the mesh pools to files.
    Create the shapenodes and setup the groups or fragments.
    Export the materials.
*/
void
nMaxExport::storeDataPools()
{  
    const int numPools = this->meshPool.Size();

    //count the verticies in the meshpools for a nice progress bar.
    this->progressNumTotal = 0;
    this->progressCount = 0;

    int poolIndex; // to resolve redifinition error in vc6.

    for (poolIndex = 0; poolIndex < numPools; poolIndex++)
    {
        const PoolEntry &entry = this->meshPool[poolIndex];
        this->progressNumTotal += entry.meshBuilder->GetNumVertices();
		//entry.meshBuilder->Cleanup(NULL);
    }

    //update statistics
    this->progressOnePercent = 100.0f / this->progressNumTotal;

    for (poolIndex = 0; poolIndex < numPools; poolIndex++)
    {
        const PoolEntry &entry = this->meshPool[poolIndex];
       
        this->progressUpdate(0, "flip UVs");
        //flip uv's
        entry.meshBuilder->FlipUvs();

        //generate tangents
        if (entry.meshBuilder->HasVertexComponent(nMeshBuilder::Vertex::NORMAL))
        {
            this->progressUpdate(25, "triangele tangents");
            entry.meshBuilder->BuildTriangleTangents();
        }
        else
        {
            this->progressUpdate(25, "triangele normals");
            entry.meshBuilder->BuildTriangleNormals();
            
            this->progressUpdate(50, "vertex normals");
            entry.meshBuilder->BuildVertexNormals();
        }
        
        this->progressUpdate(75, "vertex tangents");
        entry.meshBuilder->BuildVertexTangents();
  
        // cleanup
        //nArray< nArray<int> > collapsMap(0, 0);
		//collapsMap.SetFixedSize(entry.meshBuilder->GetNumVertices());
		//entry.meshBuilder->Cleanup(&collapsMap);

		this->progressUpdate(100, "save file");
        //store the meshfile
        if (!entry.meshBuilder->Save(nFileServer2::Instance(), (this->task->meshesPath + entry.meshFileName).Get()))
        {
            n_printf("ERROR: Meshfile '%s' could not be written!\n", entry.meshFileName.Get());
        }
        else
        {
            nArray<nMeshBuilder::Group> groupMap;
            entry.meshBuilder->BuildGroupMap(groupMap);
            
            int groupMapIndex = 0;
            const int numGroupMaps = groupMap.Size();
            
            //const bool isSkinned = (entry.meshBuilder->HasVertexComponent(nMeshBuilder::Vertex::JINDICES) && entry.meshBuilder->HasVertexComponent(nMeshBuilder::Vertex::WEIGHTS));

            //for every group in the groupMapping
            while (groupMapIndex < numGroupMaps)
            {
                const nMeshBuilder::Group &group = groupMap[groupMapIndex];

                //update statistics
                this->progressCount += (group.GetNumTriangles() * 3);
                nString msg;
                msg = "vertex ";
				msg.AppendInt(this->progressCount);
                msg += "/";
				msg.AppendInt(this->progressNumTotal);
                const int percent = (int) ((float) (this->progressCount+1) * this->progressOnePercent);
                this->progressUpdate(percent, msg);

                //find the frist group that have the same groupID as in the current meshObject
                bool found = false;
                int meshObjectIndex = 0;
                const int numMeshObjects = entry.meshObjects.Size();

                while (!found && meshObjectIndex < numMeshObjects)
                { 
                    const MeshObject &meshObject = entry.meshObjects[meshObjectIndex];
                    
                    //check the 1st group in the meshObject
                    if (meshObject.groupIDs[0] == group.GetId())
                    {
                        found = true;
                    }
                    else
                        meshObjectIndex++;
                }
                //we must have found this meshObject containing this groupID now.
                n_assert(found);

                const int groupMaterialID = groupMap[groupMapIndex].GetMaterialId();
                const MeshObject &meshObject = entry.meshObjects[meshObjectIndex];

                //DEBUG: make shure that the groups in the meshObject and the Pool are in the expected order 
                /*
                if (isSkinned)
                {
                    for (int index = 0; index < meshObject.groupIDs.Size(); index++)
                    {
                        n_assert(groupMap[groupMapIndex + index].GetId() ==  meshObject.groupIDs[index]);
                        n_assert(groupMap[groupMapIndex + index].GetMaterialId() == groupMaterialID);
                    }
                }
                */
                
                //compose the node name
                nString nodeName;
                if (meshObject.nNode)
                {
                    nodeName = meshObject.nNodeName;
                }
                else
                {
                    nodeName = this->nohBase;
                }
                nodeName += "/";
                nodeName += this->checkChars(this->materials[groupMaterialID].igMaterial->GetMaterialName());

                //create the skinshapenode or shapenode for this mesh part
                nShapeNode* shapeNode = 0;
                if (entry.skinned)
                {
                    nSkinShapeNode* skinShapeNode = static_cast<nSkinShapeNode*>(nKernelServer::Instance()->New("nskinshapenode", nodeName.Get()));
                    n_assert(skinShapeNode);
                    
                    //convert the skinshape node for later usage
                    shapeNode = static_cast<nShapeNode*>(skinShapeNode);

                    const int numFragments = meshObject.groupIDs.Size();
                    n_assert(numFragments == meshObject.jointPalettes.Size());

					// set path to skin animator
					nString skinAnimPath;
					skinAnimPath = "../";
					skinAnimPath += "skinanimator";
					skinShapeNode->SetSkinAnimator(skinAnimPath.Get());

					//store framents & joint palettes
                    skinShapeNode->BeginFragments(numFragments);
                    for (int index = 0; index < numFragments; index++)
                    {
                        skinShapeNode->SetFragGroupIndex(index, groupMapIndex + index);
                        const int numJoints = meshObject.jointPalettes[index].Size();
                        
                        skinShapeNode->BeginJointPalette(index, numJoints);
                        for (int jointIndex = 0; jointIndex < numJoints; jointIndex++)
                        {
                            skinShapeNode->SetJointIndex(index, jointIndex, meshObject.jointPalettes[index][jointIndex]);
                        }
                        skinShapeNode->EndJointPalette(index);
                    }
                    skinShapeNode->EndFragments();
                    
                    //store group index
                    shapeNode->SetGroupIndex(groupMapIndex);
                    groupMapIndex += numFragments; //increment the current index into the groupMapping by all the handled fragments
                }
                else
                {
                    shapeNode = static_cast<nShapeNode*>(nKernelServer::Instance()->New("nshapenode", nodeName.Get()));
                    n_assert(shapeNode);

                    //store group index
                    shapeNode->SetGroupIndex(groupMapIndex);
                    groupMapIndex++; //next group
                }

                //???
                vector3 v = entry.meshBuilder->GetBBox().size();
                float s = max(v.x, max(v.y, v.z));
                if (s != 0.0)
                {
                    s = 1.0/s;
                    shapeNode->SetScale(vector3(s, s, s));
                }

                //set meshfile
                shapeNode->SetMesh((this->task->meshesPath + entry.meshFileName).Get());
                
                //export material setup
                this->exportMaterial(static_cast<nMaterialNode*>(shapeNode), nodeName, this->materials[groupMaterialID].igMaterial, entry.skinned);
            }

            //skinanimator
            if (entry.skinned)
            {
                nString animFileName(this->task->animsPath);
                animFileName += entry.meshObjects[0].igNode->GetName(); //???
                animFileName += ".nanim2"; //should be choosen in the dialog box

                entry.animBuilder->Optimize();
                if (entry.animBuilder->Save(nFileServer2::Instance(), animFileName.Get()))
                {
                    IGameNode *igBone, *igParentBone;
                    nString animPath(entry.meshObjects[0].nNodeName);
                    animPath += "/skinanimator";

                    nSkinAnimator *skinAnim = (nSkinAnimator*) nKernelServer::Instance()->New("nskinanimator", animPath.Get());
                    skinAnim->SetChannel("time");
                    skinAnim->SetLoopType(nAnimator::Loop);
                    skinAnim->SetAnim(animFileName.Get());
                    
	                int i, boneNum;
                    boneNum = entry.boneIDs.Size();

                    skinAnim->BeginJoints(boneNum);
                    for (i = 0; i < boneNum; i++)
	                {
		                igBone = this->iGameScene->GetIGameNode(entry.boneIDs[i]);

                        int parentBone;
                        igParentBone = igBone->GetNodeParent();

                        if (igParentBone)
                        {
                            n_printf("Pair %d: %d -> %d (%s -> %s)", i,
                                igBone->GetNodeID(), igParentBone->GetNodeID(),
                                igBone->GetName(), igParentBone->GetName());
                        }
                        else
                        {
                            n_printf("Pair %d: %d -> -1 (%s)", i,
                                igBone->GetNodeID(), igBone->GetName());
                        }

                        if (!igParentBone)
                        {
                            n_printf("Parent bone ID = %d", igBone->GetNodeID());
                            parentBone = -1;
                        }
                        else
                        {
                            parentBone = entry.boneIDs.FindIndex(igParentBone->GetNodeID());
                            n_assert(parentBone != -1);
                        }

                        vector4 p;
                        quaternion r;
                        vector4 s;
                        this->GetPRS(igBone, 0, p, r, s, 1);

                        if (s.x == 0.0) s.x = 1.0;
                        if (s.y == 0.0) s.y = 1.0;
                        if (s.z == 0.0) s.z = 1.0;
                        skinAnim->SetJoint(i, parentBone,
                            vector3(p.x, p.y, p.z), r, vector3(s.x, s.y, s.z));
	                }
                    skinAnim->EndJoints();

                    skinAnim->SetStateChannel("charState");

                    //TODO: more animation states
                    skinAnim->BeginStates(1);
                        skinAnim->SetState(0, 0, 0.3);
                        skinAnim->BeginClips(0,1);
                            skinAnim->SetClip(0, 0, "one");
                        skinAnim->EndClips(0);
                    skinAnim->EndStates();

                    //clean up data
                    n_delete this->meshPool[poolIndex].animBuilder;
                    this->meshPool[poolIndex].animBuilder = 0;
                }
            }
            
            //clean up data
            n_delete this->meshPool[poolIndex].meshBuilder;
            this->meshPool[poolIndex].meshBuilder = 0;
        }
    }
    this->meshPool.Clear();
}

//------------------------------------------------------------------------------
/**
    - init/get the IGameMesh
    - get all used IGameMaterials
    - for each used material
        - look in the global material pool if this already is known
            - if not add to material pool
        - export the faces that use this material
        - apply transform (if present)
        - append the resulting meshBuilder to the pool and update the groupID's

    @param igNode       the node to export
    @param nNode        the nebula node that is root for this mesh
    @param nodeName     the NOH path of the root node
    @param transform    a possible transform that should be applied to the meshBuilder after data gathering from max
*/
void
nMaxExport::exportMesh(IGameNode* igNode, /*nSceneNode* nNode,*/ const nString nodeName/*, matrix44* transform*/)
{
    n_assert(igNode);

    //get the IGameMesh
    IGameObject *igObject = igNode->GetIGameObject();
    n_assert(igObject && igObject->GetIGameType() == IGameObject::IGAME_MESH);
    IGameMesh *igMesh = static_cast<IGameMesh*>(igObject);
    bool skinned;

    //create a transformnode as root node for for the material shapennodes the mesh will use
    nTransformNode* nNode = static_cast<nTransformNode*>(nKernelServer::Instance()->New("ntransformnode", nodeName.Get()));

    //setup mesh for data init
    if (this->task->UseWeightedNormals())
    {
        igMesh->SetUseWeightedNormals();
    }
        
    //init IGame data
    if (igMesh->InitializeData())
    {
        //create a new MeshObject
        MeshObject meshObject;                           
        meshObject.igNode = igNode;
        meshObject.nNode = nNode;
        meshObject.nNodeName = nodeName;
        
        if (this->task->ExportStatic() || !igMesh->IsObjectSkinned())
        {
            skinned = false;
        }
        else
        {
            skinned = true;

            this->findBones(igMesh->GetIGameSkin(), igMesh->GetNumberOfVerts());
        }


        nMeshBuilder *meshBuilder = n_new nMeshBuilder;
        nAnimBuilder *animBuilder = NULL;
        
        //get all used material ID's
        Tab<int> objectMatIDs = igMesh->GetActiveMatIDs();
        const int numObjectMat = objectMatIDs.Count();
        
        for (int materialIndex = 0; materialIndex < numObjectMat; materialIndex++)
        {
            //get the pointer to the material for the current MatID
			Tab<FaceEx*> matFaces = igMesh->GetFacesFromMatID(objectMatIDs[materialIndex]);
            if (matFaces.Count() > 0)
            {
                IGameMaterial* igMaterial = igMesh->GetMaterialFromFace(matFaces[0]);
                if (igMaterial)
                {
                    //check if the material is already in the materials array
                    const int numMaterials = this->materials.Size();
                    int materialIndex = 0;
                    bool found = false;
                    for (; materialIndex < numMaterials && !found; materialIndex++)
                    {
                        if (igMaterial == this->materials[materialIndex].igMaterial)
                        {
                            //the material is already known
                            found = true;
                            
                            //export the faces
                            this->exportFaces(matFaces, materialIndex, meshObject, meshBuilder, skinned);
                        }
                    }
                        
                    //if material is not already in the materials array
                    if (!found)
                    {
                        //create a new Material
                        Material newMaterial;
                        newMaterial.igMaterial = igMaterial;
                        //store the material
                        this->materials.Append(newMaterial);
                        
                        //export the faces
                        this->exportFaces(matFaces, numMaterials, meshObject, meshBuilder, skinned);
                    }
                }
                else
                {
                    n_printf("'%s': Material '%i' is not valid!.\n", igNode->GetName(), materialIndex);
                }
            }
            else
            {
                n_printf("'%s': don't get faces for MatID: %i.\n", igNode->GetName(), objectMatIDs[materialIndex]);
            }
        }
        
        if (meshBuilder->GetNumTriangles() > 0 && meshBuilder->GetNumVertices() > 0)
        {
            if (skinned)
            {
                IGameNode *igBone, *igParentBone;
                int i, boneNum;
                boneNum = this->boneIDs.Size();

                n_printf("Bones num: %d", this->boneIDs.Size());

                animBuilder = n_new nAnimBuilder;

                // export animation
                this->exportSkinnedAnim(igNode, nodeName, animBuilder);
            }

            //setup the group used in this meshObject
            const int groupID = this->task->groupMeshBySourceObject ? 1 : 0;
            meshObject.groupIDs.Append(groupID);
            
            this->appendDataToPool(meshObject, meshBuilder, animBuilder, skinned);
        }
    }
}


//------------------------------------------------------------------------------
/**
    
    @param matFaces     all faces of the mesh with the same material
    @param matID        the material ID used for nebula (the index into the materials pool)
    @param meshObject   the meshObject to store the groupID's
    @param meshBuilder  the meshBuilder to store the faces/vertex data
*/
void
nMaxExport::exportFaces(Tab<FaceEx*> matFaces, const int matID, MeshObject &meshObject, nMeshBuilder* meshBuilder, bool skinned)
{  
    n_assert(meshBuilder);
    const int numFaces = matFaces.Count();
    
    //FIXME: broken code: nMeshBuilder don't search the highest group id on attach, it simple count the current number of groups!!
    /*
    When we use group by source object the new group ID is 1.
        So the new group id, when append the mesh, will be highestGoupID + 1;
    When we don't use group by source object the new group ID is 0.
        So the new group id, when append the mesh, will be highestGoupID + 0 = higestGroupID (what's in this case always 0)
    */
    const int groupID = this->task->groupMeshBySourceObject ? 1 : 0;
    

    //convert to IGameMesh
    IGameNode* igNode = meshObject.igNode;
    n_assert(igNode);

    IGameObject* igObject = igNode->GetIGameObject();
    n_assert(igObject);
    n_assert(igObject->GetIGameType() == IGameObject::IGAME_MESH);
    
    IGameMesh* igMesh = static_cast<IGameMesh*>(igObject);

    const int vertexCount = meshBuilder->GetNumVertices();

    if (numFaces > 0)
    {                                       
        int indexCount = 0;

        //export faces
        for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            FaceEx* &igFace = matFaces[faceIndex];
            n_assert(igFace);

            //n_printf("Face %d, flags %d:", igFace->meshFaceIndex, igFace->flags);
        
            nMeshBuilder::Triangle face;
            
            face.SetVertexIndices(vertexCount + indexCount, vertexCount + indexCount+1, vertexCount + indexCount+2);
            indexCount+=3;
            
            //set the material id of the face
            face.SetMaterialId(matID);
            face.SetGroupId(groupID);
            
            //export indicies - per face
            for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
            {
                nMeshBuilder::Vertex vertex;
				int faceVertex;
                //coord
                Point3 coord;

				faceVertex = igFace->vert[vertexIndex];

                //n_printf("\tVert %d:", faceVertex);

                if (igMesh->GetVertex(faceVertex,coord))
                {
                    vector3 vector(coord.x, coord.y, coord.z);
                    vertex.SetCoord(vector);
                }
            
                //normal
                /** FIXME:
                    When a object was mirrored in Max the Normals are exported
                    with a wrong direction from IGame.
                */
                Point3 normal;
                if (igMesh->GetNormal(igFace->norm[vertexIndex], normal))
                {
                    vector3 vector(normal.x, normal.y, normal.z);
                    vertex.SetNormal(vector);
                }
                
                //uv
                Tab<int> mapChannels = igMesh->GetActiveMapChannelNum();
                if (mapChannels.Count() > nMeshBuilder::Vertex::MAX_TEXTURE_LAYERS)
                {
                    n_printf("Warning: mesh: %s vertex: %i more uvChannels(%i) used than exported(%i).\n",
                        igNode->GetName(), vertexIndex, mapChannels.Count(), nMeshBuilder::Vertex::MAX_TEXTURE_LAYERS);
                }
                
                //access to max to get uvw source type of the mapChannel
                IGameMaterial *igMaterial = igMesh->GetMaterialFromFace(igFace);
                if (igMaterial)
                {
                    int channelsNum = min(mapChannels.Count(), nMeshBuilder::Vertex::MAX_TEXTURE_LAYERS);
                    int uvLayer;

                    //get the indicies into the mapChannel for this face
                    Mtl* mtl = igMaterial->GetMaxMaterial();

                    for (uvLayer = 0; uvLayer < channelsNum; uvLayer++)
                    {
                        IGameTextureMap *igTextureMap;
                        IGameUVGen *igUVGen = NULL;
                        Point3 mapCoord;
                        DWORD mapIndicies[3];
                        UVGen* uvGen = NULL;

                        if (igMesh->GetMapFaceIndex(mapChannels[uvLayer], igFace->meshFaceIndex, &mapIndicies[0]))
                        {
                            igMesh->GetMapVertex(mapChannels[uvLayer], mapIndicies[vertexIndex], mapCoord);
                            if (igFace->texCoord[0] != mapIndicies[0] ||
                                igFace->texCoord[1] != mapIndicies[1] ||
                                igFace->texCoord[2] != mapIndicies[2])
                            {
                                n_printf("\t\tTex&Map diff: t[%d %d %d] m[%d %d %d]",
                                    igFace->texCoord[0], igFace->texCoord[1], igFace->texCoord[2],
                                    mapIndicies[0], mapIndicies[1], mapIndicies[2]);
                            }
                        }

                        //find the 1st texmap that uses this map channel
                        //  WARNING: if you use the same map channel for more than one texture, but
                        //  use diffent UVWSouces the export will do a false result.

                        int texMapsNum = igMaterial->GetNumberOfTextureMaps();
                        int texMap;
                        for (texMap = 0; texMap < mtl->NumSubTexmaps() && uvGen == NULL; texMap++)
                        {
                            igTextureMap = igMaterial->GetIGameTextureMap(texMap);
                            Texmap* tex = igTextureMap->GetMaxTexmap();
                            if (tex)
                            {
                                switch (tex->GetUVWSource())
	                            {
		                            case UVWSRC_EXPLICIT:
                                        //n_printf("%d: UVWSRC_EXPLICIT", vertexIndex);
			                            if (mapChannels[uvLayer] == tex->GetMapChannel())
                                        {
			                                uvGen = tex->GetTheUVGen();
                                            igUVGen = igTextureMap->GetIGameUVGen();
                                        }
			                            break;
		                            case UVWSRC_EXPLICIT2:
                                        //n_printf("%d: UVWSRC_EXPLICIT2", vertexIndex);
			                            if (mapChannels[uvLayer] == 0)  //the vertexcolorchannel
                                        {
			                                uvGen = tex->GetTheUVGen();
                                            igUVGen = igTextureMap->GetIGameUVGen();
                                        }
			                            break;
		                            //Generate planar UVW mapping coordinates from the
                                    //object local XYZ on-the-fly.
                                    case UVWSRC_OBJXYZ:
                                        //n_printf("%d: UVWSRC_OBJXYZ", vertexIndex);
                                        break;
                                    //Generate planar UVW mapping coordinates from the
                                    //world XYZ on-the-fly. This corresponds to the
                                    //"Planar From World XYZ" option. Note: this value
                                    //used for the UVW is the world XYZ, taken directly,
                                    //with out normalization to the objects bounding box.
                                    //This differs from "Planar from Object XYZ", where the
                                    //values are normalized to the object's bounding box.
                                    case UVWSRC_WORLDXYZ:
                                        //n_printf("%d: UVWSRC_WORLDXYZ", vertexIndex);
			                            break;
	                            }
	                        }    
                        }
                        
                        //choose axis
                        vector2 vector;                        
                        if (uvGen)
                        {
                            switch (uvGen->GetAxis())
                            {
				                case AXIS_UV:
                                    //n_printf("%d: AXIS_UV", vertexIndex);
					                vector.set(mapCoord.x,mapCoord.y);
					                break;
				                case AXIS_VW:
                                    //n_printf("%d: AXIS_VW", vertexIndex);
					                vector.set(mapCoord.y,mapCoord.z);
					                break;
				                case AXIS_WU:
                                    //n_printf("%d: AXIS_WU", vertexIndex);
					                vector.set(mapCoord.z,mapCoord.x);
					                break;
                            }
                        }
                        else
                        {
                            //default xy
                            vector.set(mapCoord.x, mapCoord.y);
                        }

                        vertex.SetUv(uvLayer, vector);    
                    }
                }

                //color
                Point3 color;
                float alpha; 
                if (igMesh->GetColorVertex(igFace->color[vertexIndex],color) && 
                    igMesh->GetAlphaVertex(igFace->alpha[vertexIndex],alpha))
                {
                    vector4 vector(color.x, color.y, color.z, alpha);
                    vertex.SetColor(vector);
                }
                //weights and jindices
                if (!skinned)
                {
                    face.SetUsageFlags(nMesh2::WriteOnce);
                }
                else
                {
                    face.SetUsageFlags(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);

                    IGameSkin* igSkin = igMesh->GetIGameSkin();
                    n_assert(igSkin);

                    IGameSkin::VertexType vertexType;
					vertexType = igSkin->GetVertexType(faceVertex);
					/*if (vertexType != IGameSkin::IGAME_RIGID)
                    {
						switch(vertexType)
						{
						case IGameSkin::IGAME_RIGID_BLENDED:
							n_printf("nMaxExport::exportFaces(): IGAME_RIGID_BLENDED is not supported, only RIGID skins are supported!\n");
							break;
						case IGameSkin::IGAME_UNKNOWN:
							n_printf("nMaxExport::exportFaces(): IGAME_UNKNOWN, only RIGID skins are supported!\n");
							break;
						}
                    }*/
					if (vertexType == IGameSkin::IGAME_UNKNOWN)
					{
						n_printf("nMaxExport::exportFaces(): IGAME_UNKNOWN, only RIGID skins are supported!\n");
					}
                    else /*if (igSkin->GetSkinType() != IGameSkin::IGAME_SKIN)
					{
                        if (igSkin->GetSkinType() == IGameSkin::IGAME_PHYSIQUE)
                        {
						    n_printf("nMaxExport::exportFaces(): IGAME_PHYSIQUE skining is not supported!\n");
                        }
                        else
                        {
						    n_printf("nMaxExport::exportFaces(): Unknown skinning (%d). Only IGAME_SKIN skining are supported!\n", igSkin->GetSkinType());
                        }
					}
					else*/ if (igMesh->GetNumberOfVerts() != igSkin->GetNumOfSkinnedVerts())
					{
                        /*n_printf("nMaxExport::exportFaces(): Mesh vertices(%d) and skin vertices(%d) does not match!",
                            igMesh->GetNumberOfVerts(), igSkin->GetNumOfSkinnedVerts());*/
					}
                    else
                    {                        
                        int i, j, k; // to resolve redefinition error in vc6.
						float *weights, wt;
						int *jindices, *jiNum, ji, jiCount;
                        int numBones = igSkin->GetNumberOfBones(faceVertex), realNumBones;

                        //create at least MAX_NUM_BONES weights and jindices elements
                        realNumBones = max(numBones, MAX_NUM_BONES);
                        weights = new float[realNumBones];
                        jindices = new int[realNumBones];
                        jiNum = new int[realNumBones];

                        //set default values
                        for (i = 0; i < realNumBones; i++)
                        {
                            weights[i] = 0.0;
                            jindices[i] = -1;
                            jiNum[i] = 0;
                        }

                        jiCount = 0;

                        // read bones and sort by weights
                        for (i = 0; i < numBones; i++)
                        {
                            wt = igSkin->GetWeight(faceVertex, i);
                           	
                            // 3ds max weird behaviour: zero out epsilon weights
                            //if(wt < 0.0005f) wt = 0.0f;

							ji = this->GetBoneByID(igSkin->GetBoneID(faceVertex, i), false);
                            n_assert(ji != -1);

                            bool inserted = false;
                            // sorted insert
                            for (j = 0; j < jiCount; j++)
                            {
                                //we already have this bone id
                                if (jindices[j] == ji)
                                {
                                    weights[j] += wt;
                                    jiNum[j]++;
                                    inserted = true;
                                    break;
                                }
                                else if (wt > weights[j]) //new bone with greater weight
                                {
                                    //shift the tail of array
                                    for (k = jiCount; k > j; k--)
                                    {
                                        weights[k] = weights[k - 1];
                                        jindices[k] = jindices[k - 1];
                                    }
                                    
                                    //insert new bone
                                    weights[j] = wt;
                                    jindices[j] = ji;
                                    jiNum[j] = 1;
                                    jiCount++;
                                    inserted = true;
                                    break;
                                }
                            }
                            
                            //set bone as last element
                            if (!inserted)
                            {
                                weights[jiCount] = wt;
                                jindices[jiCount] = ji;
                                jiNum[jiCount] = 1;
                                jiCount++;
                            }
                        }

                        //
                        for (i = 0; i < jiCount; i++)
                        {
                            weights[i] /= (float)jiNum[i];
                        }

                        //use only MAX_NUM_BONES elements or less
                        if (jiCount > MAX_NUM_BONES)
                        {
							//n_printf("Warning: %s: More than 4 Bones affect vertex: v(%i) b(%d). This is not supported!\n", igNode->GetName(), faceVertex, numBones);
                            jiCount = MAX_NUM_BONES;
                        }
                        else if (jiCount < MAX_NUM_BONES) // fill not used weights and jindices
                        {
                            for (i = jiCount; i < MAX_NUM_BONES; i++)
                            {
                                weights[i] = 0.0;
                                jindices[i] = 0;
                            }
                        }

                        float sum = 0.0f;
                        for (i = 0; i < jiCount; i++)
                        {
							sum += weights[i];
                        }
                            
                        //renormal the weights up to 1.0f
                        float coeff = 0.0;
						if (sum != 0.0)
                        {
							coeff = 1.0 / sum;
                        }

                        for (i = 0; i < jiCount; i++)
                        {
                            weights[i] *= coeff;
                        }
                            
                        vector4 vector;
                        vector.set(weights[0], weights[1], weights[2], weights[3]);
                        vertex.SetWeights(vector);
                            
                        vector.set((float)jindices[0], (float)jindices[1], (float)jindices[2], (float)jindices[3]);
                        vertex.SetJointIndices(vector);

                        delete[] weights;
                        delete[] jindices;
                        delete[] jiNum;
                    }
                }                        

                meshBuilder->AddVertex(vertex);    
            }
            meshBuilder->AddTriangle(face);
        }
    }
    //don't relase the IGameMesh/IGameNode here because the data must stay initiated until all materials of this mesh are exported
}

