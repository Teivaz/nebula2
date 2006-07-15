//------------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#ifndef N_MAP_H
#define N_MAP_H
//---------------------------------------------------------------------------
/**
    @class nMap
    @ingroup NMapContribModule
    @brief Provides data source for nMapNode.

    Basis for game-specific map classes.

    Maps must be square and should ideally be in nice numbers for rendering.

    @todo Make nMapNode register itself as a dependent node to be notified
    whenever the heightmap data changes.

    (C) 2002    ling
*/
#include "kernel/nroot.h"
#include "kernel/nfileserver2.h"
#include "mathlib/bbox.h"
#include "kernel/nautoref.h"
#include "resource/nresource.h"

//---------------------------------------------------------------------------
/**
    @class MapPoint
    @ingroup NMapContribModule
    @brief Maintains data for a map point which is a corner of a tile.
*/
struct MapPoint
{
    vector3 coord;      ///< Vertex coordinates
    vector3 normal;     ///< Vertex normal :TODO: convert to packed version
};

//typedef unsigned long ulong;

//------------------------------------------------------------------------------
class  nMap : public nRoot
{
public:
    /// constructor
    nMap();
    /// destructor
    virtual ~nMap();
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Get the map dimension
    int GetDimension() const;
    virtual float GetHeight(float x, float z) const;
    virtual void GetNormal(float x, float z, vector3& normal) const;
    virtual bool GetIntersect(const line3& line, vector3& contact) const;

    /// Get map point, really for internal-ish use
    const MapPoint& GetPoint(int x, int z) const;

    // Commands
    virtual void SetHeightMap(const char*);
    virtual const char* GetHeightMap();
    virtual void SetGridInterval(float spacing);
    virtual float GetGridInterval() const;
    virtual void SetHeightRange(float min, float max);
    virtual float GetHeightRangeMin() const;
    virtual float GetHeightRangeMax() const;

    /// Load map data.
    virtual void LoadMap();

private:
    int ClampToBounds( int coord ) const;
    bool LoadFromImage(const nString& abs_path);
    void CalculateNormals();
    float Interpolate(float f, float a, float b) const;
    bool CheckIntersect(int x, int z, const line3&, vector3&) const;

    nAutoRef<nFileServer2> refFileServer;

    /// Current data is dirty, only true if uninitialised
    bool isDataDirty;

    /// Heightmap image file
    //nResource imagePath;
    nString imagePath; 
    /// Grid spacing
    float gridInterval;
    /// Minimum terrain height
    float heightMin;
    /// Maximum terrain height
    float heightMax;
    /// Calculated height scaling
    float heightScale;

    /// Number of map points, note Y is the same for the meanwhile
    int mapDimension;
    /// Complete terrain bounding box
    bbox3 boundingBox;
    /// Point data, or heightmap data
    MapPoint* pointData;
};

//---------------------------------------------------------------------------
/**
*/
inline
int
nMap::GetDimension() const
{
    return mapDimension;
}

//---------------------------------------------------------------------------
/**
*/
inline
const MapPoint&
nMap::GetPoint(int x, int z) const
{
    n_assert(x >= 0 && x < mapDimension);
    n_assert(z >= 0 && z < mapDimension);
    return pointData[x + z*mapDimension];
}

//---------------------------------------------------------------------------
/**
    @brief Standard interpolate method.
    Is there one in Nebula I have not seen?
*/
inline 
float 
nMap::Interpolate(float scale, float a, float b) const
{
    return scale*(1-b) + (a*b);
}

//---------------------------------------------------------------------------

#endif
