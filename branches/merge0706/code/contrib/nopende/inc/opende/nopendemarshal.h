#ifndef N_OPENDEMARSHAL_H
#define N_OPENDEMARSHAL_H
//----------------------------------------------------------------------------
/**
    This is a set of functions that handle conversion between ODE and Nebula
    data types.
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#include "mathlib/matrix.h"
#include "mathlib/vector.h"
#include "mathlib/quaternion.h"
#include "mathlib/plane.h"
//----------------------------------------------------------------------------
namespace nOpendeMarshal {

//----------------------------------------------------------------------------
inline
void dMatrix3_2_matrix44( const dMatrix3 Rotation, matrix44& out )
{
    out.M11 = float(Rotation[0]);
    out.M21 = float(Rotation[1]);
    out.M31 = float(Rotation[2]);

    out.M12 = float(Rotation[4]);
    out.M22 = float(Rotation[5]);
    out.M32 = float(Rotation[6]);

    out.M13 = float(Rotation[8]);
    out.M23 = float(Rotation[9]);
    out.M33 = float(Rotation[10]);

    out.M41 = 0.0f;
    out.M42 = 0.0f;
    out.M43 = 0.0f;

    out.M14 = 0.0f;
    out.M24 = 0.0f;
    out.M34 = 0.0f;
    out.M44 = 1.0f;
}

//----------------------------------------------------------------------------
inline
void matrix44_2_dMatrix3( const matrix44& mat, dMatrix3 rotation )
{
    rotation[0]   = mat.M11;
    rotation[1]   = mat.M21;
    rotation[2]   = mat.M31;
    rotation[3]   = 0;
    rotation[4]   = mat.M12;
    rotation[5]   = mat.M22;
    rotation[6]   = mat.M32;
    rotation[7]   = 0;
    rotation[8]   = mat.M13;
    rotation[9]   = mat.M23;
    rotation[10]  = mat.M33;
    rotation[11]  = 0;
}

//----------------------------------------------------------------------------
inline
void dMatrix3_2_matrix33( const dMatrix3 Rotation, matrix33& out )
  {
    out.M11 = float(Rotation[0]);
    out.M21 = float(Rotation[1]);
    out.M31 = float(Rotation[2]);

    out.M12 = float(Rotation[4]);
    out.M22 = float(Rotation[5]);
    out.M32 = float(Rotation[6]);

    out.M13 = float(Rotation[8]);
    out.M23 = float(Rotation[9]);
    out.M33 = float(Rotation[10]);
  } 

//----------------------------------------------------------------------------
inline
void matrix33_2_dMatrix3( const matrix33& mat, dMatrix3 rotation )
{
    rotation[0]   = mat.M11;
    rotation[1]   = mat.M21;
    rotation[2]   = mat.M31;
    rotation[3]   = 0;
    rotation[4]   = mat.M12;
    rotation[5]   = mat.M22;
    rotation[6]   = mat.M32;
    rotation[7]   = 0;
    rotation[8]   = mat.M13;
    rotation[9]   = mat.M23;
    rotation[10]  = mat.M33;
    rotation[11]  = 0;
}

//----------------------------------------------------------------------------
/** 
    @brief Convert an ODE quaternion to a Nebula one.
*/
inline
void dQuat_2_quat( const dQuaternion source, quaternion& res )
{
    res.set( float(source[1]), float(source[2]), 
             float(source[3]), float(source[0]) );
}
  
//----------------------------------------------------------------------------
/**
    @brief Convert a Nebula quaternion to an ODE one.
*/
inline
void quat_2_dQuat( const quaternion& source, dQuaternion res )
{
    res[0] = source.w;
    res[1] = source.x;
    res[2] = source.y;
    res[3] = source.z;
}

//----------------------------------------------------------------------------
inline
vector3 dVector3_2_vector3( const dVector3 in )
{
    return vector3( float(in[0]), float(in[1]), float(in[2]) );
}

//----------------------------------------------------------------------------
inline
void dVector3_2_vector3( const dVector3 in, vector3& out )
{
    out.set( float(in[0]), float(in[1]), float(in[2]) );
}

//----------------------------------------------------------------------------
inline
void vector3_2_dVector3( const vector3& in, dVector3 out )
{
    out[0] = in.x;
    out[1] = in.y;
    out[2] = in.z;
}

