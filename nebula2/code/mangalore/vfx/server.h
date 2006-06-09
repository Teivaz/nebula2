#ifndef VFX_SERVER_H
#define VFX_SERVER_H
//------------------------------------------------------------------------------
/**
    @class VFX::Server

    The VFX (VisualFX) subsystem manages standalone visual effects which
    are created and can remove themselves after they have played. The system
    works similar to the audio subsystem. VisualFX use simple graphics entities
    to render themselves.

    (C) 2003 RadonLabs GmbH
*/
#include "foundation/refcounted.h"
#include "vfx/bank.h"
#include "vfx/effect.h"
#include "vfx/effecthandler.h"
#include "vfx/shakeeffect.h"
#include "misc/nwatched.h"

//------------------------------------------------------------------------------
namespace VFX
{
class Server : public Foundation::RefCounted
{
    DeclareRtti;
    DeclareFactory(Server);

public:
    /// constructor
    Server();
    /// destructor
    virtual ~Server();
    /// get instance pointer
    static Server* Instance();
    /// open the fx server
    virtual bool Open();
    /// close the fx server
    virtual void Close();
    /// open an effect bank and make it current
    bool OpenEffectBank(const nString& name);
    /// close the current effect bank
    void CloseEffectBank();
    /// get current FX bank
    Bank* GetEffectBank() const;
    /// set the current time
    void SetTime(nTime time);
    /// get the current time
    nTime GetTime() const;
    /// get pointer to effect handler
    EffectHandler* GetEffectHandler();
    /// play a visual effect
    void PlayEffect(const nString& effectName, const matrix44& transform);
    /// create a particle effect
    Effect* CreateEffect(const nString& effectName, const matrix44& transform);
    /// play a shake effect
    void PlayShakeEffect(const vector3& pos, float range, float duration, float intensity);
    /// call before rendering happens
    void BeginScene();
    /// call after rendering has happened
    void EndScene();
    /// get number of currently active effects
    int GetNumActiveEffects() const;
    /// get active effect at index
    Effect* GetActiveEffectAt(int index) const;
    /// compute shake intensity at a given position
    float ComputeShakeIntensityAtPosition(const vector3& pos);

private:
    static Server* Singleton;

    bool isOpen;
    nTime curTime;
    Ptr<Bank> effectBank;
    Ptr<EffectHandler> effectHandler;
    nArray<Ptr<Effect> > activeEffects;
    nArray<Ptr<ShakeEffect> > activeShakeEffects;
    nWatched statsNumActiveEffects;
    nWatched statsNumActiveShakeEffects;
};

RegisterFactory(Server);

//------------------------------------------------------------------------------
/**
*/
inline
Server*
Server::Instance()
{
    return Singleton;
}

//------------------------------------------------------------------------------
/**
    Sets the current time. Should be called once per frame.
*/
inline
void
Server::SetTime(nTime time)
{
    this->curTime = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
Server::GetTime() const
{
    return this->curTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
EffectHandler*
Server::GetEffectHandler()
{
    return this->effectHandler;
}

//------------------------------------------------------------------------------
/**
    Returns the number of currently active effects.
*/
inline
int
Server::GetNumActiveEffects() const
{
    return this->activeEffects.Size();
}

//------------------------------------------------------------------------------
/**
    Returns pointer to active effect at index. Note that the number of
    active effects may have change after Trigger() has been called.
*/
inline
Effect*
Server::GetActiveEffectAt(int index) const
{
    return this->activeEffects[index];
}

}; // namespace VFX

//------------------------------------------------------------------------------
#endif
