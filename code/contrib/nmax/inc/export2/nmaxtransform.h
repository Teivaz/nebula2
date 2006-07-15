//------------------------------------------------------------------------------
//  nmaxtransform.h
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#ifndef N_MAXTRANSFORM_H
#define N_MAXTRANSFORM_H
//------------------------------------------------------------------------------
/**
    @class nMaxTransform
    @ingroup NebulaMaxExport2Contrib

    @brief

*/
#include "mathlib/matrix.h"

//------------------------------------------------------------------------------
class nMaxTransform
{
public:
    nMaxTransform();
    virtual ~nMaxTransform();

    static vector3 PositionComponent(const AffineParts &ap);
    static vector3 PositionComponent(Matrix3 &m);
    static quaternion RotationComponent(Matrix3 &m);
    static quaternion RotationComponent(const AffineParts &ap);
    static vector3 ScaleComponent(Matrix3 m, const TCHAR* nodeName);
    static vector3 ScaleComponent(const AffineParts &ap, const TCHAR* nodeName);
    static vector3 ScaleComponent(const Quat &u, const Point3 &k, const TCHAR* nodeName);

    //static matrix33 Convert(Matrix3 &m);
    //static Matrix3 Convert(matrix33 &m);

    static Matrix3 GetLocalTM(INode *inode, TimeValue time);

};
//------------------------------------------------------------------------------
#endif