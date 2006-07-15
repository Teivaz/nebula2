#ifndef N_OPENDE_H
#define N_OPENDE_H
//----------------------------------------------------------------------------
/*
    nOpenDE - main header
    
    (c) 2004    Vadim Macagon
*/
//----------------------------------------------------------------------------

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#include "opende/nopendemarshal.h"
#include "opende/nopendelayer.h"

// include all the core classes for convenience

#include "opende/nopendeamotorjoint.h"
#include "opende/nopendeballjoint.h"
#include "opende/nopendefixedjoint.h"
#include "opende/nopendehinge2joint.h"
#include "opende/nopendehingejoint.h"
#include "opende/nopendesliderjoint.h"
#include "opende/nopendeuniversaljoint.h"

#include "opende/nopendebody.h"

#include "opende/nopendesimplespace.h"
#include "opende/nopendehashspace.h"
#include "opende/nopendequadtreespace.h"

#include "opende/nopendeboxgeom.h"
#include "opende/nopendecapsulegeom.h"
#include "opende/nopendeplanegeom.h"
#include "opende/nopenderaygeom.h"
#include "opende/nopendespheregeom.h"
#include "opende/nopendetrimeshgeom.h"
#include "opende/nopendetransformgeom.h"

//----------------------------------------------------------------------------
#endif // N_OPENDE_H
