#ifndef N_CMOBILEACTOR_H
#define N_CMOBILEACTOR_H
//------------------------------------------------------------------------------
/**
    @class CCMobileActor

    @brief The base class for everything that moves and is visible.

    At the moment, there is only one such class, namely the player.  To avoid 
    bloat, I didn't create a CCPlayer subclass.  A more complex game, say with
    computer controlled animals walking around, would derive CCPlayer and
    CCAnimal, and move much of the functionality below into the former.

    (C)	2004 Rafael Van Daele-Hunt
*/
#include "BombsquadBruce/ccactor.h"
#include "kernel/ndynautoref.h"
#include "BombsquadBruce/general.h"
class nInputServer;

//------------------------------------------------------------------------------
class CCMobileActor : public CCActor
{
public:
    CCMobileActor();
    ~CCMobileActor();
    virtual bool SaveCmds(nPersistServer* persistServer);

    // script commands
    virtual void SetShapeNode(const char* shapenode_path);
    void SetFwdSpd( float spd ) {if( spd > 0.0f ) m_FwdSpd = spd; }
    void SetTurnSpd( float spd ) {if( spd > 0.0f ) m_TurnSpd = spd; }
    void SetApparentTurnSpd( float spd ) {if( spd > 0.0f ) m_ApparentTurnSpd = spd; }
    void SetTiltSpd( float spd ) {if( spd > 0.0f ) m_TiltSpd = spd; }
    void SetControl( const std::string & control, const std::string & channel ); // if we have more than one player (for instance in a split screen game), each must be able to listen on its own channel to be separately controllable
    vector3 GetCurHeadingVector() const; 
    vector3 GetApparentHeadingVector() const;
    float GetCurSpd() const;
    float GetCurStrafeSpd() const;
    float GetCurTurnSpd() const;
    float GetCurTiltSpd() const;
    void Move( const vector3& v, float rotation, float deltaTime );
    void SetAnimState( int state, float timeFactor );
    void SetAnimSpeed( float timeFactor );
    
    //nonscript commands
    virtual void Trigger( float dt );
    virtual void Attach(nSceneServer& sceneServer, uint& frameId);
    const vector3& GetPreviousPosition() const { return m_PreviousPosition; }
private:
    void UpdateApparentRotation( float dt );
    void UpdateCurrentSpeed();

    vector3 m_PreviousPosition;
    float m_ApparentRotation; // Replaces the actual y rotation for graphical (but not logical) purposes.  This is used to disguise strafing as turning and walking sideways, etc.  It allows emulation of PoP-style movement without having to change the movement algorithms or camera controls
    float m_DesiredApparentRotation; // where we want to appear to face; may deviate from m_ApparentRotation because of limited (apparent) turn speed
    float m_LastDesiredAppRot;
    float m_ApparentTurnSpd; // how fast we complete an apparent rotation (all rotations, regardless of size, take the same time).
    float m_CurAppTurnSpd;
    vector3 m_ApparentHeadingVec;
    float m_CurFwdSpd;
    float m_CurTurnSpd;
    float m_CurTiltSpd;
    float m_CurStrafeSpd;

    // permanent attributes (non-const only because they can be set by the script):
    float m_FwdSpd;
    float m_TurnSpd; // left-right
    float m_TiltSpd; // up-down

    CCSkinData m_SkinData;

    std::string m_SpinLeftChannel;
    std::string m_SpinRightChannel;
    std::string m_SpinUpChannel;
    std::string m_SpinDownChannel;
    std::string m_MoveFwdChannel;
    std::string m_MoveBackChannel;
    std::string m_MoveLeftChannel;
    std::string m_MoveRightChannel;		
};
//------------------------------------------------------------------------------
#endif

