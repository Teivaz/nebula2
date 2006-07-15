// implementation of mesh generator class
/* Copyright (C) 2003 Gary Haussmann, based on public domain code released by Thatcher Ulrich.
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */


#include "kernel/nfileserver2.h"
#include "clodterrain/meshgenerator.h"

void MeshGenerator::clear()
{
    // clean out vertex and index arrays
    m_vertices.Clear();
    m_indices.Clear();
    m_indextable.Clear();

    m_min = vector3(1e9,1e9,1e9);
    m_max = vector3(-1e9,-1e9,-1e9);
}

void MeshGenerator::emit_vertex(int x, int y)
{
    int index = get_vertex_index(x, y);
    m_indices.PushBack(index);

    // Check coordinates and update bounding box.
    float z = m_hf->getHeight(x,y);
    vector3 v( x * m_hf->getXscale(), 
               y * m_hf->getYscale(), 
               z);
    update_bounding_box(v);
}

void MeshGenerator::emit_previous_vertex()
{
        n_assert(m_indices.Size() > 0);
        int last_index = m_indices[m_indices.Size() - 1];
        m_indices.PushBack(last_index);
}

void MeshGenerator::emit_special_vertex(int x, int y, int z)
{
        int index = special_vertex_index(x, y, z);
        m_indices.PushBack(index);

        // Check coordinates and update bounding box.
        vector3 v(x * m_hf->getXscale(),
                  y * m_hf->getYscale(),
                  z * 1.0f);
        update_bounding_box(v);
}

int MeshGenerator::get_index_count()
{
    return m_indices.Size();
}

void MeshGenerator::update_bounding_box(const vector3 &newvertex)
{
    if (newvertex.x < m_min.x) m_min.x = newvertex.x;
    if (newvertex.y < m_min.y) m_min.y = newvertex.y;
    if (newvertex.z < m_min.z) m_min.z = newvertex.z;
    if (newvertex.x > m_max.x) m_max.x = newvertex.x;
    if (newvertex.y > m_max.y) m_max.y = newvertex.y;
    if (newvertex.z > m_max.z) m_max.z = newvertex.z;
}

// get the current index for a given vertex, allocating one if it doesn't exist
int MeshGenerator::get_vertex_index(int x, int y)
{
    int index = lookup_vertex_index(x,y);

    if (index != -1)
    {
        return index;
    }

    // no index allocated yet, allocate one
    index = m_vertices.Size();
    
    chunkvertex v(x,y);
    v.bufferindex = index;
    int key = chunkvertex::genkey(v);
    m_vertices.PushBack(v);
    m_indextable.Add(key, v);

    return index;
}

int MeshGenerator::special_vertex_index(int x, int y, int z)
{
    int index = m_vertices.Size();
    chunkvertex v(x, y, z);
    m_vertices.PushBack(v);

    return index;
}

// lookup the vertex index; returns -1 if the vertex hasn't been added yet
int MeshGenerator::lookup_vertex_index(int x, int y)
{
    chunkvertex protovertex(x,y);
    int key = chunkvertex::genkey(protovertex);

    // is it in the table?
    if (m_indextable.Find(key,protovertex))
    {
        return protovertex.bufferindex;
    }

    // no match
    return -1;
}


// write out vertex data to a file
trianglestats MeshGenerator::write_vertex_data(nFile &destfile, int level)
// Utility function, to output the quantized data for a vertex.
{
    trianglestats triangleinfo;

    // Write min & max z values.  This can be used to reconstruct
    // the bounding box.
    Sint16 minz = (Sint16)m_min.z;
    Sint16 maxz = (Sint16)m_max.z;
    SI16SPEW(destfile, minz);
    SI16SPEW(destfile, maxz);

    // Write a placeholder for the mesh data file pos.
    int current_pos = destfile.Tell();
    int dummy=0;
    SI32SPEW(destfile,dummy);
    
    // write out the vertex data at the *end* of the file.
    destfile.Seek(0,nFile::END);
    int mesh_pos = destfile.Tell();

    // Compute bounding box.  Determines the scale and offset for
    // quantizing the verts.
    vector3 box_center = (m_min + m_max) * 0.5f;
    vector3 box_extent = (m_max - m_min) * 0.5f;

    // Use (1 << 14) values in both positive and negative
    // directions.  Wastes just under 1 bit, but the total range
    // is [-2^14, 2^14], which is 2^15+1 values.  This is good --
    // fits nicely w/ 2^N+1 dimensions of binary-triangle-tree
    // vertex locations.
/*  n_printf("box center = %f %f %f, extent = %f %f %f\n",
        box_center.x, box_center.y, box_center.z,
        box_extent.x, box_extent.y, box_extent.z);*/
    {
        m_compressscales.x = (1<<14) / max(1.0f,box_extent.x);
        m_compressscales.y = (1<<14) / max(1.0f,box_extent.y);
        m_compressscales.z = (1<<14) / max(1.0f,box_extent.z);
        m_boxcenter = box_center;
    }

    // Make sure the vertex buffer is not too big.
    if (m_vertices.Size() >= (1 << 16)) {
        n_printf("error: chunk contains > 64K vertices.  Try processing again, but use\n"
               "the -d <depth> option to make a deeper chunk tree.\n"
               "Or, maybe the height scale is too big; use the -v option to scale it down.\n");
        exit(1);
    }

    // Write vertices.  All verts contain morph info.
    SI16SPEW(destfile, m_vertices.Size());
    for (int i = 0; i < m_vertices.Size(); i++) {
        write_vertex(destfile, level, m_vertices[i]);
    }

    {
        // Write triangle-strip vertex indices.
        SI32SPEW(destfile, m_indices.Size()+1);
        // HACK: write first index twice, to reverse triangle winding -GJH
        SI16SPEW(destfile, m_indices[0]);
        for (int i = 0; i < m_indices.Size(); i++) {
            SI16SPEW(destfile, m_indices[i]);
        }
    }

    // Count the real triangles in the main chunk.
    {
        int tris = 0;
        for (int i = 0; i < m_indices.Size() - 2; i++) {
            if (m_indices[i] != m_indices[i+1]
                && m_indices[i] != m_indices[i+2])
            {
                // Real triangle.
                triangleinfo.realtriangles++;
            }
            triangleinfo.totaltriangles++;
        }

        // Write real triangle count.
        SI32SPEW(destfile, triangleinfo.realtriangles);
    }

    // Rewind, and fill in the mesh data file pos.
    int mesh_size = destfile.Tell() - mesh_pos;
    destfile.Seek(current_pos, nFile::START);
    SI32SPEW(destfile, mesh_pos);
    SI32SPEW(destfile, mesh_size);

    return triangleinfo;
}

