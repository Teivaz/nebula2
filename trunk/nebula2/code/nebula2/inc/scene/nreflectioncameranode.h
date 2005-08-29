#ifndef N_REFLECTIONCAMERANODE_H
#define N_REFLECTIONCAMERANODE_H
//------------------------------------------------------------------------------
/**
    @class nReflectionCameraNode
    @ingroup Camera

    Extends nClippingCameraNode.
    Special Clippingcamera witch computes a Reflectionviewmatrix.

    author: matthias
    (C) 2004 RadonLabs GmbH
*/

#include "scene/nclippingcameranode.h"

//------------------------------------------------------------------------------
class nReflectionCameraNode : public nClippingCameraNode
{
public:
    /// construktor
    nReflectionCameraNode();
    /// destruktor
    virtual ~nReflectionCameraNode();

    /// function witch updates the camera 
    virtual bool RenderCamera(const matrix44& modelWorldMatrix, const matrix44& viewMatrix, const matrix44& projectionMatrix);

protected:
    /// compute a reflection camera matrix
    matrix44 ComputeReflectionViewMatrix(const matrix44& viewMatrix, const matrix44& modelMatrix, const bool& isOnFrontSideOfPlane, const float& dist);
};
//------------------------------------------------------------------------------
#endif