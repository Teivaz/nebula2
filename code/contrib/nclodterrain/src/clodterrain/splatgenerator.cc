//
// class SplatGenerator
//
/* Copyright (C) 2004 Gary Haussmann
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */

#ifdef _MSC_VER
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include <math.h>
#include "clodterrain/splatgenerator.h"

SplatGenerator::~SplatGenerator()
{
}

// clear out current splats and start over
void SplatGenerator::clear()
{
    MeshGenerator::clear();

    // nuke our current splats
    m_splats.Clear();
}

// add a vertex to the list of vertices
void SplatGenerator::emit_vertex(int x, int y)
{
    MeshGenerator::emit_vertex(x,y);

    emit_triangle_maybe();
}

void SplatGenerator::emit_special_vertex(int x, int y, int z)
{
    MeshGenerator::emit_special_vertex(x,y,z);
    
    emit_triangle_maybe();
}

// repeat the last vertex, generating a degenerate triangle
void SplatGenerator::emit_previous_vertex()
{
    // nevermind, let the mesh generator do it
    MeshGenerator::emit_previous_vertex();

    // this is always a degenerate triangle, so don't even bother to try and emit a triangle
    // emit_triangle_maybe()
}

// check the most recent triangle and emit it if it isn't a degenerate triangle
void SplatGenerator::emit_triangle_maybe()
{
   // emit a full triangle if possible
    // do we have a full triangle to emit?
    if (m_indices.Size() > 2)
    {
        nArray<int>::iterator i = m_indices.End();
        int i1 = *(i-1);
        int i2 = *(i-2);
        int i3 = *(i-3);
        if ( (i1 != i2) && (i2 != i3) )
        {
            // reverse winding if needed
            if ( (m_indices.Size() & 1) == 0)
            {
                int ix = i1;
                i1 = i3;
                i3 = ix;
            }
            emit_triangle(i1,i2,i3);
        }
    }
}

// generate a full triangle.  Only called from emit_vertex when a whole triangle
// (not degenerates) is emitted
void SplatGenerator::emit_triangle(int i1, int i2, int i3)
{
    chunkvertex &v1 = m_vertices[i1], &v2 = m_vertices[i2], &v3 = m_vertices[i3];

    // which is the axis vertex?  too bad the chunker didn't tell us! we'll have to
    // figure it out for ourself
    // pick two legs of the triangle; the longer leg spans the l/r vertices
#define vdist(va, vb) sqrt(0.0f+(va.x-vb.x)*(va.x-vb.x)+(va.y-vb.y)*(va.y-vb.y))

    float leg1len(vdist(v1,v2)), leg2len(vdist(v2,v3));
    int ai, ri, li;

    if (leg1len > leg2len)
    {
        ri = i1; li = i2; ai = i3;
    }
    else if (leg1len == leg2len)
    {
        ai = i2; ri = i1; li = i3;
    }
    else
    {
        ai = i1; ri = i2; li = i3;
    }

    // find all the tiles needed for this triangle
    bool* tileflags = (bool*)alloca(TileIndexData::maxtileindex);
    chunkvertex &va = m_vertices[ai], &vr = m_vertices[ri], &vl = m_vertices[li];
    unsigned int numtiles = m_if->specifyTileIndices(va.x,va.y, vr.x,vr.y, vl.x,vl.y, tileflags);

    // add the triangle to the appropriate splats; we may need to make a new splat if there
    // is no splat for a given tile
    for (int tileix=0; tileix < TileIndexData::maxtileindex; tileix++)
    {
        if (tileflags[tileix])
        {
            // is there a splat for this index?
            for (nArray<splat>::iterator cursplat = m_splats.Begin();
                                         cursplat != m_splats.End();
                                         cursplat++)
            {
                if (cursplat->tileindex == tileix) break;
            }
            if (cursplat == m_splats.End())
            {
                // need to add a new splat
                m_splats.PushBack(splat(tileix));
                cursplat = m_splats.End()-1;
            }
            // add this triangle to the given splat
            cursplat->indices.PushBack(i1);
            cursplat->indices.PushBack(i2);
            cursplat->indices.PushBack(i3);
            numtiles--;
        }
    }

    // should have gotten them all
    n_assert(numtiles == 0);
}


