//------------------------------------------------------------------------------
//  nmeshbuilder_loadsave.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "util/npathstring.h"

//------------------------------------------------------------------------------
/**
    Load a mesh file. The file format will be determined by looking at
    the filename extension:

     - @c .n3d    -> legacy n3d files (ascii)
     - @c .n3d2   -> new n3d2 files (ascii)
     - @c .nvx    -> legacy nvx files (binary)
     - @c .nvx2   -> new nvx2 files (binary)
*/
bool
nMeshBuilder::Load(nFileServer2* fileServer, const char* filename)
{
    n_assert(filename);
    n_assert(fileServer);

    nPathString path(filename);
    if (path.CheckExtension("n3d"))
    {
        return this->LoadN3d(fileServer, filename);
    }
    else if (path.CheckExtension("n3d2"))
    {
        return this->LoadN3d2(fileServer, filename);
    }
    else
    {
        n_printf("nMeshBuilder::Save(): unsupported file extension in '%s'\n", filename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Save into mesh file. The file format will be determined by looking
    at the file extension:

     - .n3d2   n3d2 file format (ascii)
     - .nvx2   nvx2 file format (binary)
*/
bool
nMeshBuilder::Save(nFileServer2* fileServer, const char* filename)
{
    n_assert(filename);
    n_assert(fileServer);

    nPathString path(filename);
    if (path.CheckExtension("n3d2"))
    {
        return this->SaveN3d2(fileServer, filename);
    }
    else if (path.CheckExtension("nvx2"))
    {
        return this->SaveNvx2(fileServer, filename);
    }
    else
    {
        n_printf("nMeshBuilder::Save(): unsupported file extension in '%s'\n", filename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Save the mesh as binary nvx2 file. See inc/gfx2/nmesh2.h (nMesh2) for
    format specification.
*/
bool
nMeshBuilder::SaveNvx2(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer);
    n_assert(filename);

    // sort triangles by group id and create a group map
    this->SortTrianglesByGroupId();
    nArray<Group> groupMap;
    this->BuildGroupMap(groupMap);

    bool retval = false;
    nFile* file = fileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "wb"))
    {
        const int numGroups = groupMap.Size();
        const int vertexWidth = this->GetVertexAt(0).GetWidth();
        const int numVertices = this->GetNumVertices();
        const int numTriangles = this->GetNumTriangles();

        // write header
        file->PutInt('NVX2');
        file->PutInt(numGroups);
        file->PutInt(numVertices);
        file->PutInt(vertexWidth);
        file->PutInt(numTriangles);
        file->PutInt(this->GetVertexAt(0).GetComponentMask());

        // write groups
        int curGroupIndex;
        for (curGroupIndex = 0; curGroupIndex < groupMap.Size(); curGroupIndex++)
        {
            const Group& curGroup = groupMap[curGroupIndex];
            int firstTriangle = curGroup.GetFirstTriangle();
            int numTriangles  = curGroup.GetNumTriangles();
            int minVertexIndex, maxVertexIndex;
            this->GetGroupVertexRange(curGroup.GetId(), minVertexIndex, maxVertexIndex);
            file->PutInt(minVertexIndex);
            file->PutInt((maxVertexIndex - minVertexIndex) + 1);
            file->PutInt(firstTriangle);
            file->PutInt(numTriangles);
        }

        // write mesh block
        float* floatBuffer = new float[this->GetNumVertices() * vertexWidth];
        float* floatPtr = floatBuffer;
        int curVertexIndex;
        for (curVertexIndex = 0; curVertexIndex < numVertices; curVertexIndex++)
        {
            const Vertex& curVertex = this->GetVertexAt(curVertexIndex);
            if (curVertex.HasComponent(Vertex::COORD))
            {
                const vector3& v = curVertex.GetCoord();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
            }
            if (curVertex.HasComponent(Vertex::NORMAL))
            {
                const vector3& v = curVertex.GetNormal();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
            }
            if (curVertex.HasComponent(Vertex::TANGENT))
            {
                const vector3& v = curVertex.GetTangent();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
            }
            if (curVertex.HasComponent(Vertex::BINORMAL))
            {
                const vector3& v = curVertex.GetBinormal();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
            }
            if (curVertex.HasComponent(Vertex::COLOR))
            {
                const vector4& v = curVertex.GetColor();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
                *floatPtr++ = v.w;
            }
            int curUvSet;
            for (curUvSet = 0; curUvSet < Vertex::MAX_TEXTURE_LAYERS; curUvSet++)
            {
                if (curVertex.HasComponent((Vertex::Component) (Vertex::UV0 << curUvSet)))
                {
                    const vector2& v = curVertex.GetUv(curUvSet);
                    *floatPtr++ = v.x;
                    *floatPtr++ = v.y;
                }
            }
            if (curVertex.HasComponent(Vertex::WEIGHTS))
            {
                const vector4& v = curVertex.GetWeights();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
                *floatPtr++ = v.w;
            }
            if (curVertex.HasComponent(Vertex::JINDICES))
            {
                const vector4& v = curVertex.GetJointIndices();
                *floatPtr++ = v.x;
                *floatPtr++ = v.y;
                *floatPtr++ = v.z;
                *floatPtr++ = v.w;
            }
        }
        file->Write(floatBuffer, this->GetNumVertices() * vertexWidth * sizeof(float));
        delete[] floatBuffer;
        floatBuffer = 0;

        // write index block
        ushort* ushortBuffer = new ushort[this->GetNumTriangles() * 3];
        ushort* ushortPtr = ushortBuffer;
        int curTriangleIndex;
        for (curTriangleIndex = 0; curTriangleIndex < numTriangles; curTriangleIndex++)
        {
            const Triangle& curTriangle = this->GetTriangleAt(curTriangleIndex);
            int i0, i1, i2;
            curTriangle.GetVertexIndices(i0, i1, i2);
            *ushortPtr++ = (ushort) i0;
            *ushortPtr++ = (ushort) i1;
            *ushortPtr++ = (ushort) i2;
        }
        file->Write(ushortBuffer, this->GetNumTriangles() * 3 * sizeof(ushort));
        delete[] ushortBuffer;
        ushortBuffer = 0;
        
        file->Close();
        retval = true;
    }
    file->Release();
    return retval;
}
        
//------------------------------------------------------------------------------
/**
    Save as .n3d2 file.
*/
bool
nMeshBuilder::SaveN3d2(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer);
    n_assert(filename);

    // sort triangles by group id and create a group map
    this->SortTrianglesByGroupId();
    nArray<Group> groupMap;
    this->BuildGroupMap(groupMap);

    bool retval = false;
    nFile* file = fileServer->NewFileObject();
    n_assert(file);

    char lineBuffer[1024];
    if (file->Open(filename, "w"))
    {
        // write header
        file->PutS("type n3d2\n");
        sprintf(lineBuffer, "numgroups %d\n", groupMap.Size());
        file->PutS(lineBuffer);
        sprintf(lineBuffer, "numvertices %d\n", this->GetNumVertices());
        file->PutS(lineBuffer);
        sprintf(lineBuffer, "vertexwidth %d\n", this->GetVertexAt(0).GetWidth());
        file->PutS(lineBuffer);
        sprintf(lineBuffer, "numtris %d\n", this->GetNumTriangles());
        file->PutS(lineBuffer);

        // write vertex components
        sprintf(lineBuffer, "vertexcomps ");
        const Vertex& v = this->GetVertexAt(0);
        if (v.HasComponent(Vertex::COORD))
        {
            strcat(lineBuffer, "coord ");
        }
        if (v.HasComponent(Vertex::NORMAL))
        {
            strcat(lineBuffer, "normal ");
        }
        if (v.HasComponent(Vertex::TANGENT))
        {
            strcat(lineBuffer, "tangent ");
        }
        if (v.HasComponent(Vertex::BINORMAL))
        {
            strcat(lineBuffer, "binormal ");
        }
        if (v.HasComponent(Vertex::COLOR))
        {
            strcat(lineBuffer, "color ");
        }
        if (v.HasComponent(Vertex::UV0))
        {
            strcat(lineBuffer, "uv0 ");
        }
        if (v.HasComponent(Vertex::UV1))
        {
            strcat(lineBuffer, "uv1 ");
        }
        if (v.HasComponent(Vertex::UV2))
        {
            strcat(lineBuffer, "uv2 ");
        }
        if (v.HasComponent(Vertex::UV3))
        {
            strcat(lineBuffer, "uv3 ");
        }
        if (v.HasComponent(Vertex::WEIGHTS))
        {
            strcat(lineBuffer, "weights ");
        }
        if (v.HasComponent(Vertex::JINDICES))
        {
            strcat(lineBuffer, "jindices");
        }
        strcat(lineBuffer, "\n");
        file->PutS(lineBuffer);

        // write groups
        int i;
        for (i = 0; i < groupMap.Size(); i++)
        {
            const Group& group = groupMap[i];
            int firstTriangle = group.GetFirstTriangle();
            int numTriangles  = group.GetNumTriangles();
            int minVertexIndex, maxVertexIndex;
            this->GetGroupVertexRange(group.GetId(), minVertexIndex, maxVertexIndex);

            sprintf(lineBuffer, "g %d %d %d %d\n", 
                    minVertexIndex, (maxVertexIndex - minVertexIndex) + 1,
                    firstTriangle, numTriangles);
            file->PutS(lineBuffer);
        }

        // write vertices
        int numVertices = this->GetNumVertices();
        for (i = 0; i < numVertices; i++)
        {
            char charBuffer[128];
            sprintf(lineBuffer, "v ");
            Vertex& curVertex = this->GetVertexAt(i);
            if (curVertex.HasComponent(Vertex::COORD))
            {
                sprintf(charBuffer, "%f %f %f ", curVertex.coord.x, curVertex.coord.y, curVertex.coord.z);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::NORMAL))
            {
                sprintf(charBuffer, "%f %f %f ", curVertex.normal.x, curVertex.normal.y, curVertex.normal.z);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::TANGENT))
            {
                sprintf(charBuffer, "%f %f %f ", curVertex.tangent.x, curVertex.tangent.y, curVertex.tangent.z);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::BINORMAL))
            {
                sprintf(charBuffer, "%f %f %f ", curVertex.binormal.x, curVertex.binormal.y, curVertex.binormal.z);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::COLOR))
            {
                sprintf(charBuffer, "%f %f %f %f ", curVertex.color.x, curVertex.color.y, curVertex.color.z, curVertex.color.w);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::UV0))
            {
                sprintf(charBuffer, "%f %f ", curVertex.uv[0].x, curVertex.uv[0].y);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::UV1))
            {
                sprintf(charBuffer, "%f %f ", curVertex.uv[1].x, curVertex.uv[1].y);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::UV2))
            {
                sprintf(charBuffer, "%f %f ", curVertex.uv[2].x, curVertex.uv[2].y);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::UV3))
            {
                sprintf(charBuffer, "%f %f ", curVertex.uv[3].x, curVertex.uv[3].y);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::WEIGHTS))
            {
                sprintf(charBuffer, "%f %f %f %f ", curVertex.weights.x, curVertex.weights.y, curVertex.weights.z, curVertex.weights.w);
                strcat(lineBuffer, charBuffer);
            }
            if (curVertex.HasComponent(Vertex::JINDICES))
            {
                sprintf(charBuffer, "%f %f %f %f ", curVertex.jointIndices.x, curVertex.jointIndices.y, curVertex.jointIndices.z, curVertex.jointIndices.w);
                strcat(lineBuffer, charBuffer);
            }
            strcat(lineBuffer, "\n");
            file->PutS(lineBuffer);
        }

        // write triangles
        int numTriangles = this->GetNumTriangles();
        for (i = 0; i < numTriangles; i++)
        {
            Triangle& curTriangle = this->GetTriangleAt(i);
            int i0, i1, i2;
            curTriangle.GetVertexIndices(i0, i1, i2);
            sprintf(lineBuffer, "t %d %d %d\n", i0, i1, i2);
            file->PutS(lineBuffer);
        }

        file->Close();
        retval = true;
    }
    file->Release();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Load a .n3d2 file.
