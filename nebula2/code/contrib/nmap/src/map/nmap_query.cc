//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------
#include "map/nmap.h"
#include "mathlib/line.h"
#include "mathlib/triangle.h"

/**
    @brief Ensures that the given map point is actually on the map
*/
int 
nMap::ClampToBounds( int coord ) const 
{
    if (coord < 0 )
    {
        coord = 0;
    }
    else if( coord > mapDimension-2 )
    {
        coord = mapDimension-2;
    }
    return coord;
}

/**
    @brief Return the interpolated height at a supplied point.
*/
float 
nMap::GetHeight(float x, float z) const
{
    n_assert( 0 != mapDimension && "nMap::GetHeight: Perhaps the map hasn't been loaded yet?" );
    float temp;
    float x_frac = modff(x / gridInterval, &temp);
    int x_int = ClampToBounds(static_cast<int>(temp));
    float z_frac = modff(z / gridInterval, &temp);
    int z_int = ClampToBounds(static_cast<int>(temp));

    float nw = GetPoint(x_int, z_int).coord.y;
    float se = GetPoint(x_int+1, z_int+1).coord.y;

    float ne, sw;
    // ne triangle
    if (z_frac <= x_frac)
    {
        ne = GetPoint(x_int+1, z_int).coord.y;
        sw = nw+se - ne;
    }
    // sw triangle
    else
    {
        sw = GetPoint(x_int, z_int+1).coord.y;
        ne = nw+se - sw;
    }

    float top = Interpolate(nw, ne, x_frac);
    float bottom = Interpolate(sw, se, x_frac);
    
    return Interpolate(top, bottom, z_frac);
}

/**
    @brief Get the triangle mesh normal at a given point.
    Useful for physics and figuring out normal force from terrain.
    @param x
    @param z
    @param normal the output normal result
*/
void 
nMap::GetNormal(float x, float z, vector3& normal) const
{
    float temp;
    float x_frac = modff(x / gridInterval, &temp);
    int x_int = ClampToBounds(static_cast<int>(temp));
    float z_frac = modff(z / gridInterval, &temp);
    int z_int = ClampToBounds(static_cast<int>(temp));

    // Northeast triangle
    if (x_frac > z_frac)
    {
        // Special case along x
        if (0.0f == z_frac)
        {
            const vector3& nw = GetPoint(x_int, z_int).coord;
            const vector3& ne = GetPoint(x_int+1, z_int).coord;
            vector3 sw = GetPoint(x_int, z_int+1).coord;
            sw.y = nw.y;

            normal.set((sw - nw) * (ne - nw));
            normal.norm();
        }
        else
        {
            const vector3& nw = GetPoint(x_int, z_int).coord;
            const vector3& ne = GetPoint(x_int+1, z_int).coord;
            const vector3& se = GetPoint(x_int+1, z_int+1).coord;

            normal.set((nw - ne) * (se - ne));
            normal.norm();
        }
    }
    // Southwest triangle
    else if (x_frac < z_frac)
    {
        // Special case along z
        if (0.0f == x_frac)
        {
            const vector3& nw = GetPoint(x_int, z_int).coord;
            vector3 ne = GetPoint(x_int+1, z_int).coord;
            const vector3& sw = GetPoint(x_int, z_int+1).coord;
            ne.y = nw.y;

            normal.set((sw - nw) * (ne - nw));
            normal.norm();
        }
        else
        {
            const vector3& nw = GetPoint(x_int, z_int).coord;
            const vector3& sw = GetPoint(x_int, z_int+1).coord;
            const vector3& se = GetPoint(x_int+1, z_int+1).coord;

            normal.set((se - sw) * (nw - sw));
            normal.norm();
        }
    }
    // Special case along diagonal
    // x_frac == z_frac
    else
    {
        // At zero, just go make up the normal
        if (0.0f == x_frac)
        {
            normal = GetPoint(x_int, z_int).normal;
        }
        // Create a plane
        else
        {
            const vector3& nw = GetPoint(x_int, z_int).coord;
            vector3 sw = GetPoint(x_int, z_int+1).coord;
            const vector3& se = GetPoint(x_int+1, z_int+1).coord;
            sw.y = (nw.y + se.y) / 2;

            normal.set((se - sw) * (nw - sw));
            normal.norm();
        }
    }
}

