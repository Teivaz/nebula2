//------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Clodnodes is released under the terms of the Nebula License, in doc/source/license.txt.
 */
//------------------------------------------------------------

#include <math.h>

#include "clodterrain/heightfielddata.h"
#include "clodterrain/meshgenerator.h"
#include "kernel/nfileserver2.h"
#include "clodterrain/nclodchunkernode.h"

nNebulaScriptClass(nCLODChunkerNode, "kernel::nroot");

// Manually synced!!!  (@@ should use a fixed-size struct, to be
// safer, although that ruins endian safety.)  If you change the chunk
// header contents, you must keep this constant in sync.  In DEBUG
// builds, there's an assert that should catch discrepancies, but be
// careful.
const int   CHUNK_HEADER_BYTES = 4 + 4*4 + 1 + /*2 + 2*/ + 2*2 + 4 + 4;

//
// nCLODChunkerNODE
//
// constructor
nCLODChunkerNode::nCLODChunkerNode() : nRoot(), m_validate(true),
    m_splatthickness(4),
    m_xspacing(1.0), m_yspacing(1.0), m_zscale(1.0),
    m_targetdepth(6), m_maxerror(2.5),
    m_outputfilename(NULL), m_tileindexfilename(NULL),
    m_ref_fs("/sys/servers/file2"),
    m_heightfield(NULL), m_tileindexfield(NULL), m_mesher(NULL)
{
}

nCLODChunkerNode::~nCLODChunkerNode()
{
    if (NULL != m_heightfield)
    {
        delete m_heightfield;
        m_heightfield = NULL;
    }
    if (NULL != m_tileindexfield)
    {
        delete m_tileindexfield;
        m_tileindexfield = NULL;
    }
    if (NULL != m_mesher)
    {
        delete m_mesher;
        m_mesher = NULL;
    }
}


void nCLODChunkerNode::setChunkFilename(const char *chunkfilename)
{
    m_outputfilename = chunkfilename;
}

void nCLODChunkerNode::setSpacings(float xspacing, float yspacing, float zscale)
{
    n_assert(xspacing > 0.0);
    n_assert(yspacing > 0.0);
    n_assert(zscale > 0.0);

    m_xspacing = xspacing;
    m_yspacing = yspacing;
    m_zscale = zscale;
}

void nCLODChunkerNode::setChunkParameters(unsigned int depth, float maxerror)
{
    n_assert(depth > 0);
    n_assert(maxerror > 0.0);

    m_targetdepth = depth;
    m_maxerror = maxerror;
}

void nCLODChunkerNode::compileChunksFromFile(const char *sourcefilename)
{
    n_assert(m_ref_fs.isvalid());
    nFile *sourcefile = m_ref_fs->NewFileObject();
    nString bigsrcpath = m_ref_fs->ManglePath(sourcefilename);
    if (!sourcefile->Open(bigsrcpath.Get(), "rb"))
    {
        n_error("nCLODChunkerNode::compileChunksFromFile(): Could not open file: %s",
                bigsrcpath);
        sourcefile->Release();
        return;
    }
    m_heightfield = new HeightFieldData(m_xspacing, m_yspacing, m_zscale);
    m_heightfield->readBitmap(bigsrcpath);
    n_assert(m_heightfield->isValid());
    m_zscale = m_heightfield->getZscale();

    compileChunksFromHeightField(m_heightfield);
    sourcefile->Close();
    sourcefile->Release();
}

