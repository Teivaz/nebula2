//------------------------------------------------------------------------------
//  nmeshbuilder_tangent.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#undef __NEBULA_MEM_MANAGER__

#include "tools/nmeshbuilder.h"
#include "NVMeshMender.h"

//------------------------------------------------------------------------------
/*
    Helper method: Build tangents for one triangle group. The outMesh
    nMeshBuilder object will contain one group containing the original
    group geometry, plus new per-vertex tangents. Note the the outMesh
    may contain more vertices then the original group because of the 
    way NVMeshMender works.
*/
bool
nMeshBuilder::BuildTangentsForOneGroup(int groupIndex, nMeshBuilder& outMesh)
{
    // build a new mesh builder object containing only the group data
nMeshBuilder srcMesh = *this;
    
//    nMeshBuilder srcMesh;
//    this->ExtractGroup(groupIndex, srcMesh);

    n_assert(srcMesh.HasVertexComponent(Vertex::COORD));
    n_assert(srcMesh.GetNumTriangles() > 0);

    // do the actual tangent generation work
    NVMeshMender::VAVector inAttrs;
    NVMeshMender::VAVector outAttrs;

    int vertexIndex;
    int numVertices = srcMesh.GetNumVertices();

    // add coords
    NVMeshMender::VertexAttribute posAtt;
    posAtt.Name_ = "position";
    for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        const vector3& v = srcMesh.GetVertexAt(vertexIndex).GetCoord();
        posAtt.floatVector_.push_back(v.x);
        posAtt.floatVector_.push_back(v.y);
        posAtt.floatVector_.push_back(v.z);
    }
    inAttrs.push_back(posAtt);
    outAttrs.push_back(posAtt);

    // add normals
    if (srcMesh.HasVertexComponent(Vertex::NORMAL))
    {
        NVMeshMender::VertexAttribute normAtt;
        normAtt.Name_ = "normal";
        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
        {
            const vector3& v = srcMesh.GetVertexAt(vertexIndex).GetNormal();
            normAtt.floatVector_.push_back(v.x);
            normAtt.floatVector_.push_back(v.y);
            normAtt.floatVector_.push_back(v.z);
        }
        inAttrs.push_back(normAtt);
        outAttrs.push_back(normAtt);
    }

    // add vertex weights (since MeshMender only accepts input of up to 3
    // components, we have to split the 4d weights over 2 MeshMender
    // component arrays
    if (srcMesh.HasVertexComponent(Vertex::WEIGHTS))
    {
        NVMeshMender::VertexAttribute weightAtt0;
        NVMeshMender::VertexAttribute weightAtt1;
        weightAtt0.Name_ = "weight0";
        weightAtt1.Name_ = "weight1";
        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
        {
            const vector4& v = srcMesh.GetVertexAt(vertexIndex).GetWeights();
            weightAtt0.floatVector_.push_back(v.x);
            weightAtt0.floatVector_.push_back(v.y);
            weightAtt0.floatVector_.push_back(0.0f);
            weightAtt1.floatVector_.push_back(v.z);
            weightAtt1.floatVector_.push_back(v.w);
            weightAtt1.floatVector_.push_back(0.0f);
        }
        inAttrs.push_back(weightAtt0);
        inAttrs.push_back(weightAtt1);
        outAttrs.push_back(weightAtt0);
        outAttrs.push_back(weightAtt1);
    }

    // add joint indices, same procedure as vertex weights
    if (srcMesh.HasVertexComponent(Vertex::JINDICES))
    {
        NVMeshMender::VertexAttribute jiAtt0;
        NVMeshMender::VertexAttribute jiAtt1;
        jiAtt0.Name_ = "ji0";
        jiAtt1.Name_ = "ji1";
        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
        {
            const vector4& v = srcMesh.GetVertexAt(vertexIndex).GetJointIndices();
            jiAtt0.floatVector_.push_back(v.x);
            jiAtt0.floatVector_.push_back(v.y);
            jiAtt0.floatVector_.push_back(0.0f);
            jiAtt1.floatVector_.push_back(v.z);
            jiAtt1.floatVector_.push_back(v.w);
            jiAtt1.floatVector_.push_back(0.0f);
        }
        inAttrs.push_back(jiAtt0);
        inAttrs.push_back(jiAtt1);
        outAttrs.push_back(jiAtt0);
        outAttrs.push_back(jiAtt1);
    }

    // add texture coordinate layers
    int texCoordSet;
    for (texCoordSet = 0; texCoordSet < 4; texCoordSet++)
    {
        Vertex::Component comp;
        const char* name;
        switch (texCoordSet)
        {
            case 0: comp = Vertex::UV0; name = "tex0"; break;
            case 1: comp = Vertex::UV1; name = "tex1"; break;
            case 2: comp = Vertex::UV2; name = "tex2"; break;
            case 3: comp = Vertex::UV3; name = "tex3"; break;
        }
        if (srcMesh.HasVertexComponent(comp))
        {
            NVMeshMender::VertexAttribute texAtt;
            texAtt.Name_ = name;
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                const vector2& v = srcMesh.GetVertexAt(vertexIndex).GetUv(texCoordSet);
                texAtt.floatVector_.push_back(v.x);
                texAtt.floatVector_.push_back(v.y);
                texAtt.floatVector_.push_back(0.0f);
            }
            inAttrs.push_back(texAtt);
            outAttrs.push_back(texAtt);
        }
    }

    // add indices
    int numTriangles = srcMesh.GetNumTriangles();
    NVMeshMender::VertexAttribute indicesAtt;
    indicesAtt.Name_ = "indices";
    int triangleIndex;
    for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        int i0, i1, i2;
        srcMesh.GetTriangleAt(triangleIndex).GetVertexIndices(i0, i1, i2);
        indicesAtt.intVector_.push_back(i0);
        indicesAtt.intVector_.push_back(i1);
        indicesAtt.intVector_.push_back(i2);
    }
    inAttrs.push_back(indicesAtt);
    outAttrs.push_back(indicesAtt);

    // set requested output attributes
    NVMeshMender::VertexAttribute tangentAtt;
    tangentAtt.Name_ = "tangent";
    outAttrs.push_back(tangentAtt);

    NVMeshMender::VertexAttribute binormalAtt;
    binormalAtt.Name_ = "binormal";
    outAttrs.push_back(binormalAtt);

    // do the actual work
    NVMeshMender mender;
    bool success = mender.Munge(inAttrs,        // these are my positions & indices
                                outAttrs,       // these are the outputs I requested, plus extra stuff generated on my behalf
                                3.141592654f,   // 3.141592654f / 2.5f,    // tangent space smooth angle
                                NULL,                   // no Texture matrix applied to my tex0 coords
                                NVMeshMender::FixTangents,              // fix degenerate bases & texture mirroring
                                NVMeshMender::DontFixCylindricalTexGen, // handle cylidrically mapped textures via vertex duplication
                                NVMeshMender::WeightNormalsByFaceSize); // weight vertex normals by the triangle's size
    if (!success)
    {
        n_printf("nMeshBuilder: NVMeshMender::Munge() failed!\n");
        return false;
    }

    // NOTE: the resulting mesh may contain more vertices then the input mesh
    // thus we make sure that the outMesh is empty and fill it with the
    // new data
    n_assert(0 == outMesh.GetNumVertices());
    n_assert(0 == outMesh.GetNumTriangles());
    n_assert(0 == outMesh.GetNumGroups());
    
    numVertices = outAttrs[0].floatVector_.size() / 3;
    uint attIndex = 0;
    vector3 v3;
    for (attIndex = 0; attIndex < outAttrs.size(); attIndex++)
    {
        if ("position" == outAttrs[attIndex].Name_)
        {
            // write position
            n_assert(srcMesh.HasVertexComponent(Vertex::COORD));
            Vertex newVertex;
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                v3.x = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                v3.y = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                v3.z = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 2];
                newVertex.SetCoord(v3);
                outMesh.AddVertex(newVertex);
            }
        }
        else if ("normal" == outAttrs[attIndex].Name_)
        {
            // write normals
            if (srcMesh.HasVertexComponent(Vertex::NORMAL))
            {
                for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
                {
                    v3.x = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                    v3.y = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                    v3.z = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 2];
                    outMesh.GetVertexAt(vertexIndex).SetNormal(v3);
                }
            }
        }
        else if ("indices" == outAttrs[attIndex].Name_)
        {
            // write triangle indices
            numTriangles = outAttrs[attIndex].intVector_.size() / 3;
            n_assert(numTriangles > 0);
            Triangle newTriangle;
            int origGroupId = srcMesh.GetGroupAt(0).GetId();
            for (triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
            {
                int i0, i1, i2;
                i0 = outAttrs[attIndex].intVector_[triangleIndex * 3 + 0];
                i1 = outAttrs[attIndex].intVector_[triangleIndex * 3 + 1];
                i2 = outAttrs[attIndex].intVector_[triangleIndex * 3 + 2];
                newTriangle.SetVertexIndices(i0, i1, i2);
                newTriangle.SetGroupId(origGroupId);
                outMesh.AddTriangle(newTriangle);
            }
        }
        else if ("tangent" == outAttrs[attIndex].Name_)
        {
            // write newly created tangents
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                v3.x = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                v3.y = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                v3.z = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 2];
                outMesh.GetVertexAt(vertexIndex).SetTangent(v3);
            }
        }
        else if ("weight0" == outAttrs[attIndex].Name_)
        {
            // write first 2 components of vertex weights
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                vector4 v4 = outMesh.GetVertexAt(vertexIndex).GetWeights();
                v4.x = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                v4.y = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                outMesh.GetVertexAt(vertexIndex).SetWeights(v4);
            }
        }
        else if ("weight1" == outAttrs[attIndex].Name_)
        {
            // write last 2 components of vertex weights
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                vector4 v4 = outMesh.GetVertexAt(vertexIndex).GetWeights();
                v4.z = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                v4.w = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                outMesh.GetVertexAt(vertexIndex).SetWeights(v4);
            }
        }
        else if ("ji0" == outAttrs[attIndex].Name_)
        {
            // write first 2 components of joint indices
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                vector4 v4 = outMesh.GetVertexAt(vertexIndex).GetJointIndices();
                v4.x = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                v4.y = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                outMesh.GetVertexAt(vertexIndex).SetJointIndices(v4);
            }
        }
        else if ("ji1" == outAttrs[attIndex].Name_)
        {
            // write last 2 components of vertex weights
            for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
            {
                vector4 v4 = outMesh.GetVertexAt(vertexIndex).GetJointIndices();
                v4.z = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                v4.w = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                outMesh.GetVertexAt(vertexIndex).SetJointIndices(v4);
            }
        }
        else
        {
            // write texture coordinate sets
            vector2 v2;
            for (texCoordSet = 0; texCoordSet < 4; texCoordSet++)
            {
                char attName[32];
                sprintf(attName, "tex%d", texCoordSet);
                if (outAttrs[attIndex].Name_ == attName)
                {
                    Vertex::Component comp;
                    switch (texCoordSet)
                    {
                        case 0: comp = Vertex::UV0; break;
                        case 1: comp = Vertex::UV1; break;
                        case 2: comp = Vertex::UV2; break;
                        case 3: comp = Vertex::UV3; break;
                    }
                    if (srcMesh.HasVertexComponent(comp))
                    {
                        for (vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
                        {
                            v2.x = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 0];
                            v2.y = outAttrs[attIndex].floatVector_[vertexIndex * 3 + 1];
                            outMesh.GetVertexAt(vertexIndex).SetUv(texCoordSet, v2);
                        }
                    }
                }
            }
        }
    }

    // write a group
