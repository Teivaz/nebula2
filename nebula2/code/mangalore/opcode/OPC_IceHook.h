
// Should be included by Opcode.h if needed

	#define ICE_DONT_CHECK_COMPILER_OPTIONS

	// From Windows...
	typedef int                 BOOL;
	#ifndef FALSE
	#define FALSE               0
	#endif

	#ifndef TRUE
	#define TRUE                1
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>
	#include <string.h>
	#include <float.h>
	#include <math.h>

	#ifndef ASSERT
		#define	ASSERT(exp)	{}
	#endif
	#define ICE_COMPILE_TIME_ASSERT(exp)	extern char ICE_Dummy[ (exp) ? 1 : -1 ]

	#define	Log				{}
	#define	SetIceError(a,b)	false
	#define	EC_OUTOFMEMORY	"Out of memory"

	#include "ice/IcePreprocessor.h"

	#undef ICECORE_API
	#define ICECORE_API	OPCODE_API

	#include "ice/IceTypes.h"
	#include "ice/IceFPU.h"
	#include "ice/IceMemoryMacros.h"

	namespace IceCore
	{
		#include "ice/IceUtils.h"
		#include "ice/IceContainer.h"
		#include "ice/IcePairs.h"
		#include "ice/IceRevisitedRadix.h"
		#include "ice/IceRandom.h"
	}
	using namespace IceCore;

	#define ICEMATHS_API	OPCODE_API
	namespace IceMaths
	{
		#include "ice/IceAxes.h"
		#include "ice/IcePoint.h"
		#include "ice/IceHPoint.h"
		#include "ice/IceMatrix3x3.h"
		#include "ice/IceMatrix4x4.h"
		#include "ice/IcePlane.h"
		#include "ice/IceRay.h"
		#include "ice/IceIndexedTriangle.h"
		#include "ice/IceTriangle.h"
		#include "ice/IceTriList.h"
		#include "ice/IceAABB.h"
		#include "ice/IceOBB.h"
		#include "ice/IceBoundingSphere.h"
		#include "ice/IceSegment.h"
		#include "ice/IceLSS.h"
	}
	using namespace IceMaths;
