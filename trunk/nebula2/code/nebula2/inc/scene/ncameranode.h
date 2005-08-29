#ifndef N_CAMERANODE_H
#define N_CAMERANODE_H
//------------------------------------------------------------------------------
/**
    @classnCameraNode
    @ingroup Camera

    Extends nCameraNode. Is a fixed camera with no intelligence.  

    author: matthias
    (C) 2004 RadonLabs GmbH
*/

#include "scene/nabstractcameranode.h"

//------------------------------------------------------------------------------
class nCameraNode : public nAbstractCameraNode
{
public:
    /// constructor
    nCameraNode();
    /// destructor
    virtual ~nCameraNode();

    /// function wich updates the camera 
    virtual bool RenderCamera(const matrix44& modelWorldMatrix, const matrix44& viewMatrix, const matrix44& projectionMatrix);    

protected:
    /// compute the viewmatrix
    matrix44 ComputeViewMatrix(const vector3& cameraPosition, const vector3& cameraDirection);    
};

//------------------------------------------------------------------------------
#endif