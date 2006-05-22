//------------------------------------------------------------------------------
//  navigation/waypointlist.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "navigation/waypointlist.h"

namespace Navigation
{
ImplementRtti(Navigation::WayPointList, Foundation::RefCounted);
ImplementFactory(Navigation::WayPointList);

//------------------------------------------------------------------------------
/**
*/
WayPointList::~WayPointList()
{
}

} // namespace Navigation
