//
// class MeshGenerator
//
/* Copyright (C) 2003 Gary Haussmann, based on public domain code released by Thatcher Ulrich.
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */

#ifndef MESHGENERATOR_H
#define MESHGENERATOR_H

#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "clodterrain/heightfielddata.h"
#include "mathlib/vector.h"
#include "util/narray.h"
#include "util/nkeyarray.h"

inline void SI32SPEW(nFile &d, int value) {d.Write(&value, sizeof(value)); }
inline void SI16SPEW(nFile &d, short value) { d.Write(&value, sizeof(value)); }

// struct that holds statistics of triangles written out, so you can see how
// many degenerate tris were generated
struct trianglestats {
    trianglestats() : totaltriangles(0), realtriangles(0) {}

    trianglestats &operator+=(const trianglestats &other)
    { totaltriangles += other.totaltriangles;
    realtriangles += other.realtriangles;
    return *this; }

    int totaltriangles;
    int realtriangles;
};

// second helper class used to generate mesh data from the heightfield
class MeshGenerator {
public:
    MeshGenerator(HeightFieldData *heightfield, nFile *outputfile)
        : m_hf(heightfield), m_outfile(outputfile), 
    m_vertices(), m_indices(), m_indextable(50,50) {}

    virtual ~MeshGenerator() {}

    // clear out current mesh and start over
    virtual void clear();

    // add a vertex to the list of vertices
    virtual void emit_vertex(int x, int y);

    // repeat the last vertex, generating a degenerate triangle
    virtual void emit_previous_vertex();

    // used to find current winding on the tristrip, to see if we need a degenerate tri
    int get_index_count();

    // for special vertices, used to specify the edge skirts
    virtual void emit_special_vertex(int x, int y, int z);

    // get the current index for a given vertex, allocating one if it doesn't exist
    int get_vertex_index(int x, int y);

    int special_vertex_index(int x, int y, int z);

    // lookup the vertex index; returns -1 if the vertex hasn't been added yet
    int lookup_vertex_index(int x, int y);

    // write out vertex data to a .chu file
    virtual trianglestats write_vertex_data(nFile &destfile, int level);

    // write out vertex data as svg elements, for debugging
    virtual void write_svg_debug(nFile &svgfile, int level);

protected:
    HeightFieldData *m_hf;
    nFile *m_outfile;

    struct chunkvertex {
        int x,y,z;
        bool isspecial;

        int bufferindex;

        // constructor for normal vertex
        chunkvertex(int vx, int vy) : x(vx), y(vy), z(0), isspecial(false), bufferindex(-1) {}
        chunkvertex(int vx, int vy, int vz) : x(vx), y(vy), z(vz), isspecial(true), bufferindex(-1) {}
        chunkvertex() : x(-1), y(-1), z(-1), bufferindex(-1), isspecial(false) {}

        static int genkey(const chunkvertex &v)
        { return v.x + 
                (v.y << 5) * 101 + 
                (v.z << 10) * 101 + 
                (v.isspecial ? 1 : 0); }
    };

    void update_bounding_box(const vector3 &newvertex);

    // arrays of vertices and indices
    nArray<chunkvertex> m_vertices;
    nArray<int> m_indices;
    nKeyArray<chunkvertex> m_indextable;

    // other data relevant to the construction of the mesh
    // bounding box
    vector3 m_min,m_max;

    // values used when we write the vertex data to a file
    vector3 m_boxcenter, m_compressscales;

    void write_vertex(nFile &destfile, int level, const chunkvertex &v);
};

#endif
