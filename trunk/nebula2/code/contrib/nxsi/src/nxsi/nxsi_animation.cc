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
    CSLTransform* jointTransform, vector3& basepose, nAnimBuilder::Curve::IpolType type,
    CSLTemplate::EFCurveType xType, CSLTemplate::EFCurveType yType, CSLTemplate::EFCurveType zType,
    float startKey, int keyCount)
{
    nAnimBuilder::Curve curve;
    vector4 keyValue;
    float f;
    int k;

    // set curve infos
    curve.SetIpolType(type);
    curve.SetFirstKeyIndex(0);

    // get fcurves
    CSLFCurve* fcurves[3];
    fcurves[0] = jointTransform->GetSpecificFCurve(xType);
    fcurves[1] = jointTransform->GetSpecificFCurve(yType);
    fcurves[2] = jointTransform->GetSpecificFCurve(zType);

    // add keys
    for (k = 0, f = startKey; k < keyCount; k++, f++)
    {
        nAnimBuilder::Key key;
        keyValue = basepose;

        if (fcurves[0] && (fcurves[0]->Evaluate(f) == SI_SUCCESS))
        {
            keyValue.x = fcurves[0]->GetLastEvaluation();
        }
        if (fcurves[1] && (fcurves[1]->Evaluate(f) == SI_SUCCESS))
        {
            keyValue.y = fcurves[1]->GetLastEvaluation();
        }
        if (fcurves[2] && (fcurves[2]->Evaluate(f) == SI_SUCCESS))
        {
            keyValue.z = fcurves[2]->GetLastEvaluation();
        }
        keyValue.w = 0.0f;

        if (type == nAnimBuilder::Curve::QUAT)
        {
            quaternion quat;
            quat.set_rotate_xyz(n_deg2rad(keyValue.x), n_deg2rad(keyValue.y), n_deg2rad(keyValue.z));
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

void nXSI::BuildJointAnimations(const nArray<CSLModel*>& jointList, int jointCount)
{
    int j;

    float startKey = this->xsiScene.SceneInfo()->GetStart();
    float endKey = this->xsiScene.SceneInfo()->GetEnd();
    int keyCount = (int)(endKey - startKey + 1);
    float keyTime = 1.f / this->xsiScene.SceneInfo()->GetFrameRate();

    // create animation group
    nAnimBuilder::Group group;
    group.SetLoopType(nAnimBuilder::Group::REPEAT);
    group.SetStartKey(0);
    group.SetNumKeys(keyCount);
    group.SetKeyTime(keyTime);
    group.SetKeyStride(1);

    // build key array
    for (j = 0; j < jointCount; j++)
    {
        CSLModel* joint = jointList[j];

        // handle translation curve
        group.AddCurve( BuildJointAnimationCurve(
            joint->Transform(), (vector3&)(joint->Transform()->GetTranslation()), nAnimBuilder::Curve::LINEAR,
            CSLTemplate::SI_TRANSLATION_X, CSLTemplate::SI_TRANSLATION_Y, CSLTemplate::SI_TRANSLATION_Z,
            startKey, keyCount));

        // handle rotation curve
        group.AddCurve( BuildJointAnimationCurve(
            joint->Transform(), (vector3&)(joint->Transform()->GetEulerRotation()), nAnimBuilder::Curve::QUAT,
            CSLTemplate::SI_ROTATION_X, CSLTemplate::SI_ROTATION_Y, CSLTemplate::SI_ROTATION_Z,
            startKey, keyCount));

        // handle scaling curve
        group.AddCurve( BuildJointAnimationCurve(
            joint->Transform(), (vector3&)(joint->Transform()->GetScale()), nAnimBuilder::Curve::LINEAR,
            CSLTemplate::SI_SCALING_X, CSLTemplate::SI_SCALING_Y, CSLTemplate::SI_SCALING_Z,
            startKey, keyCount));
    }

    this->animBuilder.AddGroup(group);
}

//-----------------------------------------------------------------------------
// Eof