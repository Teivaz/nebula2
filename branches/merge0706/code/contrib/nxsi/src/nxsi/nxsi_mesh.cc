//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
// nXSI mesh handling functions
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <scene/nskinanimator.h>
#include <scene/nskinshapenode.h>
#include <scene/nshapenode.h>
#include <mathlib/quaternion.h>
#include <kernel/nfileserver2.h>

//-----------------------------------------------------------------------------

void nXSI::HandleSIMesh(CSLMesh* mesh)
{
    CSLModel* templ = mesh->ParentModel();
    nString skinName(templ->Name().GetText());
    nString animName(templ->Name().GetText());
    bool hasAnimator = false;
    bool isMultiMesh = false;
    bool isSkinned  = false;
    int groupCount  = 0;
    int groupId;
    int i;

    // get filename
    nString meshFilename(this->options.GetMeshFilename());
    if (!(this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL))
    {
        meshFilename = mesh->Name().GetText();
        if (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_BINARY)
        {
            meshFilename.Append(".nvx2");
        }
        else
        {
            meshFilename.Append(".n3d2");
        }
    }

    // get transform information
    vector3 position = (vector3&)templ->Transform()->GetTranslation();
    vector3 rotation = (vector3&)templ->Transform()->GetEulerRotation();
    vector3 scale    = (vector3&)templ->Transform()->GetScale();
    VECTOR3_DEG2RAD(rotation);

    // get shape
    CSLShape* shape = (CSLShape*)mesh->Shape();
    nArray<nXSIWeight> weightList;

    // destroy FTK converted triangle lists
    if (mesh->GetTriangleStripListCount() > 0)
    {
        mesh->ClearTriangleLists();
    }

    // convert triangle_strip_lists to triangle_lists
    CSLTriangleStripList** stripLists = mesh->TriangleStripLists();
    for (i = 0; i < mesh->GetTriangleStripListCount(); i++)
    {
        this->ConvertSITriangleStripList(stripLists[i]);
    }

    // convert polygon_lists to triangle_lists
    CSLPolygonList** polygonLists = mesh->PolygonLists();
    for (i = 0; i < mesh->GetPolygonListCount(); i++)
    {
        this->ConvertSIPolygonList(polygonLists[i]);
    }

    // check if multi mesh
    if (mesh->GetTriangleListCount() > 1)
    {
        isMultiMesh = true;

        nTransformNode* newNode = (nTransformNode*)this->kernelServer.New("ntransformnode", mesh->Name().GetText());
        this->kernelServer.PushCwd(newNode);
        newNode->SetPosition(position);
        newNode->SetEuler(rotation);
        newNode->SetScale(scale);

        // build transform animation
        if (this->BuildTransformAnimation(templ->Transform(), animName))
        {
            newNode->AddAnimator(animName.Get());
        }
    }

    // check if skinned mesh
    if (templ->GetEnvelopeCount() > 0)
    {
        isSkinned = true;
        this->HandleSIMeshSkeleton(mesh, skinName, weightList);
    }

    // create mesh parts
    CSLTriangleList** triangleLists = mesh->TriangleLists();
    int triangleListCount = mesh->GetTriangleListCount();
    if (triangleListCount > 0)
    {
        // read parts
        for (i = 0, groupId = this->meshGroupId; i < triangleListCount; i++, groupId++, groupCount++)
        {
            this->HandleSITriangleList(triangleLists[i], shape, weightList, groupId);
        }

        // if skinned, repartition mesh
        if (isSkinned) {
            nMeshBuilder newMesh;
            if (this->skinPartioner.PartitionMesh(this->meshBuilder, newMesh, 32))
            {
//              group_count = m_skin_partioner.GetNumPartitions();
                this->meshBuilder = newMesh;
            }
            else
            {
                n_printf("WARNING: repartitioning failed (%s)\n", mesh->Name().GetText());
            }
        }

        // create parts
        for (i = 0, groupId = this->meshGroupId; i < groupCount; i++, groupId++)
        {
            // create part name
            nString meshName(mesh->Name().GetText());
            if (isMultiMesh)
            {
                meshName += "_";
                meshName += groupId;
            }

            // create new mesh node
            nShapeNode* newNode;
            if (isSkinned) newNode = (nShapeNode*)this->kernelServer.New("nskinshapenode", meshName.Get());
            else           newNode = (nShapeNode*)this->kernelServer.New("nshapenode", meshName.Get());

            newNode->SetLocalBox(this->meshBuilder.GetGroupBBox(groupId));
            newNode->SetMesh(meshFilename.Get());
            newNode->SetGroupIndex(groupId);

            if (isMultiMesh)
            {
                newNode->SetPosition(vector3(0.0f, 0.0f, 0.0f));
                newNode->SetEuler(vector3(0.0f, 0.0f, 0.0f));
                newNode->SetScale(vector3(1.0f, 1.0f, 1.0f));
            }
            else
            {
                this->kernelServer.PushCwd(newNode);
                newNode->SetPosition(position);
                newNode->SetEuler(rotation);
                newNode->SetScale(scale);

                // build transform animation
                if (this->BuildTransformAnimation(templ->Transform(), animName))
                {
                    newNode->AddAnimator(animName.Get());
                }
            }

            { // get material variables
                CSLBaseMaterial* baseMaterial = triangleLists[i]->GetMaterial();
                switch (baseMaterial->Type())
                {
                    case CSLTemplate::XSI_MATERIAL:
                        HandleXSIMaterialVariables((CSLXSIMaterial*)baseMaterial, newNode, isSkinned);
                        break;

                    case CSLTemplate::SI_MATERIAL:
                        HandleSIMaterialVariables((CSLMaterial*)baseMaterial, newNode, isSkinned);
                        break;

                    default:
                        n_printf("WARNING: found unknown material type.\n");
                        break;
                }
            }

            // get bone joint infos
            if (isSkinned)
            {
                nSkinShapeNode* newSkinNode = (nSkinShapeNode*)newNode;
                int partCount = this->skinPartioner.GetNumPartitions();

                newSkinNode->SetSkinAnimator(("../" + skinName).Get());
                newSkinNode->BeginFragments(partCount);

                for (int p = 0; p < partCount; p++)
                {
                    const nArray<int>& jointList = this->skinPartioner.GetJointPalette(p);

                    newSkinNode->SetFragGroupIndex(p, groupId);
                    newSkinNode->BeginJointPalette(p, jointList.Size());

                    for (int j = 0; j < jointList.Size(); j++)
                    {
                        newSkinNode->SetJointIndex(p, j, jointList[j]);
                    }
                    newSkinNode->EndJointPalette(p);
                }
                newSkinNode->EndFragments();
            }
        }

        // update group id if merging
        if (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL)
        {
            this->meshGroupId += groupCount;
        }

        // save mesh object
        if ((this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MESH) &&
            !(this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL))
        {
            this->meshBuilder.BuildTriangleTangents();
            this->meshBuilder.BuildVertexTangents();
            this->meshBuilder.Cleanup(0);
            this->meshBuilder.Optimize();
            this->meshBuilder.Save(nFileServer2::Instance(), meshFilename.Get());
            this->meshBuilder.Clear();
            n_printf("mesh saved: %s\n", meshFilename.Get());
        }

        // handle child models
        CSLModel* *childList = templ->GetChildrenList();
        for (int i = 0; i < templ->GetChildrenCount(); i++) {
            HandleSIModel(childList[i]);
        }

        this->kernelServer.PopCwd();
    }
}

