#ifndef NAVIGATION_PATH3D_H
#define NAVIGATION_PATH3D_H
//------------------------------------------------------------------------------
/**
    @class Navigation::Path3D

    Paths in a 3d environment with several properties.

    (C) 2003 RadonLabs GmbH
*/

#include "util/narray.h"
#include "mathlib/vector.h"
#include "foundation/refcounted.h"

//------------------------------------------------------------------------------
namespace Navigation
{
class Path3D : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Path3D);

public:
    /// Destruct.
    virtual ~Path3D();
    /// Remove all information.
    void Clear();
    /// Number of path segments.
    int CountSegments() const;
    /// Extend path by point `v'.
    void Extend(const vector3& v);
    /// Remove an element from the path
    void Remove(int index);
    /// Get point array.
    const nArray<vector3>& GetPoints() const;

private:
    nArray<vector3> points;
};

RegisterFactory(Path3D);

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::Clear()
{
    points.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Path3D::CountSegments() const
{
    return points.Size() - 1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::Extend(const vector3& v)
{
    points.PushBack(v);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Path3D::Remove(int index)
{
    points.EraseQuick(index);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<vector3>&
Path3D::GetPoints() const
{
    return points;
}

} // namespace Navigation
//------------------------------------------------------------------------------
#endif
