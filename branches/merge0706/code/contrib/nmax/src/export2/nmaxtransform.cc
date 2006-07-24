//------------------------------------------------------------------------------
//  nmaxtransform.cc
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxtransform.h"
#include "export2/nmaxbones.h"

//------------------------------------------------------------------------------
/**
*/
nMaxTransform::nMaxTransform()
{
}

//------------------------------------------------------------------------------
/**
*/
nMaxTransform::~nMaxTransform()
{
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxTransform::PositionComponent(const AffineParts &ap)
{
    vector3 position(-ap.t.x, ap.t.z, ap.t.y);
    return position;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxTransform::PositionComponent(Matrix3 &m)
{
    AffineParts ap;
    decomp_affine(m, &ap);
    return nMaxTransform::PositionComponent(ap);
}

//------------------------------------------------------------------------------
/**
*/
quaternion
nMaxTransform::RotationComponent(Matrix3 &m)
{
    AffineParts ap;
    decomp_affine(m, &ap);//!!!expensive
    return nMaxTransform::RotationComponent(ap);
}

//------------------------------------------------------------------------------
/**
*/
quaternion
nMaxTransform::RotationComponent(const AffineParts &ap)
{
    quaternion q(-ap.q.x, ap.q.z, ap.q.y, -ap.q.w);
    return q;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxTransform::ScaleComponent(Matrix3 m, const TCHAR* nodeName)
{
    AffineParts ap;
    decomp_affine(m, &ap);//!!!expensive
    return nMaxTransform::ScaleComponent(ap.u, ap.k, nodeName);
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxTransform::ScaleComponent(const AffineParts &ap, const TCHAR* nodeName)
{
    return nMaxTransform::ScaleComponent(ap.u, ap.k, nodeName);
}

//------------------------------------------------------------------------------
/**
*/
vector3
nMaxTransform::ScaleComponent(const Quat &u, const Point3 &k, const TCHAR* nodeName)
{
    //check if the scale axis system is not equal to the nebula axis system
    if (u.IsIdentity() == 0)
    {
        //convert the scale from max scale axis to nebula scale axis system
        n_printf("WARNING: '%s': There is a nonuniform scale, this may result in unecpected data!.\n", nodeName ? nodeName : "UNKNOWN NODE");

        Matrix3 scaleAxis;
        u.MakeMatrix(scaleAxis, false);

        //the inverse target axis system
        Matrix3 inverseIdent(true);
        inverseIdent.Invert();

        //transform the max scale value (RH axis to LH axis)
        Point3 scaleInIdent = (scaleAxis * inverseIdent) * k;

        //flip the values (needed because of transform from RH/LH)

        //FIXME: clear if the values for animations must be multiplied  with -1.0f
        vector3 scale(-1.0f * scaleInIdent.x, -1.0f * scaleInIdent.y, -1.0f * scaleInIdent.z);
        //vector3 scale(scaleInIdent.x, scaleInIdent.y, scaleInIdent.z);
        return scale;
    }

    return vector3(k.x, k.y, k.z);
}

//------------------------------------------------------------------------------
/**
    3dsmax to n2.
*/
/*
matrix33
nMaxTransform::Convert(Matrix3 &m)
{
    matrix33 result;

    Point3 p;

    p = m.GetRow(0);
    result.M11 = p.x;
    result.M12 = p.z;
    result.M13 = p.y;

    // swap y and z.
    p = m.GetRow(2);
    result.M21 = p.x;
    result.M22 = p.z;
    result.M23 = p.y;

    p = m.GetRow(1);
    result.M31 = p.x;
    result.M32 = p.z;
    result.M33 = p.y;

    return result;
}
*/
//------------------------------------------------------------------------------
/**
    n2 to 3dsmax.
*/
/*
Matrix3
nMaxTransform::Convert(matrix33 &m)
{
    Point3 p;
    Matrix3 result;

    p.x = result.M11;
    p.z = result.M12;
    p.y = result.M13;
    result.SetRow(0, p);

    // swap y and z.
    p.x = result.M21;
    p.z = result.M22;
    p.y = result.M23;
    result.SetRow(2, p);

    p.x = result.M21;
    p.z = result.M22;
    p.y = result.M23;
    result.SetRow(1, p);

    return result;
}
*/

//------------------------------------------------------------------------------
/**
    Retrieves local transformation from the given node.
*/
static
Matrix3 UniformMatrix(Matrix3 sMatrix)
{
    AffineParts sAP;
    Matrix3 sResult;

    decomp_affine(sMatrix, &sAP);
    sAP.q.MakeMatrix(sResult);
    sResult.SetRow(3, sAP.t);

    return sResult;
}

//------------------------------------------------------------------------------
/**
    Retrieves local transformation from the given node.

    -22-Sep-05    kims    Fixed wrong rotation problem of non-biped mirrored bone.(#322 on Bugzilla)
                          Thanks Jind©ªich Rohlik for pointing it out.
*/
Matrix3 nMaxTransform::GetLocalTM(INode *inode, TimeValue time)
{
    Matrix3 result;

    Matrix3 nodeTM = inode->GetNodeTM(time);
    Matrix3 parentTM = inode->GetParentTM(time);

    // if the given node is bone, we should call NoScale()
    // before we multiply two matrices to get correct result.
    if (nMaxBoneManager::Instance()->IsBone(inode))
    {
        nodeTM   = UniformMatrix(nodeTM);
        parentTM = UniformMatrix(parentTM);
    }

    result = nodeTM * Inverse(parentTM);

    return result;
}
