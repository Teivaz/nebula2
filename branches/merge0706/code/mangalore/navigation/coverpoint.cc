//------------------------------------------------------------------------------
//  navigation/coverpoint.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "navigation/coverpoint.h"

namespace Navigation
{

//------------------------------------------------------------------------------
/**
*/
CoverPoint::CoverPoint() :
    position(0.0f, 0.0f, 0.0f),
    heading(0.0f),
    type(Crouching),
    claimed(false)
{
}

//------------------------------------------------------------------------------
/**
*/
CoverPoint::~CoverPoint()
{
}

} // namespace Navigation