void nCLODChunkerNode::compileChunksFromHeightField(HeightFieldData *heightmap)
{
    m_heightfield = heightmap;

    nFile *destfile = m_ref_fs->NewFileObject();
    if (!destfile->Open(m_outputfilename,"wb"))
    {
        n_error("nCLODChunkerNode::compileChunksFromHeightField(): Could not open file %s\n",
                m_outputfilename);
        destfile->Release();
        return;
    }
    // generate activation levels
    //n_printf("updating...\n");

    // Run a view-independent L-K style BTT update on the heightfield, to generate
    // error and activation_level values for each element.
    updateActivationLevel(0, m_heightfield->m_bitmapysize - 1,
        m_heightfield->m_bitmapxsize - 1, m_heightfield->m_bitmapysize - 1,
        0, 0);  // sw half of the square
    updateActivationLevel(m_heightfield->m_bitmapxsize - 1, 0,
        0, 0,
        m_heightfield->m_bitmapxsize - 1, m_heightfield->m_bitmapysize - 1);    // ne half of the square

    // Propagate the activation_level values of verts to their
    // parent verts, quadtree LOD style.  Gives same result as
    // L-K.
    //n_printf("propagating....");
    for (unsigned int i = 0; i < m_heightfield->m_logxsize; i++) {
        propagateActivationLevel(m_heightfield->m_logxsize - 1, i, m_heightfield->m_bitmapxsize >> 1, m_heightfield->m_bitmapysize >> 1);
        propagateActivationLevel(m_heightfield->m_logxsize - 1, i, m_heightfield->m_bitmapxsize >> 1, m_heightfield->m_bitmapysize >> 1);
        n_printf("propagating level %d\n", i);
        //char buffer[50];
        //sprintf(buffer,"prop%d.bmp", i);
        //m_heightfield->dumpActivationLevels(buffer, m_targetdepth);
    }

    if (m_validate)
    {
        n_printf("validating\n");
        validateActivationLevels(m_heightfield->m_bitmapxsize >> 1,
                                 m_heightfield->m_bitmapysize >> 1,
                                 m_heightfield->m_logxsize -1);
    }
    // Write a .chu header for the output file.
    SI32SPEW(*destfile, ('C') | ('L' << 8) | ('O' << 16) | ('D' << 24));    // four byte "CLOD" tag
    if (m_tileindexfilename)
    {
        SI16SPEW(*destfile, 11);    // splat file version.
    }
    else
    {
        SI16SPEW(*destfile, 10);    // "standard" Chunk LOD version.
    }
    SI16SPEW(*destfile, m_targetdepth); // depth of the chunk quadtree.
    float terrsize = (1<<m_heightfield->m_logxsize) * m_heightfield->getXscale();
    destfile->PutFloat(terrsize);
    destfile->PutFloat(terrsize);
    destfile->PutFloat(m_heightfield->getZscale()); // meters / unit of vertical measurement.
    destfile->PutFloat(m_maxerror); // max geometric error at base level mesh.

    // clear out the debug files
    /*
    char debugfilename[100];
    nFile *debugfile = m_ref_fs->NewFileObject();
    for (unsigned int lix=0; lix < m_targetdepth; lix++)
    {
        // write out debug file
        sprintf(debugfilename, "chunk_%d.svg",lix);
        if (debugfile->Open(debugfilename,"wb"))
        {
            char svgprelude[] = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000303 Stylable//EN\" \"http://www.w3.org/TR/2000/03/WD-SVG-20000303/DTD/svg-20000303-stylable.dtd\">\n<svg style=\"fill:white\">";
            debugfile->Write(svgprelude, strlen(svgprelude));
            debugfile->Close();
        }
    }
    debugfile->Release();
    */

    // now generate the triangle meshes
    if (m_tileindexfilename)
    {
        nString bigsrcpath = m_ref_fs->ManglePath(m_tileindexfilename);
        m_tileindexfield = new TileIndexData();
        m_tileindexfield->readBitmap(bigsrcpath);
        m_mesher = new SplatGenerator(m_heightfield, m_tileindexfield, destfile, m_splatthickness);
    }
    else
    {
        m_mesher = new MeshGenerator(m_heightfield, destfile);
    }

    generateEmptyTOC(*destfile, m_targetdepth);
    trianglestats t = generateAllMeshData(*destfile, 0,0, m_heightfield->m_logxsize, m_targetdepth);
    n_printf("total triangles: %d\n", t.totaltriangles);

    /*
    debugfile = m_ref_fs->NewFileObject();
    // wrap up debug files
    for (unsigned int lix=0; lix < m_targetdepth; lix++)
    {
        // write out debug file
        sprintf(debugfilename, "chunk_%d.svg",lix);
        if (debugfile->Open(debugfilename,"a+"))
        {
            debugfile->Seek(0, nFile::END);
            debugfile->Write("</svg>\n", 7);
            debugfile->Close();
        }
    }
    debugfile->Release();
    */
    destfile->Close();
    destfile->Release();
}

void nCLODChunkerNode::setValidateFlag(bool newstate)
{
    m_validate = newstate;
}

