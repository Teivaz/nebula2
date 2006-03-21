#ifndef N_TOOLKITSERVER_H
#define N_TOOLKITSERVER_H
//------------------------------------------------------------------------------
/**
    @class nToolkitServer
    @ingroup Scene

    For Remote Access

    -20-Mar-06  kims  Renamed nMayaToolkitServer to nToolkitServer.

    (C) 2005 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "mathlib/matrix.h"
#include "mathlib/envelopecurve.h"
#include "mathlib/vector3envelopecurve.h"
#include "kernel/nautoref.h"
#include "gfx2/nshaderparams.h"
#include "util/nbucket.h"
#include "scene/nmaterialnode.h"
#include "tools/nnodelist.h"
#include "scene/nparticleshapenode2.h"

//------------------------------------------------------------------------------
class nToolkitServer : public nRoot
{
public:
    /// constructor
    nToolkitServer();
    /// destructor
    virtual ~nToolkitServer();
    /// return pointer to instance
    static nToolkitServer* Instance();
    /// open the scene server, call after nGfxServer2::OpenDisplay()
    virtual bool Open();
    /// close the scene server;
    virtual void Close();
    /// return true if scene server open
    bool IsOpen() const;
    /// change Maya Shader Parameter (called by remoteControl)
    nString ChangeShaderParameter(nString mayaShaderName,nString shaderHandling,nString parameterID,nString value);

private:
    /// @name Toolkit Remote Shader-Parameter-Change-Functions 
    /// @{
    /// recursivly called to apply changes
    bool ChangeShaderParameterOnNode(nRoot* node,nString mayaShaderName,nString shaderHandling,nString parameterID,nString value);
    /// attribute handler 0 (Common Attributes)
    bool ChangeTypeCommon(nMaterialNode* node,nString parameterID,nString value);
    /// attribute handler 1 (Particle2)
    bool ChangeTypeParticle2(nMaterialNode* node,nString parameterID,nString value);
    /// helper function, converts a string to an envelope curve
    nEnvelopeCurve AsEnvelopeCurve(nString value);
    /// helper function, converts a string to a color envelope curve
    nVector3EnvelopeCurve AsColorCurve(nString value);
    /// @}

    static nToolkitServer* Singleton;
    static nToolkitServer* self;
    bool isOpen;

};

//------------------------------------------------------------------------------
/**
*/
inline
nToolkitServer*
nToolkitServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}
//------------------------------------------------------------------------------
/**
*/
inline
bool
nToolkitServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
#endif