//----------------------------------------------------------------------------
inline
void vector4_2_dVector4( const vector4& in, dVector4 out )
{
    out[0] = in.x;
    out[1] = in.y;
    out[2] = in.z;
    out[3] = in.w;
}

//----------------------------------------------------------------------------
inline
void dVector4_2_vector4( const dVector4 in, vector4& out )
{
    out.set( float(in[0]), float(in[1]), float(in[2]), float(in[3]) );
}

//----------------------------------------------------------------------------
/*
    @brief Convert an ODE plane into a Nebula plane.
    @fixme Not sure if this works yet :)
*/
inline 
void dPlane_2_plane( const dVector4 in, plane& out )
{
    out.set( -in[0], -in[1], -in[2], in[3] );
}

//----------------------------------------------------------------------------
/*
    @brief Convert a Nebula plane into an ODE plane.
    @fixme Not sure if this works yet :)
*/
inline 
void plane_2_dPlane( const plane& in, dVector4 out )
{
    out[0] = -in.a;
    out[1] = -in.b;
    out[2] = -in.c;
    out[3] = in.d;
}

//----------------------------------------------------------------------------
inline
vector3 dRealPointer_2_vector3( const dReal* in )
{
    return vector3( float(in[0]), float(in[1]), float(in[2]) );
}

//----------------------------------------------------------------------------
inline
void dRealPointer_2_vector3( const dReal* in, vector3& out )
{
    out.set( float(in[0]), float(in[1]), float(in[2]) );
}

//----------------------------------------------------------------------------
inline 
matrix33 dRealPointer_2_matrix33( const dReal* in )
{
    const dMatrix3& rotation = *(const dMatrix3*)in;
    matrix33 out;
    nOpendeMarshal::dMatrix3_2_matrix33( rotation, out );
    return out;
}

//----------------------------------------------------------------------------
inline 
void dRealPointer_2_matrix33( const dReal* in, matrix33& out )
{
    const dMatrix3& rotation = *(const dMatrix3*)in;
    nOpendeMarshal::dMatrix3_2_matrix33( rotation, out );
}

//----------------------------------------------------------------------------
inline 
quaternion dRealPointer_2_quat( const dReal* in )
{
    const dQuaternion& quat = *(const dQuaternion*)in;
    quaternion out;
    nOpendeMarshal::dQuat_2_quat( quat, out );
    return out;
}

//----------------------------------------------------------------------------
inline 
void dRealPointer_2_quat( const dReal* in, quaternion& out )
{
    const dQuaternion& quat = *(const dQuaternion*)in;
    nOpendeMarshal::dQuat_2_quat( quat, out );
}

//----------------------------------------------------------------------------
inline
const char* GeomClassToString( int geomClass )
{
    switch ( geomClass )
    {
        case dSphereClass:
            return "sphere";
        case dBoxClass:
            return "box";
        case dCylinderClass:
            return "cylinder";
        case dCCylinderClass:
            return "capsule";
        case dRayClass:
            return "ray";
        case dPlaneClass:
            return "plane";
        case dTriMeshClass:
            return "trimesh";
        case dGeomTransformClass:
            return "transform";
        default:
            return "unknown";
    }   
}

//----------------------------------------------------------------------------
inline
int StringToGeomClass( const char* geomClass )
{
    if ( strcmp( "sphere", geomClass ) == 0 )
        return dSphereClass;
    else if ( strcmp( "box", geomClass ) == 0 )
        return dBoxClass;
    else if ( strcmp( "cylinder", geomClass ) == 0 )
        return dCylinderClass;
    else if ( strcmp( "capsule", geomClass ) == 0 )
        return dCCylinderClass;
    else if ( strcmp( "ray", geomClass ) == 0 )
        return dRayClass;
    else if ( strcmp( "plane", geomClass ) == 0 )
        return dPlaneClass;
    else if ( strcmp( "trimesh", geomClass ) == 0 )
        return dTriMeshClass;
    else if ( strcmp( "transform", geomClass ) == 0 )
        return dGeomTransformClass;
    else
        n_error( "%s is not a valid geom class name!", geomClass );
    return 0;
}

//----------------------------------------------------------------------------

} // namespace nOpendeMarshal

//----------------------------------------------------------------------------
#endif // N_OPENDEMARSHAL_H
