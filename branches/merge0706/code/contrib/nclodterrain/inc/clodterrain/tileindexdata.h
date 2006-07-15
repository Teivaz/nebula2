/**
 * @class TileIndexData
 * @ingroup NCLODTerrainContribModule
 *
 * This class manages access to a bitmap representing the various textures that
 * are tiled and blended together on a splatted texture terrain
 *
 * Copyright (C) 2004 Gary Haussmann
 */

#ifndef TILEINDEXDATA_H
#define TILEINDEXDATA_H

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif
#include "util/nstring.h"

class TileIndexData {
public:
    typedef unsigned char tileindex;
    const static int maxtilebits;
    const static int maxtileindex;

    TileIndexData() : m_bitmapxsize(0), m_bitmapysize(0), m_tileindices(NULL) 
    { n_assert(maxtilebits >= (sizeof(tileindex) * 8) ); };

    virtual ~TileIndexData();

    /// load a bitmap--typically an indexed bitmap!
    virtual bool readBitmap(nString bitmapfilename);

    /// get the tile index for a given location
    tileindex getTileIndex(int x, int y) const
    {
       n_assert(x >= 0 && x < m_bitmapxsize);
       n_assert(y >= 0 && y < m_bitmapysize);
       return m_tileindices[ x + y * m_bitmapxsize ];
    }

    /// get the tile index for a given location, returning -1 if the request coordinate is out-of-bounds
    int getTileIndexClipped(int x, int y) const
    {
        if ( (x<0) || (x >= m_bitmapxsize) ||
             (y<0) || (y >= m_bitmapxsize) )
             return -1;
        else
            return getTileIndex(x,y);
    }
    
    unsigned int getXSize() const
    { return m_bitmapxsize; }

    unsigned int getYSize() const
    { return m_bitmapysize; }

    /// get the number of different tile indices for a given triangle
    unsigned int countTileIndices(int ax, int ay,
                                  int rx, int ry,
                                  int lx, int ly);

    /// given triangle, marks array elements to be 1 if the triangle contains any tiles of that index
    // entries of tile indices not in the triangle are marked with 0.
    // note that the array passed in should be at least TileIndexData::maxtileindex bytes long!
    // @return the # of tiles contained by the triangle
    unsigned int specifyTileIndices(int ax, int ay,
                                    int rx, int ry,
                                    int lx, int ly,
                    bool *markarray);


    /// given a rectangle and a tile index, marks user bitmap such that spots with that tile index are true
    // all other parts of the array are marked with false.  Markbitmap is row-major (x varies, then y)
    // note that the user array should be of size (x2-x1)*(y1-y2) elements!
    // @return the # of spots marked with 1
    unsigned int makeTileMap(int x1, int y1,
                             int x2, int y2,
                             unsigned int targetindex,
                             bool *markbitmap);

    /// give the default RGB color for a give tile index
    // result is placed in the 3-character array 'rgb'
    void getRGB(unsigned int tileindex, unsigned char *rgb);

protected:
    // the tile data
    tileindex *m_tileindices;

    // the palette--maps from tile index to a RGB base for that tile
    unsigned char *m_tilepalette;

    int m_bitmapxsize, m_bitmapysize;
    int m_logxsize, m_logysize;

    void tileindices_recursive(int ax, int ay,
                                    int rx, int ry,
                                    int lx, int ly,
                    bool *markarray);


};

#endif