/**
    @brief Returns exact line intersection against terrain.
    Should consider optimising this as it would probably get called often.

    Could be optimised by following the method in:
    "Fast Line-Edge Intersections on a Uniform Grid"
    by Andrew Shapira from "Graphics Gems", Academic Press, 1990

    @param line the line to check terrain against
    @param contact position of terrain/line contact
    @return true if the line intersects with the terrain.

    @bug Sometimes does not deal with near vertical rays.
    @bug Occassionally misses sharp peaks on the terrain.
*/
bool 
nMap::GetIntersect(const line3& line, vector3& contact) const
{
    // Obtain point on or inside terrain bounding box
    // to save doing irrelevant terrain checks
    if (false == boundingBox.intersect(line, contact))
    {
        return false;
    }
    vector3 pos = contact;
    
    // Special case, vertical ray
    if (0.0f == line.m.x && 0.0f == line.m.z)
    {
        contact.y = GetHeight(contact.x, contact.z);
        return true;
    }

    // Maximise sample interval
    vector3 dir = line.m;
    dir = dir * (1 / n_max(fabsf(dir.x), fabsf(dir.z)));
    dir *= gridInterval;

    // Near vertical ray, shorten so it does not skip terrain
    if (fabsf(contact.y - boundingBox.vmin.y) < fabsf(dir.y))
    {
        dir *= 0.5f * (fabsf(dir.y) / (boundingBox.vmax.y - boundingBox.vmin.y));
    }

    // :HACK: Counter to track whether the line goes out of scope,
    // this is needed to let checks come in from east and south edges where
    // the contact starts at the boundary and is out of scope
    int boundary = 0;

    line3 ray(contact, line.end());
    //int iterations = 1 + int(ray.m.len_squared() / (ray.m % dir));
    int iterations = 1 + int(ray.m.lensquared() / (ray.m % dir));
    for (int i = 0; i < iterations; ++i)
    {
        int x = int(floorf(pos.x / this->gridInterval));
        int z = int(floorf(pos.z / this->gridInterval));

        // Room for optimisation, precalculate number of iterations
        // before line goes out of scope
        if (x < 0 || x >= this->mapDimension-1 ||
            z < 0 || z >= this->mapDimension-1 ||
            pos.y < boundingBox.vmin.y || pos.y > boundingBox.vmax.y)
        {
            ++boundary;
            if (boundary > 4)
            {
                return false;
            }
        }
        // Intersects, check against triangle for exact intersection
        // Duplicated checks for subsequent loops, could probably speed it up
        else if (GetPoint(x, z).coord.y > pos.y ||
                 GetPoint(x+1, z).coord.y > pos.y ||
                 GetPoint(x, z+1).coord.y > pos.y ||
                 GetPoint(x+1, z+1).coord.y > pos.y)
        {
            // Opposite direction of ray
            int x_offset = (0.0f < dir.x) ? -1 : 1;
            int z_offset = (0.0f < dir.z) ? -1 : 1;

            // :HACK: Check for previous triangles instead of current,
            // slower but more robust
            if (true == CheckIntersect(x+x_offset, z+z_offset, line, contact) ||
                true == CheckIntersect(x+x_offset, z, line, contact) ||
                true == CheckIntersect(x, z+z_offset, line, contact))
            {
                return true;
            }

            // Check against present and adjacent quads
            if (true == CheckIntersect(x, z, line, contact))
            {
                return true;
            }
        }

        // Does not intersect, next
        pos += dir;
    }

    return false;
}

/**
    @brief Check intersection between a point on line and map tile.
    Makes two line-triangle intersection checks for each tile, i.e. quad.
*/
bool 
nMap::CheckIntersect(int x, int z,
                          const line3& line,
                          vector3& contact) const
{
    // Always false for out of bounds
    if (x < 0 || x >= this->mapDimension-1 ||
        z < 0 || z >= this->mapDimension-1)
    {
        return false;
    }

    float ipos;
    triangle tri(GetPoint(x, z).coord,
                 GetPoint(x, z+1).coord,
                 GetPoint(x+1, z+1).coord);

    if (true == tri.intersect_both_sides(line, ipos))
    {
        contact = line.ipol(ipos);
        return true;
    }

    // Try other triangle
    tri.set(GetPoint(x, z).coord,
            GetPoint(x+1, z+1).coord,
            GetPoint(x+1, z).coord);
    if (true == tri.intersect_both_sides(line, ipos))
    {
        contact = line.ipol(ipos);
        return true;
    }

    return false;
}