void nCLODChunkerNode::updateActivationLevel(
                               unsigned int ax, unsigned int ay,
                               unsigned int rx, unsigned int ry,
                               unsigned int lx, unsigned int ly)
{
    // Compute the coordinates of this triangle's base vertex.
    int dx = lx - rx;
    int dy = ly - ry;
    if (iabs(dx) <= 1 && iabs(dy) <= 1) {
        // We've reached the base level.  There's no base
        // vertex to update, and no child triangles to
        // recurse to.
        return;
    }

    // base vert is midway between left and right verts.
    unsigned int bx = rx + (dx >> 1);
    unsigned int by = ry + (dy >> 1);

    float error = m_zscale * fabsf( m_heightfield->getHeight(bx, by) - 0.5f * (m_heightfield->getHeight(lx,ly) + m_heightfield->getHeight(rx, ry)) );
    n_assert(error >= 0);
    if (error >= m_maxerror) {
        // Compute the mesh level above which this vertex
        // needs to be included in LOD meshes.
        int activation_level = (int) floor(log2(error / m_maxerror) + 0.5f);

        // Force the base vert to at least this activation level.
        m_heightfield->setActivation(bx,by, activation_level);
    }

    // Recurse to child triangles.
    updateActivationLevel(bx, by, ax, ay, rx, ry);  // base, apex, right
    updateActivationLevel(bx, by, lx, ly, ax, ay);  // base, left, apex
}

void nCLODChunkerNode::propagateActivationLevel(unsigned int level, unsigned int target_level,
                                unsigned int cx, unsigned int cy)
// Does a quadtree descent through the heightfield, in the square with
// center at (cx, cz) and size of (2 ^ (level + 1) + 1).  Descends
// until level == target_level, and then propagates this square's
// child center verts to the corresponding edge vert, and the edge
// verts to the center.  Essentially the quadtree meshing update
// dependency graph as in my Gamasutra article.  Must call this with
// successively increasing target_level to get correct propagation.
{
    int half_size = 1 << level;
    int quarter_size = half_size >> 1;

    if (level > target_level) {
        // Recurse to children.
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 2; i++) {
                propagateActivationLevel(level-1, target_level,
                               cx - quarter_size + half_size * i,
                               cy - quarter_size + half_size * j);
            }
        }
        return;
    }

    // We're at the target level.  Do the propagation on this
    // square.

    if (level > 0) {
        // Propagate child verts to edge verts.
        unsigned int lev = m_heightfield->getActivation(cx + quarter_size, cy - quarter_size);  // ne
        m_heightfield->activate(cx + half_size, cy, lev);
        m_heightfield->activate(cx, cy - half_size, lev);

        lev = m_heightfield->getActivation(cx - quarter_size, cy - quarter_size);   // nw
        m_heightfield->activate(cx, cy - half_size, lev);
        m_heightfield->activate(cx - half_size, cy, lev);

        lev = m_heightfield->getActivation(cx - quarter_size, cy + quarter_size);   // sw
        m_heightfield->activate(cx - half_size, cy, lev);
        m_heightfield->activate(cx, cy + half_size, lev);

        lev = m_heightfield->getActivation(cx + quarter_size, cy + quarter_size);   // se
        m_heightfield->activate(cx, cy + half_size, lev);
        m_heightfield->activate(cx + half_size, cy, lev);
    }

    // Propagate edge verts to center.
    m_heightfield->activate(cx, cy, m_heightfield->getActivation(cx + half_size, cy));
    m_heightfield->activate(cx, cy, m_heightfield->getActivation(cx, cy - half_size));
    m_heightfield->activate(cx, cy, m_heightfield->getActivation(cx, cy + half_size));
    m_heightfield->activate(cx, cy, m_heightfield->getActivation(cx - half_size, cy));

}

