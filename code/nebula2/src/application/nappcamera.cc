//------------------------------------------------------------------------------
//  ncamera.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "application/nappcamera.h"

//------------------------------------------------------------------------------
/**
*/
nAppCamera::nAppCamera()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAppCamera::~nAppCamera()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppCamera::Reset()
{
    this->transform.settranslation(vector3(0.0f, 0.0f, 0.0f));
    this->transform.seteulerrotation(vector3(0.0f, 0.0f, 0.0f));
    this->transform.setscale(vector3(1.0f, 1.0f, 1.0f));
    this->transform.setmatrix(matrix44::identity);
}

//------------------------------------------------------------------------------
/**
*/
void
nAppCamera::Trigger()
{
    // empty
}

