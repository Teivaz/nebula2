#ifndef N_SHADOWSERVER_H
#define N_SHADOWSERVER_H
//------------------------------------------------------------------------------
/**
    @class nShadowServer
    @ingroup Shadow

    @brief The central server object in the stencil shadow buffer subsystem.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/ndynamicshadermesh.h"

class nShadowCaster;
//------------------------------------------------------------------------------
class nShadowServer : public nRoot
{
public:
    /// caster type
    enum CasterType
    {
        Static,
        Skin,
    };

    enum DrawType
	{
		zPass = 0,
		zFail,

        NumDrawTypes,
	};

    /// constructor
    nShadowServer();
    /// destructor
    virtual ~nShadowServer();
    /// return instance pointer
    static nShadowServer* Instance();
        
	/// start a new scene - return true is shadow is enabled
    bool BeginScene();
    /// end the current scene and draw the shadow plane
    void EndScene();
	/// start shadow casting for light
	bool BeginLight(nLight::Type type, const vector3& lightPos);
	/// end current light
    void EndLight();   
	/// get light position (world space)
    const vector3& GetLight() const;
	/// get light type
	const nLight::Type GetLightType() const;
	/// request draw buffer from shadowserver
	bool BeginDrawBuffer(DrawType type, vector3*& dstCoords, int& numCoords);
	/// swap draw buffer from shadowserver
	bool SwapDrawBuffer(DrawType type, vector3*& dstCoords, int& numCoords);
    /// end write to draw buffer
    void EndDrawBuffer(DrawType type, vector3* dstCoords, int numCoords);
	/// get maximum number of valid coords that can be stored in the draw buffer
	const int GetMaxNumCoords(DrawType type) const;
    
    /// create a new shadow caster resource of the specified type.
    nShadowCaster* NewShadowCaster(CasterType casterType, const char* rsrcName);
    
    /// set the shadow color
    void SetShadowColor(const vector4& shadowColor);
    /// get the shadow color
    const vector4& GetShadowColor() const;
    /// enable/disable shadow
    void SetShowShadows(bool show);
    /// get shadow enabled status
    bool GetShowShadows() const;
    /// enable/disable debug shadows
    void SetDebugShadows(bool show);
    /// get shadow debug status
    bool GetDebugShadows() const;

    /// enable/disable zFail shadow rendering
    void SetUseZFail(bool use);
    /// get shadow debug status
    bool GetUseZFail() const;

    /// render current setup mesh as debug
    void DrawDebugShadows(const vector4& matDiffuse);

    /// redner current setup mesh in wireframe for edge debug
    void DrawDebugEdges(const vector4& matDiffuse);

protected:
    /// are the resources valid?
    bool AreResourcesValid();
    /// load resources for this object
    virtual bool LoadResources();
    /// unload resources for this object
    virtual void UnloadResources();

private:
    static nShadowServer* Singleton;
    
    nRef<nShader2> refPlaneShader;
    nRef<nMesh2> refPlaneMesh;
    
    // dynamic mesh buffers
    nDynamicShaderMesh dynMesh[NumDrawTypes];
    vector3* coordPtr[NumDrawTypes];
    int maxNumCoord[NumDrawTypes];
    int numCoord[NumDrawTypes];
    nRef<nShader2> refStencilShader[NumDrawTypes];
    
    bool inBeginScene;
    bool inBeginLight;

    bool useZFail;

    bool showShadow;
    vector3 lightPosition;
    nLight::Type lightType;
    vector4 shadowColor;

    bool debugShowGeometry;
    nRef<nShader2> refDebugShader;
};

//------------------------------------------------------------------------------
/**
*/
inline
nShadowServer*
nShadowServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowServer::SetShowShadows(bool b)
{
    this->showShadow = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShadowServer::GetShowShadows() const
{
    return this->showShadow;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowServer::SetDebugShadows(bool b)
{
    this->debugShowGeometry = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShadowServer::GetDebugShadows() const
{
    return this->debugShowGeometry;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowServer::SetUseZFail(bool b)
{
    this->useZFail = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShadowServer::GetUseZFail() const
{
    return this->useZFail;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShadowServer::SetShadowColor(const vector4& c)
{
    this->shadowColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nShadowServer::GetShadowColor() const
{
    return this->shadowColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
nShadowServer::GetMaxNumCoords(DrawType type) const
{
    n_assert(this->inBeginLight);
    n_assert(type >= 0 && type < NumDrawTypes);
    n_assert(this->dynMesh[type].IsValid());
    return this->maxNumCoord[type];
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nShadowServer::GetLight() const
{
    n_assert(this->inBeginLight);
    return this->lightPosition;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nLight::Type
nShadowServer::GetLightType() const
{
    n_assert(this->inBeginLight);
    return this->lightType;
}
//------------------------------------------------------------------------------
#endif