*/
bool
nMeshBuilder::LoadN3d2(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer);
    n_assert(filename);

    bool retval = false;
    nFile* file = fileServer->NewFileObject();
    n_assert(file);

    nArray<Group> groupMap;
    int numGroups = 0;
    int numVertices = 0;
    int vertexWidth = 0;
    int numTriangles = 0;
    int curTriangle = 0;
    int curGroup = 0;
    int vertexComponents = 0;
    if (file->Open(filename, "r"))
    {
        char line[1024];
        while (file->GetS(line, sizeof(line)))
        {
            // get keyword
            char* keyWord = strtok(line, N_WHITESPACE);
            if (0 == keyWord)
            {
                continue;
            }
            else if (0 == strcmp(keyWord, "type"))
            {
                // type must be 'n3d2'
                char* typeString = strtok(0, N_WHITESPACE);
                n_assert(typeString);
                if (0 != strcmp(typeString, "n3d2"))
                {
                    n_printf("nMeshBuilder::Load(%s): Invalid type '%s', must be 'n3d2'\n", filename, typeString);
                    file->Close();
                    file->Release();
                    return false;
                }
            }
            else if (0 == strcmp(keyWord, "numgroups"))
            {
                char* numGroupsString = strtok(0, N_WHITESPACE);
                n_assert(numGroupsString);
                numGroups = atoi(numGroupsString);
            }
            else if (0 == strcmp(keyWord, "numvertices"))
            {
                char* numVerticesString = strtok(0, N_WHITESPACE);
                n_assert(numVerticesString);
                numVertices = atoi(numVerticesString);
            }
            else if (0 == strcmp(keyWord, "vertexwidth"))
            {
                char* vertexWidthString = strtok(0, N_WHITESPACE);
                n_assert(vertexWidthString);
                vertexWidth = atoi(vertexWidthString);
            }
            else if (0 == strcmp(keyWord, "numtris"))
            {
                char* numTrianglesString = strtok(0, N_WHITESPACE);
                n_assert(numTrianglesString);
                numTriangles = atoi(numTrianglesString);
            }
            else if (0 == strcmp(keyWord, "vertexcomps"))
            {
                char* str;
                while (str = strtok(0, N_WHITESPACE))
                {
                    if (0 == strcmp(str, "coord"))
                    {
                        vertexComponents |= Vertex::COORD;
                    }
                    else if (0 == strcmp(str, "normal"))
                    {
                        vertexComponents |= Vertex::NORMAL;
                    }
                    else if (0 == strcmp(str, "tangent"))
                    {
                        vertexComponents |= Vertex::TANGENT;
                    }
                    else if (0 == strcmp(str, "binormal"))
                    {
                        vertexComponents |= Vertex::BINORMAL;
                    }
                    else if (0 == strcmp(str, "color"))
                    {
                        vertexComponents |= Vertex::COLOR;
                    }
                    else if (0 == strcmp(str, "uv0"))
                    {
                        vertexComponents |= Vertex::UV0;
                    }
                    else if (0 == strcmp(str, "uv1"))
                    {
                        vertexComponents |= Vertex::UV1;
                    }
                    else if (0 == strcmp(str, "uv2"))
                    {
                        vertexComponents |= Vertex::UV2;
                    }
                    else if (0 == strcmp(str, "uv3"))
                    {
                        vertexComponents |= Vertex::UV3;
                    }
                    else if (0 == strcmp(str, "weights"))
                    {
                        vertexComponents |= Vertex::WEIGHTS;
                    }
                    else if (0 == strcmp(str, "jindices"))
                    {
                        vertexComponents |= Vertex::JINDICES;
                    }
                    else
                    {
                        n_printf("nMeshBuilder::Load(%s): Invalid vertex component '%s'\n", filename, str);
                        file->Close();
                        file->Release();
                        return false;
                    }
                }
            }
            else if (0 == strcmp(keyWord, "g"))
            {
                // a triangle group
                const char* firstVertexString   = strtok(0, N_WHITESPACE);
                const char* numVerticesString   = strtok(0, N_WHITESPACE);
                const char* firstTriangleString = strtok(0, N_WHITESPACE);
                const char* numTriangleString   = strtok(0, N_WHITESPACE);

                n_assert(firstTriangleString);
                n_assert(numTriangleString);

                Group group;
                group.SetId(curGroup++);
                group.SetFirstTriangle(atoi(firstTriangleString));
                group.SetNumTriangles(atoi(numTriangleString));
                groupMap.Append(group);
            }
            else if (0 == strcmp(keyWord, "v"))
            {
                // a vertex 
                n_assert(vertexComponents != 0);
                Vertex vertex;
                if (vertexComponents & Vertex::COORD)
                {
                    const char* xStr = strtok(0, N_WHITESPACE);
                    const char* yStr = strtok(0, N_WHITESPACE);
                    const char* zStr = strtok(0, N_WHITESPACE);
                    n_assert(xStr && yStr && zStr);
                    vertex.SetCoord(vector3((float) atof(xStr), (float) atof(yStr), (float) atof(zStr)));
                }
                if (vertexComponents & Vertex::NORMAL)
                {
                    const char* xStr = strtok(0, N_WHITESPACE);
                    const char* yStr = strtok(0, N_WHITESPACE);
                    const char* zStr = strtok(0, N_WHITESPACE);
                    n_assert(xStr && yStr && zStr);
                    vertex.SetNormal(vector3((float) atof(xStr), (float) atof(yStr), (float) atof(zStr)));
                }
                if (vertexComponents & Vertex::TANGENT)
                {
                    const char* xStr = strtok(0, N_WHITESPACE);
                    const char* yStr = strtok(0, N_WHITESPACE);
                    const char* zStr = strtok(0, N_WHITESPACE);
                    n_assert(xStr && yStr && zStr);
                    vertex.SetTangent(vector3((float) atof(xStr), (float) atof(yStr), (float) atof(zStr)));
                }
                if (vertexComponents & Vertex::BINORMAL)
                {
                    const char* xStr = strtok(0, N_WHITESPACE);
                    const char* yStr = strtok(0, N_WHITESPACE);
                    const char* zStr = strtok(0, N_WHITESPACE);
                    n_assert(xStr && yStr && zStr);
                    vertex.SetBinormal(vector3((float) atof(xStr), (float) atof(yStr), (float) atof(zStr)));
                }
                if (vertexComponents & Vertex::COLOR)
                {
                    const char* rStr = strtok(0, N_WHITESPACE);
                    const char* gStr = strtok(0, N_WHITESPACE);
                    const char* bStr = strtok(0, N_WHITESPACE);
                    const char* aStr = strtok(0, N_WHITESPACE);
                    n_assert(rStr && gStr && bStr && aStr);
                    vertex.SetColor(vector4((float) atof(aStr), (float) atof(gStr), (float) atof(bStr), (float) atof(aStr)));
                }
                if (vertexComponents & Vertex::UV0)
                {
                    const char* uStr = strtok(0, N_WHITESPACE);
                    const char* vStr = strtok(0, N_WHITESPACE);
                    n_assert(uStr && vStr);
                    vertex.SetUv(0, vector2((float) atof(uStr), (float) atof(vStr)));
                }
                if (vertexComponents & Vertex::UV1)
                {
                    const char* uStr = strtok(0, N_WHITESPACE);
                    const char* vStr = strtok(0, N_WHITESPACE);
                    n_assert(uStr && vStr);
                    vertex.SetUv(1, vector2((float) atof(uStr), (float) atof(vStr)));
                }
                if (vertexComponents & Vertex::UV2)
                {
                    const char* uStr = strtok(0, N_WHITESPACE);
                    const char* vStr = strtok(0, N_WHITESPACE);
                    n_assert(uStr && vStr);
                    vertex.SetUv(2, vector2((float) atof(uStr), (float) atof(vStr)));
                }
                if (vertexComponents & Vertex::UV3)
                {
                    const char* uStr = strtok(0, N_WHITESPACE);
                    const char* vStr = strtok(0, N_WHITESPACE);
                    n_assert(uStr && vStr);
                    vertex.SetUv(3, vector2((float) atof(uStr), (float) atof(vStr)));
                }
                if (vertexComponents & Vertex::WEIGHTS)
                {
                    const char* xStr = strtok(0, N_WHITESPACE);
                    const char* yStr = strtok(0, N_WHITESPACE);
                    const char* zStr = strtok(0, N_WHITESPACE);
                    const char* wStr = strtok(0, N_WHITESPACE);
                    n_assert(xStr && yStr && zStr && wStr);
                    vertex.SetWeights(vector4(float(atof(xStr)), float(atof(yStr)), float(atof(zStr)), float(atof(wStr))));
                }
                if (vertexComponents & Vertex::JINDICES)
                {
                    const char* xStr = strtok(0, N_WHITESPACE);
                    const char* yStr = strtok(0, N_WHITESPACE);
                    const char* zStr = strtok(0, N_WHITESPACE);
                    const char* wStr = strtok(0, N_WHITESPACE);
                    n_assert(xStr && yStr && zStr && wStr);
                    vertex.SetJointIndices(vector4(float(atof(xStr)), float(atof(yStr)), float(atof(zStr)), float(atof(wStr))));
                }
				this->AddVertex(vertex);
            }
            else if (0 == strcmp(keyWord, "t"))
            {
                // a triangle
                const char* i0Str = strtok(0, N_WHITESPACE);
                const char* i1Str = strtok(0, N_WHITESPACE);
                const char* i2Str = strtok(0, N_WHITESPACE);
                Triangle triangle;
                triangle.SetVertexIndices(atoi(i0Str), atoi(i1Str), atoi(i2Str));
                curTriangle++;
                this->AddTriangle(triangle);
            }
        }
        file->Close();
        retval = true;
    }
    file->Release();

    // update the triangle group ids from the group map
    this->UpdateTriangleGroupIds(groupMap);
    return retval;
}