unsigned int nCLODChunkerNode::validateActivationLevels(unsigned int cx, unsigned int cy, unsigned int level)
// Debugging function -- verifies that activation level dependencies
// are correct throughout the tree.
{
    int half_size = 1 << level;
    int quarter_size = half_size >> 1;

    int max_act = -1;

    // cne = ne child, cnw = nw child, etc.
    int cne = -1;
    int cnw = -1;
    int csw = -1;
    int cse = -1;
    if (level > 0) {
        // Recurse to children.
        cne = validateActivationLevels(cx + quarter_size, cy - quarter_size, level - 1);
        cnw = validateActivationLevels(cx - quarter_size, cy - quarter_size, level - 1);
        csw = validateActivationLevels(cx - quarter_size, cy + quarter_size, level - 1);
        cse = validateActivationLevels(cx + quarter_size, cy + quarter_size, level - 1);
    }

    // ee == east edge, en = north edge, etc
    int ee = m_heightfield->getActivation(cx + half_size, cy);
    int en = m_heightfield->getActivation(cx, cy - half_size);
    int ew = m_heightfield->getActivation(cx - half_size, cy);
    int es = m_heightfield->getActivation(cx, cy + half_size);

    if (level > 0) {
        // Check child verts against edge verts.
        if (cne > ee || cse > ee) {
            n_printf("cp error! ee! lev = %d, cx = %d, cy = %d, alev = %d\n", level, cx, cy, ee);   //xxxxx
        }

        if (cne > en || cnw > en) {
            n_printf("cp error! en! lev = %d, cx = %d, cy = %d, alev = %d\n", level, cx, cy, en);   //xxxxx
        }

        if (cnw > ew || csw > ew) {
            n_printf("cp error! ew! lev = %d, cx = %d, cy = %d, alev = %d\n", level, cx, cy, ew);   //xxxxx
        }

        if (csw > es || cse > es) {
            n_printf("cp error! es! lev = %d, cx = %d, cy = %d, alev = %d\n", level, cx, cy, es);   //xxxxx
        }
    }

    // Check level of edge verts against center.
    int c = m_heightfield->getActivation(cx, cy);
    max_act = imax(max_act, ee);
    max_act = imax(max_act, en);
    max_act = imax(max_act, es);
    max_act = imax(max_act, ew);

    if (max_act > c) {
        n_printf("cp error! center! lev = %d, cx = %d, cy = %d, alev = %d, max_act = %d ee = %d en = %d ew = %d es = %d\n", level, cx, cy, c, max_act, ee, en, ew, es); //xxxxx
    }

    return imax(max_act, c);
}

// shamelessly yanked from Thatcher's tqt library - GJH
static int tqt_node_count(int depth)
// Return the number of nodes in a fully populated quadtree of the specified depth.
{
    return 0x55555555 & ((1 << depth*2) - 1);
}


// generate zero-filled output TOC, to be filled as we generate mesh data
void nCLODChunkerNode::generateEmptyTOC(nFile &destfile, int root_level)
// Append an empty table-of-contents for a fully-populated quadtree,
// and rewind the stream to the start of the contents.  Use this to
// make room for TOC at the beginning of the file, while bulk
// vert/index data gets appended after the TOC.
{
    unsigned char buf[CHUNK_HEADER_BYTES];  // dummy chunk header.
    memset(buf, 0, sizeof(buf));

    int start_pos = destfile.Tell();

    int chunk_count = tqt_node_count(root_level + 1);   // tqt has a handy function to compute # of nodes in a quadtree
    for (int i = 0; i < chunk_count; i++)
    {
        destfile.Write(buf, sizeof(buf));
    }

    // Rewind, so caller can start writing real TOC data.
    destfile.Seek(start_pos, nFile::START);
}