// write out vertex data as svg elements, for debugging
void MeshGenerator::write_svg_debug(nFile &svgfile, int level)
{
    // Compute bounding box.  Determines the scale and offset for
    // quantizing the verts.
    vector3 box_center = (m_min + m_max) * 0.5f;

    float compressfactor = 0.9f;
    int i;

    // generate a line strip
    char pathstart[] = "<path style=\"stroke:black\" d=\"";
    char buffer[1000];
    float v1x,v1y,v2x,v2y,v3x,v3y;
    
    // write out the header and move to the first point
    svgfile.Write( pathstart, strlen(pathstart) );

    int addcr = 0; // add newlines for readability
    for (i = 0; i < m_indices.Size()-2; i++) {
        int i1 = m_indices[i], i2 = m_indices[i+1], i3 = m_indices[i+2];
        if ( (i1 != i2) && (i2 != i3) )
        {
            v1x = (float)m_vertices[i1].x;
            v1y = (float)m_vertices[i1].y;
            v2x = (float)m_vertices[i2].x;
            v2y = (float)m_vertices[i2].y;
            v3x = (float)m_vertices[i3].x;
            v3y = (float)m_vertices[i3].y;
            v1x = (v1x - box_center.x) * compressfactor + box_center.x;
            v1y = (v1y - box_center.y) * compressfactor + box_center.y;
            v2x = (v2x - box_center.x) * compressfactor + box_center.x;
            v2y = (v2y - box_center.y) * compressfactor + box_center.y;
            v3x = (v3x - box_center.x) * compressfactor + box_center.x;
            v3y = (v3y - box_center.y) * compressfactor + box_center.y;

            sprintf(buffer, "M%6.2f %6.2f L%6.2f %6.2f L%6.2f %6.2f L%6.2f %6.2f ", v1x,v1y, v2x,v2y, v3x,v3y, v1x,v1y);
            if (addcr++ % 10 == 0)
                strcat(buffer,"\n");
            svgfile.Write(buffer,strlen(buffer));
        }
    }
    svgfile.Write( "\"/>\n", 4);
}



void MeshGenerator::write_vertex(nFile &destfile, int level, const chunkvertex &v)
{
    Sint16  x, y, z;
/*  n_printf("original vertex: %d %d %d\n", v.x,v.y,v.z);
    n_printf("xscale = %f, yscale = %f, box = %f %f %f scales = %f %f %f\n",
                m_hf->getXscale(), m_hf->getYscale(),
                m_boxcenter.x, m_boxcenter.y, m_boxcenter.z,
                m_compressscales.x, m_compressscales.y, m_compressscales.z);
*/
    x = (int) floor(((v.x * m_hf->getXscale() - m_boxcenter.x) * m_compressscales.x) + 0.5);
    y = (int) floor(((v.y * m_hf->getYscale() - m_boxcenter.y) * m_compressscales.y) + 0.5);
    if (v.isspecial) {
        z = v.z;
    } else {
        z = m_hf->getHeight(v.x,v.y);
    }

    //destfile->PutShort(x);
    //destfile->PutShort(y);
    //destfile->PutShort(z);
    SI16SPEW(destfile, x);
    SI16SPEW(destfile, y);
    SI16SPEW(destfile, z);

    // Morph info.  Calculate the difference between the
    // vert height, and the height of the same spot in the
    // next lower-LOD mesh.
    Sint16  lerped_height;
    if (v.isspecial) {
        lerped_height = z;  // special verts don't morph.
    } else {
        lerped_height = m_hf->get_height_at_LOD(level+1, v.x, v.y);
    }
    int morph_delta = (lerped_height - z);
    //destfile->PutShort((Sint16) morph_delta);
    Sint16 md = (Sint16) morph_delta;
//  n_printf(" vertex: %d %d %d; lerp=%d morph=%d\n", x,y,z, lerped_height, md);
    n_assert(morph_delta == md);    // Watch out for overflow.
    SI16SPEW(destfile, md);
}


