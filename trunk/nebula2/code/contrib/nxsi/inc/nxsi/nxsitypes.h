//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
//#include <mathlib/vector.h>
//-----------------------------------------------------------------------------

struct nXSIWeight
{
    float joints[4];
    float weights[4];
    int count;
};

#define VECTOR3_DEG2RAD(v) {(v).x = n_deg2rad((v).x); \
                            (v).y = n_deg2rad((v).y); \
                            (v).z = n_deg2rad((v).z);}

//-----------------------------------------------------------------------------
// Eof