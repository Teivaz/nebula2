//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
// nXSI animation functions
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <mathlib/quaternion.h>
#include <iostream>

using std::cerr;

//-----------------------------------------------------------------------------

nAnimBuilder::Curve BuildJointAnimationCurve(
    CSLModel* joint, vector3& basepose, nAnimBuilder::Curve::IpolType type,
    CSLTemplate::EFCurveType xType, CSLTemplate::EFCurveType yType, CSLTemplate::EFCurveType zType,
    int startKey, int endKey)
{
    int keyCount = endKey - startKey + 1;
    nAnimBuilder::Curve curve;
    int f, k;

    // set curve infos
    curve.SetIpolType(type);
    curve.SetFirstKeyIndex(0);

    // get fcurves
    CSLFCurve* fcurves[3];
    fcurves[0] = joint->GetSpecificFCurve(xType);
    fcurves[1] = joint->GetSpecificFCurve(yType);
    fcurves[2] = joint->GetSpecificFCurve(zType);

    // add keys
    for (k = 0, f = startKey; k < keyCount; k++, f++)
    {
        nAnimBuilder::Key key;
        vector4 keyValue(basepose);

        if (fcurves[0]) keyValue.x = fcurves[0]->GetValueAtPrevFrame((float)f);
        if (fcurves[1]) keyValue.y = fcurves[1]->GetValueAtPrevFrame((float)f);
        if (fcurves[2]) keyValue.z = fcurves[2]->GetValueAtPrevFrame((float)f);
        keyValue.w = 0.0f;

        if (type == nAnimBuilder::Curve::QUAT)
        {
            quaternion quat;
            quat.set_rotate_xyz(keyValue.x, keyValue.y, keyValue.z);
            key.SetX(quat.x);
            key.SetY(quat.y);
            key.SetZ(quat.z);
            key.SetW(quat.w);
        } else {
            key.Set(keyValue);
        }
        curve.SetKey(k, key);
    }
    return curve;
}

void nXSI::BuildJointAnimations(const nArray<CSLModel*>& jointList)
{
    int jointCount = jointList.Size();
    int j;

    int startKey = (int)(this->xsiScene.SceneInfo()->GetStart());
    int endKey = (int)(this->xsiScene.SceneInfo()->GetEnd());
    int keyCount = endKey - startKey + 1;
    float keyTime = 1.f / this->xsiScene.SceneInfo()->GetFrameRate();

    // create animation group
    nAnimBuilder::Group group;
    group.SetLoopType(nAnimBuilder::Group::REPEAT);
    group.SetStartKey(0);
    group.SetNumKeys(keyCount);
    group.SetKeyTime(keyTime);
    group.SetKeyStride(0);

    // build key array
    for (j = 0; j < jointCount; j++)
    {
        CSLModel* joint = jointList[j];

        // handle translation curve
        group.AddCurve( BuildJointAnimationCurve(
            joint, (vector3&)(joint->Transform()->GetTranslation()), nAnimBuilder::Curve::LINEAR,
            CSLTemplate::SI_TRANSLATION_X, CSLTemplate::SI_TRANSLATION_Y, CSLTemplate::SI_TRANSLATION_Z,
            startKey, endKey));

        // handle rotation curve
        group.AddCurve( BuildJointAnimationCurve(
            joint, (vector3&)(joint->Transform()->GetEulerRotation()), nAnimBuilder::Curve::QUAT,
            CSLTemplate::SI_ROTATION_X, CSLTemplate::SI_ROTATION_Y, CSLTemplate::SI_ROTATION_Z,
            startKey, endKey));

        // handle scaling curve
        group.AddCurve( BuildJointAnimationCurve(
            joint, (vector3&)(joint->Transform()->GetScale()), nAnimBuilder::Curve::LINEAR,
            CSLTemplate::SI_SCALING_X, CSLTemplate::SI_SCALING_Y, CSLTemplate::SI_SCALING_Z,
            startKey, endKey));
    }

    this->animBuilder.AddGroup(group);
}

//-----------------------------------------------------------------------------
// Eof