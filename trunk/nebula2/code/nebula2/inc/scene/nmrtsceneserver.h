#ifndef N_MRTSCENESERVER_H
#define N_MRTSCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nMTRSceneServer
    @ingroup SceneServers

    @brief "Multiple Render Target" scene server, spreads rendering across
    various specialized offscreen buffers, which are combined to the final
    result by specific "compositing" pixel shaders.

    (C) 2003 RadonLabs GmbH
*/
#include "scene/nstdsceneserver.h"
#include "kernel/nautoref.h"
#include "variable/nvariable.h"
#include "gfx2/nshaderparams.h"
#include "misc/nwatched.h"
#include "shadow/nshadowserver.h"

class nTexture2;
class nShader2;
class nMesh2;

//------------------------------------------------------------------------------
class nMRTSceneServer : public nStdSceneServer
{
public:
    /// constructor
    nMRTSceneServer();
    /// destructor
    virtual ~nMRTSceneServer();
    /// begin the scene
    virtual bool BeginScene(const matrix44& viewer);
    /// render the scene
    virtual void RenderScene();
    /// enable/disable frame compositing
    void SetCompositingEnabled(bool b);
    /// get compositing enabled state
    bool IsCompositingEnabled() const;
    /// set luminance computation vector (usually 0.299, 0.587, 0.114)
    void SetLuminance(const vector4& l);
    /// get luminance computation vector
    const vector4& GetLuminance() const;
    /// get lum
    /// set saturation value (0.0 .. 1.0)
    void SetSaturation(float s);
    /// get saturation
    float GetSaturation() const;
    /// set color balance
    void SetBalance(const vector4& b);
    /// get color balance
    const vector4& GetBalance() const;

protected:
    /// initialize required resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// check if resources are valid
    bool AreResourcesValid();
    /// perform the final frame compositing
    void DoCompositing();
    
    nAutoRef<nShadowServer> refShadowServer;

    nWatched dbgNumInstanceGroups;
    nWatched dbgNumInstances;

    bool compositingEnabled;
    float saturation;
    vector4 balance;
    vector4 luminance;                  // for luminance computation

    nRef<nTexture2> renderBuffer;       // render buffer if compositing enabled
    nRef<nMesh2> quadMesh;              // mesh describing a full screen quad
    nRef<nShader2> compShader;          // compositing shader
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMRTSceneServer::SetLuminance(const vector4& l)
{
    this->luminance = l;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nMRTSceneServer::GetLuminance() const
{
    return this->luminance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMRTSceneServer::SetCompositingEnabled(bool b)
{
    this->compositingEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMRTSceneServer::IsCompositingEnabled() const
{
    return this->compositingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMRTSceneServer::SetSaturation(float s)
{
    this->saturation = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nMRTSceneServer::GetSaturation() const
{
    return this->saturation;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMRTSceneServer::SetBalance(const vector4& c)
{
    this->balance = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nMRTSceneServer::GetBalance() const
{
    return this->balance;
}

//------------------------------------------------------------------------------
#endif