// generate mesh data for a specific level and all deeper tree levels
trianglestats nCLODChunkerNode::generateAllMeshData(nFile &destfile, int x0, int y0, int log_size, int level)
{
    trianglestats overallstats;
    int start_pos = destfile.Tell();    // use this to verify the value of CHUNK_HEADER_BYTES

//  stats.output_chunks++;

    int size = (1 << log_size);
    int half_size = size >> 1;
    int cx = x0 + half_size;
    int cy = y0 + half_size;

    // Assign a label to this chunk, so edges can reference the chunks.
    int chunk_label = m_heightfield->node_index(cx, cy);
    n_printf("chunk_label(%d,%d) = %d\n", cx, cy, chunk_label);//xxxx

    // Write our label.
    SI32SPEW(destfile, chunk_label);

    // Write the labels of our neighbors.
    SI32SPEW(destfile, m_heightfield->node_index(cx + size, cy));
    SI32SPEW(destfile, m_heightfield->node_index(cx, cy - size));
    SI32SPEW(destfile, m_heightfield->node_index(cx - size, cy));
    SI32SPEW(destfile, m_heightfield->node_index(cx, cy + size));

    // Chunk address.
    int LOD_level = m_targetdepth - level;
    n_assert(LOD_level >= 0 && LOD_level < 256);
    destfile.PutChar((char)LOD_level);
//  SI16SPEW(destfile, x0 >> log_size);
//  SI16SPEW(destfile, y0 >> log_size);

    // Start making the mesh.
    m_mesher->clear();

    // !!! This needs to be done in propagate, or something (too late now) !!!
    // Make sure our corner verts are activated on this level.
    m_heightfield->activate(x0 + size, y0, level);
    m_heightfield->activate(x0, y0, level);
    m_heightfield->activate(x0, y0 + size, level);
    m_heightfield->activate(x0 + size, y0 + size, level);

    // Generate the mesh.
    generateBlockMeshData(destfile, x0+half_size, y0 + half_size, log_size, level);

//  printf("\b%c", spinner[(spin_count++)&3]);

//  // Print some interesting info.
    //n_printf("chunk: (%d, %d) size = %d\n", x0, y0, size);

    // Generate data for our edge skirts.  Go counterclockwise around
    // the outside (ensures correct winding).
    generateEdgeMeshData(destfile, 0, cx + half_size, cy + half_size, cx + half_size, cy - half_size, level); // east
    generateEdgeMeshData(destfile, 1, cx + half_size, cy - half_size, cx - half_size, cy - half_size, level); // north
    generateEdgeMeshData(destfile, 2, cx - half_size, cy - half_size, cx - half_size, cy + half_size, level); // west
    generateEdgeMeshData(destfile, 3, cx - half_size, cy + half_size, cx + half_size, cy + half_size, level); // south


    // Finish writing our data.
    trianglestats t;
    t = m_mesher->write_vertex_data(destfile, level);
    n_printf("triangles: %d\n", t.realtriangles);
    n_assert(t.realtriangles < 65000);

    // write out debug file
/*    char debugfilename[100];
    sprintf(debugfilename, "chunk_%d.svg",m_targetdepth - level);
    nFile *debugfile = m_ref_fs->NewFileObject();
    if (debugfile->Open(debugfilename,"a+"))
    {
        debugfile->Seek(0, nFile::END);
        m_mesher->write_svg_debug(*debugfile, level);
        debugfile->Close();
        debugfile->Release();
    }
  */
    // add in the triangle stats to our current ones
    overallstats += t;

    int header_bytes_written = destfile.Tell() - start_pos;
    n_assert(header_bytes_written == CHUNK_HEADER_BYTES);

    // recurse to child regions, to generate child chunks.
    if (level > 1) {
        int half_size = (1 << (log_size-1));
        overallstats += generateAllMeshData(destfile, x0, y0, log_size-1, level-1); // nw
        overallstats += generateAllMeshData(destfile, x0 + half_size, y0, log_size-1, level-1); // ne
        overallstats += generateAllMeshData(destfile, x0, y0 + half_size, log_size-1, level-1); // sw
        overallstats += generateAllMeshData(destfile, x0 + half_size, y0 +  half_size, log_size-1, level-1); // se
    }

    return overallstats;
}


struct gen_state {
    int my_buffer[2][2];    // x,z coords of the last two vertices emitted by the generate_ functions.
    int activation_level;   // for determining whether a vertex is enabled in the block we're working on
    int ptr;    // indexes my_buffer.
    int previous_level; // for keeping track of level changes during recursion.

    bool    in_my_buffer(int x, int z)
    // Returns true if the specified vertex is in my_buffer.
    {
        return ((x == my_buffer[0][0]) && (z == my_buffer[0][1]))
            || ((x == my_buffer[1][0]) && (z == my_buffer[1][1]));
    }

    void    set_my_buffer(int x, int z)
    // Sets the current my_buffer entry to (x,z)
    {
        my_buffer[ptr][0] = x;
        my_buffer[ptr][1] = z;
    }
};