//------------------------------------------------------------------------------
/**
    Load an old-style n3d file. Since n3d has no concept of triangle groups,
    one group will be created for all triangles.
*/
bool
nMeshBuilder::LoadN3d(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer);
    n_assert(filename);

    nFile* file = fileServer->NewFileObject();
    n_assert(file);

    if (file->Open(filename, "r"))
    {
		int act_vn   = 0;
		int act_rgba = 0;
		int act_vt   = 0;
		int act_vt1  = 0;
		int act_vt2  = 0;
		int act_vt3  = 0;
        char line[1024];
		while (file->GetS(line, sizeof(line))) 
		{
			char *kw = strtok(line, N_WHITESPACE);
			if (kw) 
			{
				if (strcmp(kw, "v") == 0) 
				{
					char *xs = strtok(NULL, N_WHITESPACE);
					char *ys = strtok(NULL, N_WHITESPACE);
					char *zs = strtok(NULL, N_WHITESPACE);
					if (xs && ys && zs) 
					{
						Vertex vertex;
						vector3 v((float)atof(xs), (float)atof(ys), (float)atof(zs));
						vertex.SetCoord(v);
						this->AddVertex(vertex);
					} 
					else 
					{
						n_printf("Broken 'v' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "vn") == 0) 
				{
					char *nxs = strtok(NULL, N_WHITESPACE);
					char *nys = strtok(NULL, N_WHITESPACE);
					char *nzs = strtok(NULL, N_WHITESPACE);
					if (nxs && nys && nzs) 
					{
						vector3 v((float)atof(nxs), (float)atof(nys), (float)atof(nzs));
						this->GetVertexAt(act_vn++).SetNormal(v);
					} 
					else 
					{
						n_printf("Broken 'vn' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "rgba")== 0) 
				{
					char *rs = strtok(NULL, N_WHITESPACE);
					char *gs = strtok(NULL, N_WHITESPACE);
					char *bs = strtok(NULL, N_WHITESPACE);
					char *as = strtok(NULL, N_WHITESPACE);
					if (rs && gs && bs && as) 
					{
						vector4 v((float)atof(rs), (float)atof(gs), (float)atof(bs), (float)atof(as));
						this->GetVertexAt(act_rgba++).SetColor(v);
					} 
					else 
					{
						n_printf("Broken 'rgba' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "vt") == 0) 
				{
					char *us = strtok(NULL, N_WHITESPACE);
					char *vs = strtok(NULL, N_WHITESPACE);
					if (us && vs) 
					{
						vector2 v((float)atof(us), (float)atof(vs));
						this->GetVertexAt(act_vt++).SetUv(0, v);
					} 
					else 
					{
						n_printf("Broken 'vt' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "vt1") == 0)
				{
					char *us = strtok(NULL, N_WHITESPACE);
					char *vs = strtok(NULL, N_WHITESPACE);
					if (us && vs) 
					{
						vector2 v((float)atof(us), (float)atof(vs));
						this->GetVertexAt(act_vt1++).SetUv(1, v);
					} 
					else 
					{
						n_printf("Broken 'vt1' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "vt2") == 0)
				{
					char *us = strtok(NULL, N_WHITESPACE);
					char *vs = strtok(NULL, N_WHITESPACE);
					if (us && vs) 
					{
						vector2 v((float)atof(us), (float)atof(vs));
						this->GetVertexAt(act_vt2++).SetUv(2, v);
					} 
					else 
					{
						n_printf("Broken 'vt2' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "vt3") == 0)
				{
					char *us = strtok(NULL, N_WHITESPACE);
					char *vs = strtok(NULL, N_WHITESPACE);
					if (us && vs) 
					{
						vector2 v((float)atof(us), (float)atof(vs));
						this->GetVertexAt(act_vt3++).SetUv(3, v);
					} 
					else 
					{
						n_printf("Broken 'vt3' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				} 
				else if (strcmp(kw, "f") == 0) 
				{
					char* t0s = strtok(0, N_WHITESPACE);
					char* t1s = strtok(0, N_WHITESPACE);
					char* t2s = strtok(0, N_WHITESPACE);
					if (t0s && t1s && t2s)
					{
						char *slash;
						if ((slash=strchr(t0s, '/'))) 
                        {
							*slash=0;
                        }
						if ((slash=strchr(t1s, '/'))) 
                        {
							*slash=0;
                        }
						if ((slash=strchr(t2s, '/'))) 
                        {
							*slash=0;
                        }
						Triangle triangle;
						triangle.SetVertexIndices(atoi(t0s) - 1, atoi(t1s) - 1, atoi(t2s) - 1);
						triangle.SetGroupId(0);
						this->AddTriangle(triangle);
					}
					else 
					{
						n_printf("Broken 'f' line in '%s'!\n", filename);
						file->Close();
						file->Release();
						return false;
					}
				}
			}
		}

		file->Close();
		file->Release();
		return true;
	}

	file->Release();
	return false;
}

