#ifndef N_SHADOWSERVER2_H
#define N_SHADOWSERVER2_H
//------------------------------------------------------------------------------
/**
    @class nShadowServer2
    @ingroup Shadow2

    Server object of shadow2 subsystem.

    (C) 2005 Radon Labs GmbH
*/
#include "kernel/nroot.h"
#include "gfx2/nlight.h"
#include "shadow2/nshadowcaster2.h"

class nShadowCaster2;
class nShader2;

//------------------------------------------------------------------------------
class nShadowServer2 : public nRoot
{
public:
    /// constructor
    nShadowServer2();
    /// destructor
    virtual ~nShadowServer2();
    /// return instance pointer
    static nShadowServer2* Instance();
    /// create a static shadow caster
    nShadowCaster2* NewShadowCaster(nShadowCaster2::Type t, const char* rsrcName);
    /// open the shadow server
    bool Open();
    /// close the shadow server
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// begin rendering the shadow scene
    bool BeginScene();
    /// begin a new light
    void BeginLight(const nLight& light);
    /// render shadow caster with current light
    void RenderShadowCaster(nShadowCaster2* caster, const matrix44& modelMatrix);
    /// end current light
    void EndLight();
    /// finish rendering the shadow scene
    void EndScene();
    /// enable/disable zfail rendering
    void SetUseZFail(bool b);
    /// return zfail flag
    bool GetUseZFail() const;
    /// enable/disable shadow rendering
    void SetEnableShadows(bool b);
    /// get shadow enabled state
    bool GetEnableShadows() const;

private:
    static nShadowServer2* Singleton;
    nRef<nShader2> refShader;       // the shadow volume shader
    bool isOpen;
    bool inBeginScene;
    bool inBeginLight;
    bool useZFail;
    bool shadowsEnabled;
    nLight curLight;
    int numShaderPasses;
};

//------------------------------------------------------------------------------
/**
*/
inline
nShadowServer2*
nShadowServer2::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShadowServer2::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowServer2::SetUseZFail(bool b)
{
    this->useZFail = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShadowServer2::GetUseZFail() const
{
    return this->useZFail;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowServer2::SetEnableShadows(bool b)
{
    this->shadowsEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShadowServer2::GetEnableShadows() const
{
    return this->shadowsEnabled;
}

//------------------------------------------------------------------------------
#endif