// generate mesh data for a specific square of data with the given center, size, and level
void nCLODChunkerNode::generateBlockMeshData(nFile &destfile, int cx, int cy, int log_size, int level)
// Generate the mesh for the specified square with the given center.
// This is paraphrased directly out of Lindstrom et al, SIGGRAPH '96.
// It generates a square mesh by walking counterclockwise around four
// triangular quadrants.  The resulting mesh is composed of a single
// continuous triangle strip, with a few corners turned via degenerate
// tris where necessary.
{
    // quadrant corner coordinates.
    int hs = 1 << (log_size - 1);
    int q[4][2] = {
        { cx + hs, cy + hs },   // se
        { cx + hs, cy - hs },   // ne
        { cx - hs, cy - hs },   // nw
        { cx - hs, cy + hs },   // sw
    };

    // Init state for generating mesh.
    gen_state   state;
    state.ptr = 0;
    state.previous_level = 0;
    state.activation_level = level;
    for (int i = 0; i < 4; i++) {
        state.my_buffer[i>>1][i&1] = -1;
    }

    m_mesher->emit_vertex(q[0][0], q[0][1]);
    state.set_my_buffer(q[0][0], q[0][1]);

    {for (int i = 0; i < 4; i++) {
        if ((state.previous_level & 1) == 0) {
            // tulrich: turn a corner?
            state.ptr ^= 1;
        } else {
            // tulrich: jump via degenerate?
            int x = state.my_buffer[1 - state.ptr][0];
            int y = state.my_buffer[1 - state.ptr][1];
            m_mesher->emit_vertex(x, y);    // or, emit vertex(last - 1);
        }

        // Initial vertex of quadrant.
        m_mesher->emit_vertex(q[i][0], q[i][1]);
        state.set_my_buffer(q[i][0], q[i][1]);
        state.previous_level = 2 * log_size + 1;

        generateQuadrantMeshData(
                  destfile,
                  &state,
                  q[i][0], q[i][1], // q[i][l]
                  cx, cy,   // q[i][t]
                  q[(i+1)&3][0], q[(i+1)&3][1], // q[i][r]
                  2 * log_size
            );
    }}
    if (state.in_my_buffer(q[0][0], q[0][1]) == false) {
        // finish off the strip.  @@ may not be necessary?
        m_mesher->emit_vertex(q[0][0], q[0][1]);
    }
}

// generate a single quadrant of mesh data
void nCLODChunkerNode::generateQuadrantMeshData(nFile &destfile, gen_state * s, int lx, int ly, int tx, int ty, int rx, int ry, int recursion_level)
// Auxiliary function for generate_block().  Generates a mesh from a
// triangular quadrant of a square heightfield block.  Paraphrased
// directly out of Lindstrom et al, SIGGRAPH '96.
{
    if (recursion_level <= 0) return;

    if (m_heightfield->getActivation(tx, ty) >= s->activation_level) {
        // Find base vertex.
        int bx = (lx + rx) >> 1;
        int by = (ly + ry) >> 1;

        generateQuadrantMeshData(destfile, s, lx, ly, bx, by, tx, ty, recursion_level - 1); // left half of quadrant

        if (s->in_my_buffer(tx,ty) == false) {
            if ((recursion_level + s->previous_level) & 1) {
                s->ptr ^= 1;
            } else {
                int x = s->my_buffer[1 - s->ptr][0];
                int y = s->my_buffer[1 - s->ptr][1];
                m_mesher->emit_vertex(x, y);    // or, emit vertex(last - 1);
            }
            m_mesher->emit_vertex(tx, ty);
            s->set_my_buffer(tx, ty);
            s->previous_level = recursion_level;
        }

        generateQuadrantMeshData(destfile, s, tx, ty, bx, by, rx, ry, recursion_level - 1);
    }
}

