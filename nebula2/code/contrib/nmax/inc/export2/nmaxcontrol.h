//-----------------------------------------------------------------------------
//  nmaxcontrol.h
//
//  (C)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXCONTROL_H
#define N_MAXCONTROL_H
//-----------------------------------------------------------------------------
#include "export2/nmaxnode.h"
#include "util/narray.h"

//-----------------------------------------------------------------------------
/**
    sampled key type of a control.
*/
enum nMaxControlType 
{
    nMaxPos,        /// Position Controller
    nMaxRot,        /// Rotation Controller
    nMaxScale,      /// Scale Controller
    nMaxFloat,      /// Float Controller
    nMaxPoint3,     /// Point3 Controller
    nMaxTM,         /// Used for sampling the node transformation matrix.  
    nMaxEulerX,     /// Euler X controller
    nMaxEulerY,     /// Euler Y controller
    nMaxEulerZ,     /// Euler Z controller
    nMaxPoint4,     /// Point4 based controllers - This is available from 3ds max 6.
};

//-----------------------------------------------------------------------------
/**
    @class nMaxSampleKey
    @ingroup NebulaMaxExport2Contrib

    @brief A class for sampled key of a control.

    @note
        pos, rot, scale members are decomposed for nMaxTM for easy to use.
*/
struct nMaxSampleKey 
{
    Matrix3    tm;    // use it for nMaxTM
    Point3     pos;   // use it for nMaxPoint3 
    Quat       rot;   // use if for nMaxRot
    Point3     scale; // use it for nMaxScale
    Point4     pt4;      // use if for nMaxPoint4
    float      fval;  // use it for nMaxFloat

    float      time;  // key time.
};

//-----------------------------------------------------------------------------
/**
    @class nMaxControl
    @ingroup NebulaMaxExport2Contrib

    @brief A class for getting keys from controllers.

*/
class nMaxControl
{
public:
	enum Type 
	{
		TCBFloat,
		TCBPosition,
		TCBRotation,
		TCBScale,
        TCBPoint3,
		HybridFloat,
		HybridPosition,
		HybridRotation,
		HybridScale,
        HybridPoint3,
		LinearFloat,
		LinearPosition,
		LinearRotation,
		LinearScale,
        EulerRotation,
        Unknown,
	};

    nMaxControl();
    virtual ~nMaxControl();

    static void GetSampledKey(INode* node, nArray<nMaxSampleKey> & sampleKeyArray, 
                           int sampleRate, nMaxControlType type);

    static void GetSampledKey(Control* control, nArray<nMaxSampleKey> & sampleKeyArray, 
        int sampleRate, nMaxControlType type);

	static Type GetType(Control *control);

};
//-----------------------------------------------------------------------------
#endif
