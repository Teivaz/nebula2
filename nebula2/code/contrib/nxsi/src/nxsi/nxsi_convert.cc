//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
// nXSI data convert functions
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <iostream>

using std::cerr;

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
        uvsetCount = NXSI_MAX_UVSETS;
        cerr << "ERROR: found over " << NXSI_MAX_UVSETS << " uvsets in polygon list. clamping into " << NXSI_MAX_UVSETS << " sets.\n";
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
            for (j = 2; j < pointCount; j++)
            {
                // copy positions
                *triPositions = polyPositions[0];
                triPositions++;
                *triPositions = polyPositions[1];
                triPositions++;
                *triPositions = polyPositions[j];
                triPositions++;

                // copy normals
                if (polyNormals)
                {
                    *triNormals = polyNormals[0];
                    triNormals++;
                    *triNormals = polyNormals[1];
                    triNormals++;
                    *triNormals = polyNormals[j];
                    triNormals++;
                }

                // copy colors
                if (polyColors) {
                    *triColors = polyColors[0];
                    triColors++;
                    *triColors = polyColors[1];
                    triColors++;
                    *triColors = polyColors[j];
                    triColors++;
                }

                // copy uvsets
                for (uv = 0; uv < uvsetCount; uv++)
                {
                    *triUvset[uv] = polyUvset[uv][0];
                    triUvset[uv]++;
                    *triUvset[uv] = polyUvset[uv][1];
                    triUvset[uv]++;
                    *triUvset[uv] = polyUvset[uv][j];
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

void nXSI::ConvertSITriangleStripList(CSLTriangleStripList* strip_list)
{
    cerr << "ERROR: currently doesn't handle triangle strips.\n";
}

//-----------------------------------------------------------------------------
// Eof
