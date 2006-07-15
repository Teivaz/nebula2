//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
// nXSI data convert functions
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"

//-----------------------------------------------------------------------------

void nXSI::ConvertSIPolygonList(CSLPolygonList* polyList)
{
    CSLMesh* mesh       = (CSLMesh*)polyList->ParentModel()->Primitive();
    int polyCount       = polyList->GetPolygonCount();
    int* polyPointCount = polyList->GetPolygonVertexCountListPtr();
    int uvsetCount      = polyList->GetUVArrayCount();
    int triangleCount   = 0;
    int pointCount;
    int i,j, uv;

    // calculate triangle count
    for (i = 0; i < polyCount; i++)
    {
        triangleCount += polyPointCount[i] - 2;
    }

    // create triangle list object
    CSLTriangleList* triangleList = mesh->AddTriangleList();
    triangleList->SetName(polyList->GetName());
    triangleList->SetMaterial(polyList->GetMaterial());
    triangleList->GetVertexIndices()->Resize(triangleCount);

    if (polyList->GetNormalIndicesPtr())
    {
        triangleList->CreateNormalIndices()->Resize(triangleCount);
    }
    if (polyList->GetColorIndicesPtr())
    {
        triangleList->CreateColorIndices()->Resize(triangleCount);
    }
    if (uvsetCount > NXSI_MAX_UVSETS)
    {
        n_printf("WARNING: found %i uvsets in polygon list. clamping into %i sets.\n", uvsetCount, NXSI_MAX_UVSETS);
        uvsetCount = NXSI_MAX_UVSETS;
    }
    for (i = 0; i < uvsetCount; i++)
    {
        triangleList->AddUVArray()->Resize(triangleCount);
    }

    // convert datas
    { 
        // get poly buffers
        int* polyPositions = polyList->GetVertexIndicesPtr();
        int* polyNormals   = polyList->GetNormalIndicesPtr();
        int* polyColors    = polyList->GetColorIndicesPtr();

        int* polyUvset[NXSI_MAX_UVSETS];
        for (i = 0; i < uvsetCount; i++) {
            polyUvset[i]   = polyList->GetUVIndicesPtr(i);
        }

        // get triangle buffers
        int* triPositions  = triangleList->GetVertexIndicesPtr();
        int* triNormals    = triangleList->GetNormalIndicesPtr();
        int* triColors     = triangleList->GetColorIndicesPtr();

        int* triUvset[NXSI_MAX_UVSETS];
        for (i = 0; i < uvsetCount; i++)
        {
            triUvset[i] = triangleList->GetUVIndicesPtr(i);
        }

        // convert datas
        for (i = 0; i < polyCount; i++)
        {
            pointCount = polyPointCount[i];
            for (j = 1; j < pointCount-1; j++)
            {
                // copy positions
                *triPositions = polyPositions[0];
                triPositions++;
                *triPositions = polyPositions[j];
                triPositions++;
                *triPositions = polyPositions[j+1];
                triPositions++;

                // copy normals
                if (polyNormals)
                {
                    *triNormals = polyNormals[0];
                    triNormals++;
                    *triNormals = polyNormals[j];
                    triNormals++;
                    *triNormals = polyNormals[j+1];
                    triNormals++;
                }

                // copy colors
                if (polyColors) {
                    *triColors = polyColors[0];
                    triColors++;
                    *triColors = polyColors[j];
                    triColors++;
                    *triColors = polyColors[j+1];
                    triColors++;
                }

                // copy uvsets
                for (uv = 0; uv < uvsetCount; uv++)
                {
                    *triUvset[uv] = polyUvset[uv][0];
                    triUvset[uv]++;
                    *triUvset[uv] = polyUvset[uv][j];
                    triUvset[uv]++;
                    *triUvset[uv] = polyUvset[uv][j+1];
                    triUvset[uv]++;
                }
            }

            // advance poly buffers
            polyPositions += pointCount;
            if (polyNormals) polyNormals += pointCount;
            if (polyColors) polyColors += pointCount;
            for (uv = 0; uv < uvsetCount; uv++)
            {
                polyUvset[uv] += pointCount;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void nXSI::ConvertSITriangleStripList(CSLTriangleStripList* stripList)
{
    CSLMesh* mesh             = (CSLMesh*)stripList->ParentModel()->Primitive();
    CSLTriangleStrip** strips = stripList->TriangleStrips();
    int stripCount            = stripList->GetTriangleStripCount();
    int uvsetCount            = 0;
    int triangleCount;
    int second;
    int last;
    int i, j, uv;

    // calculate triangle count
    triangleCount = 0;
    for (i = 0; i < stripCount; i++)
    {
        triangleCount += strips[i]->GetVertexCount() - 2;
        uvsetCount = strips[i]->GetUVArrayCount();
    }

    // create triangle list object
    CSLTriangleList* triangleList = mesh->AddTriangleList();
    triangleList->SetName(stripList->GetName());
    triangleList->SetMaterial(stripList->GetMaterial());
    triangleList->GetVertexIndices()->Resize(triangleCount);

    if (strips[0]->GetNormalIndicesPtr())
    {
        triangleList->CreateNormalIndices()->Resize(triangleCount);
    }
    if (strips[0]->GetColorIndicesPtr())
    {
        triangleList->CreateColorIndices()->Resize(triangleCount);
    }
    if (uvsetCount > NXSI_MAX_UVSETS)
    {
        n_printf("WARNING: found %i uvsets in triangle strip list. clamping into %i sets.\n", uvsetCount, NXSI_MAX_UVSETS);
        uvsetCount = NXSI_MAX_UVSETS;
    }
    for (i = 0; i < uvsetCount; i++)
    {
        triangleList->AddUVArray()->Resize(triangleCount);
    }

    // get triangle buffers
    int* triPositions  = triangleList->GetVertexIndicesPtr();
    int* triNormals    = triangleList->GetNormalIndicesPtr();
    int* triColors     = triangleList->GetColorIndicesPtr();

    int* triUvset[NXSI_MAX_UVSETS];
    for (i = 0; i < uvsetCount; i++)
    {
        triUvset[i] = triangleList->GetUVIndicesPtr(i);
    }

    // copy datas
    for (j = 0; j < stripCount; j++)
    { 
        CSLTriangleStrip* strip = strips[j];

        // get strip buffers
        int* stripPositions = strip->GetVertexIndicesPtr();
        int* stripNormals   = strip->GetNormalIndicesPtr();
        int* stripColors    = strip->GetColorIndicesPtr();

        int* stripUvset[NXSI_MAX_UVSETS];
        for (i = 0; i < uvsetCount; i++) {
            stripUvset[i]   = strip->GetUVIndicesPtr(i);
        }

        // copy datas
        for (i = 0; i < strip->GetVertexCount()-2; i++)
        {
            second = i+1;
            last = i+2;
            if (i & 1)
            {
                second = i+2;
                last = i+1;
            }

            // copy positions
            *triPositions = stripPositions[i];
            triPositions++;
            *triPositions = stripPositions[second];
            triPositions++;
            *triPositions = stripPositions[last];
            triPositions++;

            // copy normals
            if (stripNormals)
            {
                *triNormals = stripNormals[i];
                triNormals++;
                *triNormals = stripNormals[second];
                triNormals++;
                *triNormals = stripNormals[last];
                triNormals++;
            }

            // copy colors
            if (stripColors) {
                *triColors = stripColors[i];
                triColors++;
                *triColors = stripColors[second];
                triColors++;
                *triColors = stripColors[last];
                triColors++;
            }

            // copy uvsets
            for (uv = 0; uv < uvsetCount; uv++)
            {
                *triUvset[uv] = stripUvset[uv][i];
                triUvset[uv]++;
                *triUvset[uv] = stripUvset[uv][second];
                triUvset[uv]++;
                *triUvset[uv] = stripUvset[uv][last];
                triUvset[uv]++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Eof
