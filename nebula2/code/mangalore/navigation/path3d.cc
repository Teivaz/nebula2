//------------------------------------------------------------------------------
//  navigation/path3d.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "navigation/path3d.h"

namespace Navigation
{
ImplementRtti(Navigation::Path3D, Foundation::RefCounted);
ImplementFactory(Navigation::Path3D);

//------------------------------------------------------------------------------
/**
*/
Path3D::~Path3D()
{
}

} // namespace Navigation