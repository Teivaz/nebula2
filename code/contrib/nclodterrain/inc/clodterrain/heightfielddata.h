//------------------------------------------------------------------------
/* Copyright (C) 2003 Gary Haussmann
 *
 * Re-targeting of Thatcher Ulrich's heightfield chunker code for use in
 * The Nebula Device
 */
//------------------------------------------------------------------------

//
// class HeightFieldData
//


#ifndef HEIGHTFIELDDATA_H
#define HEIGHTFIELDDATA_H

#include <math.h>

#include "il/il.h"
#include "il/ilu.h"
#include "kernel/ntypes.h"
#include "util/nstring.h"

typedef short Sint16;

inline int imax(int a, int b) { return (a>b) ? a : b; }

inline int imin(int a, int b) { return (a>b) ? b : a; }

inline int iabs(int a) { return (a < 0) ? -a : a; }

inline int  iclamp(int i, int minval, int maxval) {
    n_assert( minval <= maxval );
    return imax(minval, imin(i, maxval));
}

inline double log2(double x) { return log(x)/log(2.0); }

static int  lowest_one(int x)
// Returns the bit position of the lowest 1 bit in the given value.
// If x == 0, returns the number of bits in an integer.
//
// E.g. lowest_one(1) == 0; lowest_one(16) == 4; lowest_one(5) == 0;
{
    int intbits = sizeof(x) * 8;
    int i;
    for (i = 0; i < intbits; i++, x = x >> 1) {
        if (x & 1) break;
    }
    return i;
}


// given a number, find the lowest power of 2 plus one,
// 2^N+1, greater than or equal to that number.  This is since
// ChunkLOD likes bitmaps of size 2^N+1 in x/y dimensions.
static unsigned int nextPOW2p1(unsigned int basevalue)
{
    unsigned int bitpos = 0;
    while ( (1 << bitpos) + 1u < basevalue)
    {
        bitpos++;
        n_assert(bitpos < 31);
    }
    return ( (1<<bitpos) + 1 ) ;
}
// given a number, find the lowest power of 2,
// 2^N, greater than or equal to that number.  This is since
// ChunkLOD likes tile index bitmaps of size 2^N in x/y dimensions.
static unsigned int nextPOW2(unsigned int basevalue)
{
    unsigned int bitpos = 0;
    while ( (1u << bitpos) < basevalue)
    {
        bitpos++;
        n_assert(bitpos < 31);
    }
    return ( (1<<bitpos) ) ;
}

// helper class to read in a bitmap as a heightfield
class HeightFieldData {
public:
    HeightFieldData(float xscale, float yscale, float zscale);
    virtual ~HeightFieldData();

    virtual bool readBitmap(nString bitmapfilename);

    void dumpActivationLevels(nString dumpfilename, unsigned int max_activation);

    Sint16 getHeight(unsigned int x, unsigned int y) const {
        n_assert(NULL != m_heights);
        n_assert(x < m_bitmapxsize);
        n_assert(y < m_bitmapysize);
        return m_heights[x+y*m_bitmapxsize];
    }

    void setHeight(unsigned x, unsigned int y, Sint16 newheight) {
        n_assert(NULL != m_heights);
        n_assert(x < m_bitmapxsize);
        n_assert(y < m_bitmapysize);
        m_heights[x+y*m_bitmapxsize] = newheight;
    }

    unsigned char getActivation(unsigned int x, unsigned int y) const {
        n_assert(NULL != m_activations);
        n_assert(x < m_bitmapxsize);
        n_assert(y < m_bitmapysize);
        return m_activations[x+y*m_bitmapxsize];
    }

    void setActivation(unsigned int x, unsigned int y, unsigned char newactivationlevel) {
        n_assert(NULL != m_activations);
        n_assert(x < m_bitmapxsize);
        n_assert(y < m_bitmapysize);
        m_activations[x+y*m_bitmapxsize] = newactivationlevel;
    }

    void activate(unsigned int x, unsigned int y, unsigned int level) {
        n_assert(NULL != m_activations);
        n_assert(x < m_bitmapxsize);
        n_assert(y < m_bitmapysize);
        n_assert(level < 255);
        unsigned char curlevel = getActivation(x,y);
        if (level > curlevel)
            setActivation(x,y,level);
    }

    int node_index(unsigned int x, unsigned int y)
    // Given the coordinates of the center of a quadtree node, this
    // function returns its node index.  The node index is essentially
    // the node's rank in a breadth-first quadtree traversal.  Assumes
    // a [nw, ne, sw, se] traversal order.
    //
    // If the coordinates don't specify a valid node (e.g. if the coords
    // are outside the heightfield) then returns -1.
    {
        if (x >= m_bitmapxsize || y >= m_bitmapysize) {
            return -1;
        }

        n_assert(m_logxsize == m_logysize);

        int l1 = lowest_one(x | y);
        int depth = m_logxsize - l1 - 1;

        int base = 0x55555555 & ((1 << depth*2) - 1);   // total node count in all levels above ours.
        int shift = l1 + 1;

        // Effective coords within this node's level.
        int col = x >> shift;
        int row = y >> shift;

        // interleave x and y coordinates here
        int curbit = 0;
        int xy = 0;
        while ((col>0) || (row>0))
        {
            xy |= (col&1) << curbit;
            curbit++;
            xy |= (row&1) << curbit;
            curbit++;
            col >>= 1;
            row >>= 1;
        }

        return base + xy;
        //return base + (row << depth) + col;
    }

    int minimum_edge_lod(int coord, int tree_depth)
    // Given an x or y coordinate, along which an edge runs, this
    // function returns the lowest LOD level that the edge divides.
    //
    // (This is useful for determining how conservative we need to be
    // with edge skirts.)
    {
        n_assert(m_logxsize == m_logysize);
        int l1 = lowest_one(coord);
        int depth = (m_logxsize - l1 - 1);

        return iclamp(tree_depth - depth, 0, tree_depth);   // TODO: reverse direction of level
    }

    Sint16 get_height_at_LOD(unsigned int level, unsigned int x, unsigned int y)
    // Returns the height of the mesh as simplified to the specified level
    // of detail.
    {
        if (y > x) {
            // Query in SW quadrant.
            return height_query(level, x, y, 
                        0, m_bitmapxsize - 1, 
                        m_bitmapxsize - 1, m_bitmapysize - 1, 
                        0, 0);  // sw half of the square

        } else {    // query in NW quadrant
            return height_query(level, x, y, 
                        m_bitmapxsize - 1, 0, 
                        0, 0, 
                        m_bitmapxsize - 1, m_bitmapysize - 1);  // ne half of the square
        }
    }

    bool isValid() const { return (m_heights != NULL); }

    float getXscale() const { return m_xscale; }
    float getYscale() const { return m_yscale; }
    float getZscale() const { return m_zscale; }

    unsigned int m_bitmapxsize, m_bitmapysize, m_logxsize, m_logysize;

protected:
    float m_xscale, m_yscale, m_zscale;

    unsigned char *m_activations;
    Sint16 *m_heights;

    Sint16 height_query(unsigned int level, unsigned int x, unsigned int y, 
                        unsigned int ax, unsigned int ay, 
                        unsigned int rx, unsigned int ry, 
                        unsigned int lx, unsigned int ly);
    // Returns the height of the query point (x,y) within the triangle (a, r, l),
    // as tesselated to the specified LOD.
    //
    // Return value is in heightfield discrete coords.  To get meters,
    // multiply by hf.vertical_scale.

};

#endif