trianglestats SplatGenerator::write_vertex_data(nFile &destfile, int level)
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


    // write out the original mesh (for collision use) and then write out the splats
    // first, write out a 0 where the normal MeshGenerator writes out the # of indices,
    // so that if the normal mesh loader tries to load this it will fail
    SI32SPEW(destfile, 0);

    // write out the counts for the triangle strip and the splats
    SI32SPEW(destfile, m_indices.Size()+1);       // size of orig. triangle strip

    // write out splat info

    // first find the splat w/ the largest number of triangles.  This is because
    // the first splat is always drawn with the full triangle sets, in order to
    // put down the base (black) color in addition to displaying the first texture.
    // Further splats use a subset of these triangles--only the triangles needed to
    // draw the splat texture are drawn, blended on top of the base.  If the first
    // splat has very few actual triangles with textures, we waste a lot of fill drawing
    // in black areas, where we could be using a splat which covers more of the chunk, and
    // get more out of our fill rate.
    nArray<splat>::iterator cursplat;
    nArray<splat>::iterator biggestsplat = m_splats.Begin();
    int biggestsplatsize = biggestsplat->indices.Size();
    for (cursplat = m_splats.Begin(); cursplat != m_splats.End(); cursplat++)
    {
        int currentsplatsize = cursplat->indices.Size();
        if (currentsplatsize > biggestsplatsize)
        {
            biggestsplatsize = currentsplatsize;
            biggestsplat = cursplat;
        }
    }
    // swap the biggest splat into the first slot--there's a lot of implicit copying
    // from this swap, but it's not like we're sorting the whole array or anything...
    splat tempsplat = *biggestsplat;
    *biggestsplat = *(m_splats.Begin());
    *(m_splats.Begin()) = tempsplat;

    // # of splats
    SI32SPEW(destfile, m_splats.Size());
    // describe each splat--the tile index and splat size
    for (cursplat = m_splats.Begin(); cursplat != m_splats.End(); cursplat++)
    {
        nArray<int> curindices = cursplat->indices;
        SI32SPEW(destfile, cursplat->tileindex);
        SI32SPEW(destfile, curindices.Size());
    }

    // write out the original triangle strip, for collision purposes
    {
        // Write triangle-strip vertex indices.
//      SI32SPEW(destfile, m_indices.Size());
        // HACK: write first index twice, to reverse triangle winding -GJH
        SI16SPEW(destfile, m_indices[0]);
        for (int i = 0; i < m_indices.Size(); i++) {
            SI16SPEW(destfile, m_indices[i]);
        }
    }

    // write out all the splats, as triangle lists
    for (cursplat = m_splats.Begin(); cursplat != m_splats.End(); cursplat++)
    {
        nArray<int> curindices = cursplat->indices;
        for (nArray<int>::iterator idx = curindices.Begin();
                                   idx != curindices.End();
                                   idx++)
        {
            SI16SPEW(destfile, *idx);
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
void SplatGenerator::write_svg_debug(nFile &svgfile, int level)
{
    // Compute bounding box.  Determines the scale and offset for
    // quantizing the verts.
    vector3 box_center = (m_min + m_max) * 0.5f;

    float compressfactor = 0.9f;
    int i;
    char buffer[1000];
    float v1x,v1y,v2x,v2y,v3x,v3y;

    // generate colored triangle sets for each splat
    for (nArray<splat>::iterator cursplat = m_splats.Begin();
                                 cursplat != m_splats.End();
                                 cursplat++)
    {
        unsigned char rgbcolor[3];
        m_if->getRGB(cursplat->tileindex, rgbcolor);
        char splatstart[1000];
        int r = rgbcolor[0], g = rgbcolor[1], b = rgbcolor[2];
        sprintf(splatstart,"<path style=\"fill:#%02x%02x%02x\" d=\"", r,g,b);
        svgfile.Write( splatstart, strlen(splatstart) );

        int addcr = 0; // add newlines for readability
        float tileinset = cursplat->tileindex * 0.5f;
        nArray<int> &curindices = cursplat->indices;
        for (i = 0; i < curindices.Size()-2; i+=3) {
            int i1 = curindices[i], i2 = curindices[i+1], i3 = curindices[i+2];
            if ( (i1 != i2) && (i2 != i3) )
            {
                v1x = (float)m_vertices[i1].x+tileinset;
                v1y = (float)m_vertices[i1].y+tileinset;
                v2x = (float)m_vertices[i2].x+tileinset;
                v2y = (float)m_vertices[i2].y+tileinset;
                v3x = (float)m_vertices[i3].x+tileinset;
                v3y = (float)m_vertices[i3].y+tileinset;
                v1x = (v1x - box_center.x) * compressfactor + box_center.x;
                v1y = (v1y - box_center.y) * compressfactor + box_center.y;
                v2x = (v2x - box_center.x) * compressfactor + box_center.x;
                v2y = (v2y - box_center.y) * compressfactor + box_center.y;
                v3x = (v3x - box_center.x) * compressfactor + box_center.x;
                v3y = (v3y - box_center.y) * compressfactor + box_center.y;

                sprintf(buffer, "M%6.2f %6.2f L%6.2f %6.2f L%6.2f %6.2f L%6.2f %6.2f ", v1x,v1y, v2x,v2y, v3x,v3y, v1x,v1y);
                if (addcr++ % 8 == 0)
                    strcat(buffer,"\n");
                svgfile.Write(buffer,strlen(buffer));
            }
        }

        svgfile.Write( "\"/>\n", 4);
    }

    // generate a line strip outline
    char pathstart[] = "<path style=\"stroke:black;fill:none\" d=\"";
    
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
    

