#ifndef N_MAYACAMCONTROL_H
#define N_MAYACAMCONTROL_H
//------------------------------------------------------------------------------
/**
    @class nMayaCamControl
    @ingroup Tools

    Implements a Maya camera control model.

    (C) 2004 RadonLabs GmbH
*/
#include "mathlib/polar.h"
#include "mathlib/matrix.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "misc/nwatched.h"

//------------------------------------------------------------------------------
class nMayaCamControl
{
public:
    /// constructor
    nMayaCamControl();

    /// initialize the object
    void Initialize();
    /// update view matrix
    void Update();
    /// reset of View
    void Reset();

    /// set default center of interest - call initialize() for update
    void SetDefaultCenterOfInterest(const vector3& defCenterOfInterest);
    /// get default center of interest
    const vector3& GetDefaultCenterOfInterest() const;
    /// set default eyepos - call initialize() for update
    void SetDefaultEyePos(const vector3& defEyePos);
    /// get default eyepos
    const vector3& GetDefaultEyePos() const;
    /// set default camera up vector - call initialize() for update
    void SetDefaultUpVec(const vector3& defUpVec);
    /// get default camera up vector
    const vector3& GetDefaultUpVec() const;
    /// get resulting view matrix
    const matrix44& GetViewMatrix() const;
    /// Set LookButton-state
    void SetLookButton(bool button);
    /// Get LookButton-state
    bool GetLookButton() const;
    /// Set PanButton-state
    void SetPanButton(bool button);
    /// Get PanButton-state
    bool GetPanButton() const;
    /// Set ZoomButton-state
    void SetZoomButton(bool button);
    /// Get ZoomButton-state
    bool GetZoomButton() const;
    /// Set ResetButton-state
    void SetResetButton(bool button);
    /// Get ResetButton-state
    bool GetResetButton() const;
    /// Set slider left
    void SetSliderLeft(float slider);
    /// Get slider left
    float GetSliderLeft() const;
    /// Set slider right
    void SetSliderRight(float slider);
    /// Get slider right
    float GetSliderRight() const;
    /// Set slider up
    void SetSliderUp(float slider);
    /// Get slider up
    float GetSliderUp() const;
    /// Set slider down
    void SetSliderDown(float slider);
    /// Get slider down
    float GetSliderDown() const;
    /// Get Center of Interest
    const vector3& GetCenterOfInterest() const;

protected:

    vector3 defEyePos;
    vector3 defCenterOfInterest;
    vector3 defUpVec;

    polar2 viewerAngles;
    float viewerDistance;
    vector3 centerOfInterest;

    matrix44 viewMatrix;

    bool lookButton;
    bool panButton;
    bool zoomButton;
    bool resetButton;
    float sliderLeft;
    float sliderRight;
    float sliderUp;
    float sliderDown;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetDefaultCenterOfInterest(const vector3& defCenterOfInterest)
{
    this->defCenterOfInterest = defCenterOfInterest;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMayaCamControl::GetDefaultCenterOfInterest() const
{
    return this->defCenterOfInterest;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetDefaultEyePos(const vector3& defEyePos)
{
    this->defEyePos = defEyePos;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMayaCamControl::GetDefaultEyePos() const
{
    return this->defEyePos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetDefaultUpVec(const vector3& defUpVec)
{
    this->defUpVec = defUpVec;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMayaCamControl::GetDefaultUpVec() const
{
    return this->defUpVec;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMayaCamControl::GetLookButton() const
{
    return this->lookButton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMayaCamControl::GetPanButton() const
{
    return this->panButton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMayaCamControl::GetZoomButton() const
{
    return this->zoomButton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMayaCamControl::GetResetButton() const
{
    return this->resetButton;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMayaCamControl::GetSliderLeft() const
{
    return this->sliderLeft;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMayaCamControl::GetSliderRight() const
{
    return this->sliderRight;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMayaCamControl::GetSliderUp() const
{
    return this->sliderUp;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMayaCamControl::GetSliderDown() const
{
    return this->sliderDown;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetLookButton(bool button)
{
    this->lookButton = button;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetPanButton(bool button)
{
    this->panButton = button;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetZoomButton(bool button)
{
    this->zoomButton = button;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetResetButton(bool button)
{
    this->resetButton = button;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetSliderLeft(float slider)
{
    this->sliderLeft = slider;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetSliderRight(float slider)
{
    this->sliderRight = slider;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetSliderUp(float slider)
{
    this->sliderUp = slider;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMayaCamControl::SetSliderDown(float slider)
{
    this->sliderDown = slider;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nMayaCamControl::GetViewMatrix() const
{
    return this->viewMatrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMayaCamControl::GetCenterOfInterest() const
{
    return this->centerOfInterest;
}

//------------------------------------------------------------------------------
#endif