void RemapBones(int boneId, const nArray<int>& boneParentList, int& remapId, nArray<int>& boneRemapList)
{
    int count = boneParentList.Size();

    boneRemapList[remapId] = boneId;
    remapId++;

    for (int i = 0; i < count; i++)
    {
        if (boneParentList[i] == boneId)
        {
            RemapBones(i, boneParentList, remapId, boneRemapList);
        }
    }
}

void nXSI::HandleSIMeshSkeleton(CSLMesh* mesh, nString& skinName, nArray<nXSIWeight>& weightList)
{
    CSLShape* shape = (CSLShape*)mesh->Shape();
    CSLModel* templ = mesh->ParentModel();
    nString animFilename;
    int vertexCount = shape->GetVertexCount();
    int i, j, b, w;

    // set name
    skinName.Append("_animator");
    animFilename = this->options.GetAnimFilename().Get();
    if (!(this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL))
    {
        animFilename = mesh->Name().GetText();
        if (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_BINARY)
        {
            animFilename.Append(".nax2");
        }
        else
        {
            animFilename.Append(".nanim2");
        }
    }

    // get envelope infos
    CSLEnvelope** envelopeList = templ->GetEnvelopeList();
    int envelopeCount = templ->GetEnvelopeCount();

    // create envelope remap list
    nArray<int> envelopeRemapList;
    envelopeRemapList.SetFixedSize(envelopeCount);

    // build skeleton
    {
        nArray<int> boneParentList;
        nArray<CSLModel*> boneList;
        boneParentList.SetFixedSize(envelopeCount);
        boneList.SetFixedSize(envelopeCount);

        // fill bone list
        for (i = 0; i < envelopeCount; i++)
        {
            boneList[i] = envelopeList[i]->GetDeformer();
        }

        // fill parent list
        for (i = 0; i < envelopeCount; i++)
        {
            boneParentList[i] = -1;
            for (j = 0; j < envelopeCount; j++)
            {
                if (boneList[i]->ParentModel() == boneList[j]) {
                    boneParentList[i] = j;
                }
            }
        }

        // create sorted list
        for (i = 0, b = 0; i < envelopeCount; i++)
        {
            if (boneParentList[i] == -1)
            {
                RemapBones(i, boneParentList, b, envelopeRemapList);
                break;
            }
        }
        envelopeCount = b;

        // remap bone list
        for (i = 0; i < envelopeCount; i++)
        {
            boneList[i] = envelopeList[envelopeRemapList[i]]->GetDeformer();
        }

        // create skin animator
        nSkinAnimator* newNode = (nSkinAnimator*)this->kernelServer.New("nskinanimator", skinName.Get());
        newNode->SetChannel("time");
        newNode->SetLoopType(nAnimator::Loop);
        newNode->SetAnim(animFilename.Get());

        // add joints
        newNode->BeginJoints(envelopeCount);
        for (i = 0; i < envelopeCount; i++)
        {
            int boneId = envelopeRemapList[i];
            int parentId = -1;
            if (boneParentList[boneId] > -1) {
                parentId = envelopeRemapList[boneParentList[boneId]];
            }
            CSLModel* joint = boneList[i];

            // get joint info
            vector3 translation = (vector3&)(joint->Transform()->GetTranslation());
            vector3 eulerRotation = (vector3&)(joint->Transform()->GetEulerRotation());
            vector3 scale = (vector3&)(joint->Transform()->GetScale());
            VECTOR3_DEG2RAD(eulerRotation);

            quaternion rotation;
            rotation.set_rotate_xyz(eulerRotation.x, eulerRotation.y, eulerRotation.z);

            // set joint
            newNode->SetJoint(i, parentId, translation, rotation, scale);
            newNode->AddJointName(i, joint->Name().GetText());
        }
        newNode->EndJoints();

        // build joint_animations
        this->BuildJointAnimations(boneList, envelopeCount);

        // set states
        newNode->SetStateChannel("charState");
        newNode->BeginStates(1);
        newNode->SetState(0, this->animGroupId, 0.0f);
        newNode->BeginClips(0, 1);
        newNode->SetClip(0, 0, "one");
        newNode->EndClips(0);
        newNode->EndStates();
    }

    // create weight list
    weightList.SetFixedSize(vertexCount);

    //  clear list
    for (i = 0; i < vertexCount; i++)
    {
        weightList[i].joints[0] = 0;
        weightList[i].joints[1] = 0;
        weightList[i].joints[2] = 0;
        weightList[i].joints[3] = 0;
        weightList[i].weights[0] = 0;
        weightList[i].weights[1] = 0;
        weightList[i].weights[2] = 0;
        weightList[i].weights[3] = 0;
        weightList[i].count = 0;
    }

    // fill weight list
    for (i = 0; i < envelopeCount; i++)
    {
        CSLEnvelope* envelope = envelopeList[envelopeRemapList[i]];
        SLVertexWeight* envelopeWeights = envelope->GetVertexWeightListPtr();
        int weightCount = envelope->GetVertexWeightCount();

        // get envelope weights
        for (w = 0; w < weightCount; w++)
        {
            nXSIWeight& weight = weightList[(int)(envelopeWeights[w].m_fVertexIndex)];
            if (weight.count < 4)
            {
                weight.joints[weight.count] = (float)(i);
                weight.weights[weight.count] = envelopeWeights[w].m_fWeight / 100.f;
                weight.count++;
            }
            else
            {
                n_printf("WARNING: too much weights in one vertex\n");
            }
        }
    }

    // update group id if merging
    if (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL)
    {
        this->animGroupId++;
    }

    // save skin animation
    if ((this->options.GetOutputFlags() & nXSIOptions::OUTPUT_ANIM) &&
        !(this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL))
    {
        this->animBuilder.Optimize();
        this->animBuilder.FixKeyOffsets();
        this->animBuilder.Save(nFileServer2::Instance(), animFilename.Get());
        this->animBuilder.Clear();
        n_printf("animation saved: %s\n", animFilename.Get());
    }
}