/*
    Group newGroup = srcMesh.GetGroupAt(0);
    newGroup.SetFirstTriangle(0);
    newGroup.SetNumTriangles(numTriangles);
    outMesh.AddGroup(newGroup);
*/
    return true;
}

//------------------------------------------------------------------------------
/*
    Generate vertex tangents. Uses nVidia's NVMeshMender class to do the
    actual work.
*/
bool
nMeshBuilder::BuildTangents()
{
    // First make a clean copy of the source mesh with coordinates 
    // and normales only. This eliminates unwanted hard edges when
    // MeshMender builds its adjacency information. Record the
    // cleanup operation in a collapsMap which we will need later.
    nArray< nArray<int> > collapsMap(32000, 32000);
    nMeshBuilder cleanMesh = *this;
    cleanMesh.ForceVertexComponents(Vertex::COORD | Vertex::NORMAL | Vertex::UV0);
    cleanMesh.Cleanup(&collapsMap);

    nMeshBuilder tangentMesh;
    cleanMesh.BuildTangentsForOneGroup(-1, tangentMesh);

    this->InflateCopyComponents(tangentMesh, collapsMap, Vertex::TANGENT);

/*
    // since the tangent generation can change the number of vertices
    // we must handle each group individually
    int groupIndex;
    int numGroups = this->GetNumGroups();
    nMeshBuilder finalMesh;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        nMeshBuilder groupMesh;
        if (!this->BuildTangentsForOneGroup(groupIndex, groupMesh))
        {
            n_printf("nMeshBuilder: error in tangent generation in mesh group %d\n", groupIndex);
            return false;
        }

        // append result to final mesh
        finalMesh.Append(groupMesh);
    }
    
    // overwrite myself with final mesh
    this->Copy(finalMesh);
*/

    return true;
}