// generate edge "skirt" for a mesh block
void nCLODChunkerNode::generateEdgeMeshData(nFile &destfile, int direction, int x0, int y0, int x1, int y1, int level)
// Write out the data for an edge of the chunk that was just generated.
// (x0,z0) - (x1,z1) defines the extent of the edge in the heightfield.
// level determines which vertices in the mesh are active.
//
// Generates a "skirt" mesh which ensures that this mesh always covers
// the space between our simplified edge and the full-LOD edge.
{
    // We're going to write a list of vertices comprising the
    // edge.
    //
    // We're also going to write the index (in this list) of the
    // midpoint vertex of the edge, so the renderer can join
    // t-junctions.

    //
    // Vertices.
    //

    // Scan the edge, looking for the minimum height at each vert,
    // taking into account the full LOD mesh, plus all meshes up to
    // two levels above our own.


    nArray<int> vert_minimums;

    // Step along the edge.
    int dx, dy, steps;
    if (x0 < x1) {
        n_assert(y0 == y1);
        dx = 1;
        dy = 0;
        steps = x1 - x0 + 1;
    } else if (x0 > x1) {
        n_assert(y0 == y1);
        dx = -1;
        dy = 0;
        steps = x0 - x1 + 1;
    } else if (y0 < y1) {
        n_assert(x0 == x1);
        dx = 0;
        dy = 1;
        steps = y1 - y0 + 1;
    } else if (y0 > y1) {
        n_assert(x0 == x1);
        dx = 0;
        dy = -1;
        steps = y0 - y1 + 1;
    } else {
        n_assert(0);    // edge must have non-zero length...
    }

    int current_min = m_heightfield->getHeight(x0,y0);
    for (int i = 0, x = x0, y = y0; i < steps; i++, x += dx, y += dy)
    {
        current_min = imin(current_min, m_heightfield->getHeight(x,y));

        if (m_heightfield->getActivation(x,y) >= level) {
            // This is an active vert at this level of detail.

            // TODO: activation level & chunk level are consistent
            // with each other, but they go in the unintuitive
            // direction: level 0 is the *highest* (i.e. most
            // detailed) "level of detail".  Should reverse this to be
            // less confusing.

            // Check height of lower LODs.  The rule is, the renderer
            // allows a certain level of difference in LOD between
            // neighboring chunks, so we want to ensure we can cover
            // cracks that encompass gaps between our mesh and the
            // minimum LOD of our neighbor.
            //
            // The more levels of difference allowed, the less
            // constrained the renderer can be about paging.  On the
            // other hand, the more levels of difference allowed, the
            // more conservative we have to be with our skirts.
            //
            // Also, every edge is *interior* to some minimum LOD
            // chunk.  So we don't have to look any higher than that
            // LOD.

            int major_coord = x0;
            if (dy == 0) {
                major_coord = y0;
            }
            int minimum_edge_lod = m_heightfield->minimum_edge_lod(major_coord, m_targetdepth); // lod of the least-detailed chunk bordering this edge.

            const int MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE = 2;  // A parameter in the renderer as well -- keep in sync.

            int level_diff = imin(minimum_edge_lod + 1, m_targetdepth) - level;
            level_diff = imin(level_diff, MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE);

            for (int lod = level; lod <= level + level_diff; lod++)
            {
                Sint16  lod_height = m_heightfield->get_height_at_LOD(lod, x, y);
                current_min = imin(current_min, lod_height);
            }

            // Remember the minimum height of the edge for this
            // segment.

            if (current_min > -32768) {
                current_min -= 1;   // be slightly conservative here.
                }
            vert_minimums.PushBack(current_min);
            current_min = m_heightfield->getHeight(x,y);
        }
    }

    // Generate a "skirt" which runs around our outside edges and
    // makes sure we don't leave any visible cracks between our
    // (simplified) edges and the true shape of the mesh along our
    // edges.

    m_mesher->emit_previous_vertex();   // end the previous strip; starting a new one.
    if ((m_mesher->get_index_count() & 1) == 0) {
        // even number of verts, which means current winding order
        // will be backwards, after we add the degenerate to start the
        // strip.  Emit an extra degenerate vert to restore the normal
        // winding order.
        m_mesher->emit_previous_vertex();
    }

    int vert_index = 0;
    {for (int i = 0, x = x0, y = y0; i < steps; i++, x += dx, y += dy)
    {
        if (m_heightfield->getActivation(x,y) >= level)
        {
            // Put down a vertex which is at the minimum of our
            // previous and next segments.
            int min_height = vert_minimums[vert_index];
            if (vert_minimums.Size() > vert_index + 1)
            {
                min_height = imin(min_height, vert_minimums[vert_index + 1]);
            }

            m_mesher->emit_vertex(x,y);
            if (i == 0) {
                m_mesher->emit_previous_vertex();   // starting a new strip.
            }
            m_mesher->emit_special_vertex(x,y,min_height);

            vert_index++;
        }
    }}
}


/**
    * @brief Set the tile index bitmap to use
    *
    * If tile index bitmap is specified, the chunker will partition the triangles
    * in "splats" such that each splat contains no more than N tiles, where you can
    * also specify N.
    */
void nCLODChunkerNode::setTileIndexFilename(const char *tileindexfilename)
{
    if (strlen(tileindexfilename) < 1)
        m_tileindexfilename = NULL;
    else
        m_tileindexfilename = tileindexfilename;
}

/**
    * @brief Set tiles per splat
    *
    * the default value is 4
    */
void nCLODChunkerNode::setTilesPerSplat(int splatsize)
{
    m_splatthickness = splatsize;
}