void nXSI::HandleSITriangleList(CSLTriangleList* templ, CSLShape* shapeOld, const nArray<nXSIWeight>& weightList, uint groupId)
{
    CSLModel* parentTempl = templ->ParentModel();
    CSLShape_35* shape = (CSLShape_35*)shapeOld;
    int shapeType = shape->Type();
    int firstVertex = this->meshBuilder.GetNumVertices();
    int triangleCount = templ->GetTriangleCount();
    int vertexCount = triangleCount * 3;
    bool isSkinned = false;
    int i, j;

    // check if mesh is skinned
    if (parentTempl->GetEnvelopeCount() > 0)
    {
        isSkinned = true;
    }

    // get uvset count
    int uvsetCount = templ->GetUVArrayCount();
    if (shapeType == CSLTemplate::SI_SHAPE35)
    {
        if (uvsetCount > NXSI_MAX_UVSETS)
        {
            n_printf("WARNING: found %i uvsets. clamping into %i sets.\n", uvsetCount, NXSI_MAX_UVSETS);
            uvsetCount = NXSI_MAX_UVSETS;
        }
    }
    else if (uvsetCount > 1)
    {
        uvsetCount = 1;
    }

    if (uvsetCount == 0)
    {
        n_printf("WARNING: uv coordinates not found (%s). may result strange lighting.\n", parentTempl->Name().GetText());
    }

    // fill vertex list
    {
        int* triPositions   = templ->GetVertexIndicesPtr();
        int* triNormals     = templ->GetNormalIndicesPtr();
        int* triColors      = templ->GetColorIndicesPtr();

        int* triUvset[NXSI_MAX_UVSETS];
        for (i = 0; i < uvsetCount; i++)
        {
            triUvset[i] = templ->GetUVIndicesPtr(i);
        }

        vector3* positions  = (vector3*)(shape->GetVertexListPtr());
        vector3* normals    = (vector3*)(shape->GetNormalListPtr());
        vector4* colors     = (vector4*)shape->GetColorListPtr();

        vector2* uvset[NXSI_MAX_UVSETS];
        if (shapeType == CSLTemplate::SI_SHAPE35)
        {
            for (i = 0; i < uvsetCount; i++)
            {
                uvset[i] = (vector2*)(shape->UVCoordArrays()[i]->GetUVCoordListPtr());
            }
        }
        else
        {
            uvset[0] = (vector2*)(shapeOld->GetUVCoordListPtr());
        }

        nMeshBuilder::Vertex vertex;
        for (i = 0; i < vertexCount; i++)
        {
            vertex.SetCoord(positions[*triPositions]);
            if (isSkinned)
            {
                vertex.SetJointIndices(*((vector4*)(&weightList[*triPositions].joints)));
                vertex.SetWeights(*((vector4*)(&weightList[*triPositions].weights)));
            }
            triPositions++;

            if (normals)
            {
                vertex.SetNormal(normals[*triNormals]);
                triNormals++;
            }

            if (colors)
            {
                vertex.SetColor(colors[*triColors]);
                triColors++;
            }

            for (j = 0; j < uvsetCount; j++)
            {
                vertex.SetUv(j, uvset[j][*triUvset[j]]);
                triUvset[j]++;
            }

            this->meshBuilder.AddVertex(vertex);
        }
    }

    // fill triangle list
    nMeshBuilder::Triangle triangle;
    for (i = 0; i < triangleCount; i++)
    {
        triangle.SetVertexIndices(firstVertex + i*3, firstVertex + i*3 + 1, firstVertex + i*3 + 2);
        triangle.SetGroupId(groupId);
        this->meshBuilder.AddTriangle(triangle);
    }
}

//-----------------------------------------------------------------------------
// Eof