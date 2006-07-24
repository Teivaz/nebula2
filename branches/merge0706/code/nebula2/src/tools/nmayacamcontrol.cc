//------------------------------------------------------------------------------
//  nmayacamcontrol.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nmayacamcontrol.h"

//------------------------------------------------------------------------------
/**
    The Constructor
*/
nMayaCamControl::nMayaCamControl():
    defCenterOfInterest(0.0f, 0.0f, 0.0f),
    defEyePos(9.0f, 9.0f, 9.0f),
    defUpVec(0.0f, 1.0f, 0.0f),
    viewerDistance(0.0f),
    lookButton(false),
    panButton(false),
    zoomButton(false),
    resetButton(false),
    sliderLeft(0.0f),
    sliderRight(0.0f),
    sliderUp(0.0f),
    sliderDown(0.0f)
{
    this->Initialize();
}

//------------------------------------------------------------------------------
/**
    Calculates several values in order to initialize the view
*/
void
nMayaCamControl::Initialize()
{
    this->centerOfInterest = this->defCenterOfInterest;
    // calculate distance between camera and center of interest
    this->viewerDistance = vector3::distance(this->defCenterOfInterest, this->defEyePos);
    // initialize theta and rho
    vector3 viewerDirection = this->defEyePos - this->defCenterOfInterest;
    viewerDirection.norm();
    this->viewerAngles.set(viewerDirection);
    this->viewerAngles.theta -= N_PI*0.5f;
    if (viewerDirection.y < 0) this->viewerAngles.theta = - this->viewerAngles.theta;
    this->Update();
}

//------------------------------------------------------------------------------
/**
    Resets the view to initial value
*/
void
nMayaCamControl::Reset()
{
    this->resetButton = false;
    this->Initialize();
}

//------------------------------------------------------------------------------
/**
    Handle input for the Maya control model.
*/
void
nMayaCamControl::Update()
{
    // corresponds with a distance of 1.0f
    const float defLookVelocity = 0.25f;
    const float defPanVelocity  = 0.08f;
    const float defZoomVelocity = 0.25f;

    const float minPanVelocity = 0.08f;
    const float minZoomVelocity = 0.50f;

    float lookVelocity = defLookVelocity;
    // multiply with viewerDistance to get a camera-movement-speed corresponding to the actual distance.
    float panVelocity  = defPanVelocity  *  viewerDistance;
    panVelocity = (minPanVelocity > panVelocity)? minPanVelocity : panVelocity;
    // multiply with viewerDistance to get a camera-movement-speed corresponding to the actual distance with a minimum of defZoomVelocity
    // float zoomVelocity = (1.0f <= viewerDistance)? (defZoomVelocity * viewerDistance): defZoomVelocity;
    float zoomVelocity = defZoomVelocity  *  viewerDistance;
    zoomVelocity = (minZoomVelocity > zoomVelocity)? minZoomVelocity : zoomVelocity;

    float panHori  = 0.0f;
    float panVert  = 0.0f;
    float zoomHori = 0.0f;
    float zoomVert = 0.0f;
    float lookHori = 0.0f;
    float lookVert = 0.0f;

    // Handle maya controls look/pan/zoom
    if (true == this->lookButton)
    {
        lookHori = this->sliderLeft - this->sliderRight;
        lookVert = this->sliderDown - this->sliderUp;
    }
    if (true == this->panButton)
    {
        panHori = this->sliderLeft - this->sliderRight;
        panVert = this->sliderDown - this->sliderUp;
    }
    if (true == this->zoomButton)
    {
        zoomHori = this->sliderLeft - this->sliderRight;
        zoomVert = this->sliderDown - this->sliderUp;
    }

    // Handle reset of view
    if (true == this->resetButton)
    {
       this->Reset();
    }

    // pushes the center of view in front of the camera to avoid a negative distance
    if (0.0f > this->viewerDistance)
    {
        this->centerOfInterest = this->viewMatrix.pos_component();
        this->viewerDistance = 0.0f;
    }

    // handle viewer move
    vector3 horiMoveVector(this->viewMatrix.x_component() * panHori * panVelocity);
    vector3 vertMoveVector(this->viewMatrix.y_component() * panVert * panVelocity);
    this->centerOfInterest += horiMoveVector + vertMoveVector;

    // handle viewer zoom
    float horiZoomMoveVector(       zoomHori * zoomVelocity);
    float vertZoomMoveVector((-1) * zoomVert * zoomVelocity);
    this->viewerDistance += horiZoomMoveVector + vertZoomMoveVector ;

    // handle viewer rotation
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    // apply changes
    this->viewMatrix.ident();
    this->viewMatrix.translate(vector3(0.0f, 0.0f, viewerDistance));
    this->viewMatrix.rotate_x(this->viewerAngles.theta);
    this->viewMatrix.rotate_y(this->viewerAngles.rho);
    this->viewMatrix.translate(this->centerOfInterest);
}
