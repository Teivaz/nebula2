//
// class SplatGenerator
//
/* Copyright (C) 2004 Gary Haussmann
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */

#ifndef SPLATGENERATOR_H
#define SPLATGENERATOR_H

#ifndef MESHGENERATOR_H
#include "clodterrain/meshgenerator.h"
#endif

#ifndef TILEINDEXDATA_H
#include "clodterrain/tileindexdata.h"
#endif


// second helper class used to generate mesh data from the heightfield
/**
 * @class SplatGenerator
 * @ingroup NCLODTerrainContribModule
 *
 * As the mesh generator makes a
 * triangle strip for each terrain chunk, the splat generator generates
 * one or more "splats" for each terrain chunk.
 * In addition to the terrain heightfield bitmap, you need second bitmap of
 * the same size in pixels representing the texture indices.  Each pixel of
 * this second bitmap is an index into an array of various textures that can
 * be applied to the terrain.  For instance, some areas of the terrain would
 * be "grass" (index=1) and others would be "rock" (index=2) etc.  These
 * indices indicate textures that are blended together at runtime and
 * applied to the terrain.
 *
 * For a given terrain chunk, the triangles are grouped into splats, with one
 * splat corresponding to all the triangles needed to render a single tile type.
 * Then, these splats are merged and written out in groups of 4. (or less, or more,,
 * if you want to target hardware with a certain # of texture units)
 */
class SplatGenerator : public MeshGenerator
{
public:
    SplatGenerator(HeightFieldData *heightfield,
                   TileIndexData *indexfield,
            nFile *outputfile,
            unsigned int splatthickness = 4 // # of tile types per splat
                        )
               : MeshGenerator(heightfield, outputfile),
                     m_if(indexfield), m_splatthickness(splatthickness)
    {

    }

    virtual ~SplatGenerator();

    /// clear out current splats and start over
    virtual void clear();

    /// add a vertex to the list of vertices
    virtual void emit_vertex(int x, int y);

    /// add a vertex on the chunk skirt
    virtual void emit_special_vertex(int x, int y, int z);


    /// repeat the last vertex, generating a degenerate triangle
    virtual void emit_previous_vertex();

    virtual trianglestats write_vertex_data(nFile &destfile, int level);

    // write out vertex data as svg elements, for debugging
    virtual void write_svg_debug(nFile &svgfile, int level);
    
    /// specify the number of tile type in a splat, useful for customizing texture processing
    void setTilesPerSplat(int splatsize)
    { m_splatthickness = splatsize; }

private:
    TileIndexData *m_if; // bitmap of indices indicating what terrain type goes where on the terrain
    unsigned int m_splatthickness;

    struct splat {
       splat(): tileindex(-1) {}
       splat(int tix) : tileindex(tix) {} // constructor
       int tileindex;      // what tile index is this splat for?
       nArray<int> indices;
    };

    nArray<splat> m_splats;

    // generate a full triangle.  Only called from emit_vertex when a whole triangle
    // (not degenerates) is emitted
    void emit_triangle(int i1, int i2, int i3);

    // check the most recent triangle and emit it if it isn't a degenerate triangle
    void emit_triangle_maybe();
};

#endif
