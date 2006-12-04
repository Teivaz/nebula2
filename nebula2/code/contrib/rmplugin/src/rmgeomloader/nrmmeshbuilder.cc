//-----------------------------------------------------------------------------
//  nrmmeshbuilder.cc
//
//  Mesh builder for geometry loader plug-in.
//
//  (C) 2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------

#include "rmgeomloader/nrmmeshbuilder.h"

//-----------------------------------------------------------------------------
/**
    Load .n3d2 and construct render monkey scene with it.

    @param filename .n3d2 filename which to be loaded.
    @param modelContainer RenderMonkey model container.
*/
bool ConstructScene(const RM_TCHAR* filename, RmMeshModelContainer *modelContainer)
{
    // convert wide char(unicode) to mbcs(ansi) : RenderMonkey uses unicode but N2 uses ansi.
    char path[1024] = {'\0'};
    wcstombs(path, filename, 1024);

    nMeshBuilder meshBuilder;
    if (!meshBuilder.Load(nFileServer2::Instance(), path))
    {
        return false;
    }

    nArray<nMeshBuilder::Group> group;
    meshBuilder.BuildGroupMap(group);

    //RmMeshModel *mesh = 0;
    for(int i=0; i<group.Size(); i++)
    {
        RmMeshModel *mesh = new RmMeshModel();
        assert(0 != mesh);

        // setup transformation matrix for this frame.
        //RmMatrix4x4& localTM = mesh->GetLocalTM();
        //localTM.Rotate(0.0f, 1.0f, 1.0f, 90, RM_ANGLE_DEGREE);

        if (BuildMesh(i, group, mesh, modelContainer, meshBuilder))
        {
            modelContainer->AddMeshModel(mesh);
        }
        else
        {
            // error
            delete mesh;
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Specify vertices and indices of loaded n3d2 mesh to RenderMonkey mesh.

    @param groupIdx
    @param group 
    @param modelContainer, 
    @param meshbuilder
*/
bool BuildMesh(int groupIdx, nArray<nMeshBuilder::Group> &group, 
               RmMeshModel* mesh, RmMeshModelContainer *modelContainer,
               const nMeshBuilder& meshbuilder)
{
    int numFaces = group[groupIdx].GetNumTriangles();

    int numVerts = 0; 
    int vmin, vmax;   // vertex range 

    if (meshbuilder.GetGroupVertexRange(groupIdx, vmin, vmax))
    {
        numVerts = (vmax - vmin) + 1;
    }
    else
    {
        return false;
    }

    if (numVerts <= 0)
    {
        return false;
    }

    mesh->GetVertexArray()->SetNumVertices(numVerts, false);
    mesh->GetVertexArray()->CreateVertexElementArray(RM_DECLUSAGE_POSITION, 0, RMVSDT_FLOAT3);

    RmVertexElementArray *elemPos  = mesh->GetVertexArray()->GetVertexElementArray(RM_DECLUSAGE_POSITION, 0, RMVSDT_FLOAT3);
    RmVertexElementArray *elemNorm = 0;
    RmVertexElementArray *elemUv0  = 0;
    RmVertexElementArray *elemUv1  = 0;
    RmVertexElementArray *elemClr  = 0;

    if (0 == elemPos)
    {
        // error
        return false;
    }

    nMeshBuilder::Vertex& v = meshbuilder.GetVertexAt(0);

    if (v.HasComponent(nMeshBuilder::Vertex::Component::UV0))
    {
        mesh->GetVertexArray()->CreateVertexElementArray( RM_DECLUSAGE_TEXCOORD, 0, RMVSDT_FLOAT2 );
        elemUv0 = mesh->GetVertexArray()->GetVertexElementArray(RM_DECLUSAGE_TEXCOORD,0,RMVSDT_FLOAT2);
    }

    if (v.HasComponent(nMeshBuilder::Vertex::Component::UV1))
    {
        mesh->GetVertexArray()->CreateVertexElementArray( RM_DECLUSAGE_TEXCOORD, 1, RMVSDT_FLOAT2 );
        elemUv1 = mesh->GetVertexArray()->GetVertexElementArray(RM_DECLUSAGE_TEXCOORD,1,RMVSDT_FLOAT2);
    }

    if (v.HasComponent(nMeshBuilder::Vertex::Component::NORMAL))
    {
        mesh->GetVertexArray()->CreateVertexElementArray( RM_DECLUSAGE_NORMAL, 0, RMVSDT_FLOAT3 );
        elemNorm = mesh->GetVertexArray()->GetVertexElementArray(RM_DECLUSAGE_NORMAL,0,RMVSDT_FLOAT3);
    }

    if (v.HasComponent(nMeshBuilder::Vertex::Component::COLOR))
    {
        mesh->GetVertexArray()->CreateVertexElementArray( RM_DECLUSAGE_COLOR, 0, RMVSDT_FLOAT4 );
        elemClr = mesh->GetVertexArray()->GetVertexElementArray(RM_DECLUSAGE_COLOR,0,RMVSDT_FLOAT4);
    }

    // copy vertex data into the vertex element array for RenderMonkey.
    int i = 0;
    int vidx = 0;
    for (i=vmin; i<=vmax; i++, vidx++)
    {
        nMeshBuilder::Vertex& v = meshbuilder.GetVertexAt(i);

        float *pos = (float*) elemPos->GetBuffer(vidx);
        pos[0] = v.coord.x;
        pos[1] = v.coord.y;
        pos[2] = v.coord.z;

        if (elemNorm)
        {
            float *normal = (float*)elemNorm->GetBuffer(vidx);
            normal[0] = v.normal.x;
            normal[1] = v.normal.y;
            normal[2] = v.normal.z;
        }

        if (elemUv0)
        {
            float *uv0 = (float*)elemUv0->GetBuffer(vidx);
            uv0[0] = v.uv[0].x;
            uv0[1] = v.uv[0].y;
        }

        if (elemUv1)
        {
            float *uv1 = (float*)elemUv1->GetBuffer(vidx);
            uv1[0] = v.uv[1].x;
            uv1[1] = v.uv[1].y;
        }

        if (elemClr)
        {
            float* color = (float*)elemClr->GetBuffer(vidx);
            color[0] = v.color.x;
            color[1] = v.color.y;
            color[2] = v.color.z;
            color[3] = v.color.w;
        }
    }

    // copy face index data into the index element 
    mesh->SetPrimitiveType(RM_PRIMITIVETYPE_TRIANGLELIST);
    mesh->SetNumIndices(numFaces * 3);

    RM_DWORD *indices = mesh->GetIndices();
    RM_DWORD facePos  = 0;

    int firstTri = group[groupIdx].GetFirstTriangle();
    for (int i=0; i<numFaces; i++)
    {
        nMeshBuilder::Triangle &triangle = meshbuilder.GetTriangleAt(firstTri + i);

        int i0 = triangle.vertexIndex[0] - vmin;
        int i1 = triangle.vertexIndex[1] - vmin;
        int i2 = triangle.vertexIndex[2] - vmin;

        indices[facePos * 3 + 0] = i0;
        indices[facePos * 3 + 1] = i1;
        indices[facePos * 3 + 2] = i2;

        facePos++;
    }

    if (0 == elemNorm)
    {
        mesh->GetVertexArray()->CreateVertexElementArray(RM_DECLUSAGE_NORMAL, 0, RMVSDT_FLOAT3);
        mesh->ComputeNormals(0);
    }

    mesh->GetVertexArray()->CreateVertexElementArray(RM_DECLUSAGE_TANGENT,  0, RMVSDT_FLOAT3);
    mesh->GetVertexArray()->CreateVertexElementArray(RM_DECLUSAGE_BINORMAL, 0, RMVSDT_FLOAT3);

    mesh->ComputeTangents(0);

    return true;
}

