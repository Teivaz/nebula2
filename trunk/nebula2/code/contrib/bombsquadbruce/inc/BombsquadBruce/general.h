#ifndef GENERAL_H_SDFGKLJWEIONBVYXASPORWETLKYDFG
#define GENERAL_H_SDFGKLJWEIONBVYXASPORWETLKYDFG

class nScriptServer;
class nKernelServer;
#include "mathlib/vector.h"
#include "kernel/npersistserver.h"
// the following is just for CCSkinData, which should maybe be in its own file, as its own class
class nVariableServer;
class nSkinAnimator;
class nSceneNode;
class nVariable;
class nRenderContext;

struct coord2 
{
	int x;
	int y;
	
	coord2(int newx, int newy) : x(newx), y(newy) {} 
};

static inline coord2 operator+( const coord2 & lhs, const coord2 & rhs )
{
	return coord2( lhs.x + rhs.x, lhs.y + rhs.y );
}

class CCSkinData
{
public:
    CCSkinData();
    ~CCSkinData();
    void Init( nRenderContext& renderContext );
    void SetState( int state );
    void StepTime( float dt );
    void SetSpeedFactor( float );
private:
    nVariable* m_pSkinAnimatorTimeVar;
    nVariable* m_pSkinAnimatorStateVar;
    nDynAutoRef<nSkinAnimator> m_rSkinAnimator;
    nRenderContext* m_pRenderContext;
    float m_SpeedFactor;
};


namespace CCUtil
{
	float Normalize( float angleInRadians );
    /// This runs a script *file* -- useful for stuff you want to be able to change while the game is running
    void RunScript( const char* scriptFunc );
    /// This runs a predefined script *function* -- this is faster than RunScript (I think)
    void RunScriptFunction( const char* scriptFunc );
	bool ApproximatelyEqual( float lhs, float rhs );

	static const vector3 UP_VECTOR(0.0f, 1.0f, 0.0f);
	static const vector3 IDENTITY_VECTOR( 0.0f, 0.0f, 1.0f );

    struct SaveStrCmd  // Helper functor for SaveCmds
    {
        SaveStrCmd( nPersistServer* ps, nRoot* caller ) : m_pPersistServer( ps ), m_pCaller( caller ) {}
        void operator() ( const char* strToSave, uint cmdId );
        nPersistServer* m_pPersistServer;
        nRoot* m_pCaller;
    };
}

#endif