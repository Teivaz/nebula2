//------------------------------------------------------------------------------
//  ntoolkitserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "toolkit/ntoolkitserver.h"
#include "scene/nmultilayerednode.h"

nNebulaScriptClass(nToolkitServer, "nroot");
nToolkitServer* nToolkitServer::Singleton = 0;
nToolkitServer* nToolkitServer::self = 0;


//------------------------------------------------------------------------------
/**
*/
nToolkitServer::nToolkitServer():
    isOpen(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
    self = this;
}

//------------------------------------------------------------------------------
/**
*/
nToolkitServer::~nToolkitServer()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the scene server. This will parse the render path, initialize
    the shaders assign from the render path, and finally invoke
    nGfxServer2::OpenDisplay().
*/
bool
nToolkitServer::Open()
{
    n_assert(!this->isOpen);   
    
    this->isOpen = true;
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    Close the scene server. This will also nGfxServer2::CloseDisplay().
*/
void
nToolkitServer::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}


//------------------------------------------------------------------------------
/**
    ChangeShaderParameter

    This method is called by the "changeshaderparameter"-tcl-command and is used
    by the toolkit remote control to change attributes within the nviewer

    @param mayaShaderName:
        this is the name of the mayashader. it it used to identify the materialnodes where
        the parameter changes should be applied

    @param shaderHandling:
        Used to identify the handling function
        0 - Standard parameters (this->ChangeTypeCommon() is called for handling)
        1 - Particle2 parameters (this->ChangeTypeParticle2() is called for handling)
        ...

    @param parameterID:
        a unique identifier for the parameter that should be changed
        does not need to be globally unique, but unique for one parameterhandler

    @value:
        a string containing the value of the parameter
*/
nString
nToolkitServer::ChangeShaderParameter(nString mayaShaderName,nString shaderHandling,nString parameterID,nString value)
{
    // iterate through nodelist and apply changes
    bool result = true;
    nNodeList *nodeList = nNodeList::Instance();
    uint nodeCount = nodeList->GetCount();
    uint i;
    for( i=0; i<nodeCount; i++)
    {
        nTransformNode *node = nodeList->GetNodeAt(i);
        result &= ChangeShaderParameterOnNode(node,mayaShaderName,shaderHandling,parameterID,value);
    };
    if(!result) return "false";
    return "true";
};

//------------------------------------------------------------------------------
/**
    recursive iteration through node hirarchy
*/
bool
nToolkitServer::ChangeShaderParameterOnNode(nRoot* node,nString mayaShaderName,nString shaderHandling,nString parameterID,nString value)
{
    bool result = true;
    if(node->IsA(nKernelServer::Instance()->FindClass("nmaterialnode")))
    {
        nMaterialNode *matNode = (nMaterialNode*) node;
        if(matNode->GetMayaShaderName() == mayaShaderName)
        {
            // ok, we found a material node which has the given mayashadername assigned

            // common shader attributes
            if(shaderHandling == "common")      this->ChangeTypeCommon(matNode,parameterID,value);
            // particle2 shader attributes
            if(shaderHandling == "particle2")   this->ChangeTypeParticle2(matNode,parameterID,value);
        };
    };
    // apply changes on subnodes
    nRoot* curChild;
    for (curChild = (nRoot*) node->GetHead();
         curChild;
         curChild = (nRoot*) curChild->GetSucc())
    {
        result &= this->ChangeShaderParameterOnNode(curChild,mayaShaderName,shaderHandling,parameterID,value);
    }
    return result;
};

//------------------------------------------------------------------------------
/**
*/
bool 
nToolkitServer::ChangeTypeCommon(nMaterialNode* node,nString parameterID,nString value)
{
    bool result = true;
    
    nString fileName;

    n_printf("Received remote command : %s %s\n",parameterID.Get(),value.Get());

    // Material Diffuse
    if(parameterID == "MatDiffuse")
    {
        if(node->HasParam(nShaderState::MatDiffuse))
                node->SetVector(nShaderState::MatDiffuse,value.AsVector4());  
    };
    // Emissive Color
    if(parameterID == "MatEmissive")
    {
        if(node->HasParam(nShaderState::MatEmissive))
                node->SetVector(nShaderState::MatEmissive,value.AsVector4());  
    };
    // Specular Color
    if(parameterID == "MatSpecular")
    {
        if(node->HasParam(nShaderState::MatSpecular))
                node->SetVector(nShaderState::MatSpecular,value.AsVector4());  
    };
    // Emissive Intensity
    if(parameterID == "MatEmissiveIntensity")
    {
        if(node->HasParam(nShaderState::MatEmissiveIntensity))
                node->SetFloat(nShaderState::MatEmissiveIntensity,value.AsFloat());  
    };
    // Specular Intensity
    if(parameterID == "MatSpecularPower")
    {
        if(node->HasParam(nShaderState::MatSpecularPower))
                node->SetFloat(nShaderState::MatSpecularPower,value.AsFloat());  
    };
    // Bump Scale
    if(parameterID == "BumpScale")
    {
        if(node->HasParam(nShaderState::BumpScale))
                node->SetFloat(nShaderState::BumpScale,value.AsFloat());  
    };
    // Alpha Src Blend
    if(parameterID == "AlphaSrcBlend")
    {
        if(node->HasParam(nShaderState::AlphaSrcBlend))
                node->SetInt(nShaderState::AlphaSrcBlend,value.AsInt());  
    };
    // Alpha Dst Blend
    if(parameterID == "AlphaDstBlend")
    {
        if(node->HasParam(nShaderState::AlphaDstBlend))
                node->SetInt(nShaderState::AlphaDstBlend,value.AsInt());  
    };
    // DiffMap 0
    if(parameterID == "DiffMap0")
    {
        fileName = nFileServer2::Instance()->ManglePath(value.Get());
        if(nFileServer2::Instance()->FileExists(fileName))
        {
            if(node->HasParam(nShaderState::DiffMap0))
                    node->SetTexture(nShaderState::DiffMap0,fileName.Get());  
        };
    };
    // BumpMap 0
    if(parameterID == "BumpMap0")
    {
        fileName = nFileServer2::Instance()->ManglePath(value.Get());
        if(nFileServer2::Instance()->FileExists(fileName))
        {
            if(node->HasParam(nShaderState::BumpMap0))
                    node->SetTexture(nShaderState::BumpMap0,fileName.Get());  
        };
    };

    // UV Stretch of MultiLayeredShader , FIXME : currently only working with dx9
    if(parameterID.FindStringIndex("rlUVStretch",0) != -1)
    {
        if(node->IsA(nKernelServer::Instance()->FindClass("nmultilayerednode")))
        {
            int maxTexturesPerShader = 5;
            nMultiLayeredNode *mlNode = (nMultiLayeredNode*) node;

            // extract map index from parameterID
            int mapIndex = (parameterID.ExtractRange(11,parameterID.Length()-11)).AsInt();
            float uv = value.AsFloat();

            n_printf("setting to %4.4f\n",uv);

            // now we need to recalculate the correct index
            int dx9pass = mapIndex / maxTexturesPerShader;
            int dx9offset = mapIndex % maxTexturesPerShader;
            if(mlNode->GetPartIndex() == dx9pass)
            {
                mlNode->SetUVStretch(dx9offset,uv);
            };
        };
    };

    return result;
};

//------------------------------------------------------------------------------
/**
*/
nEnvelopeCurve 
nToolkitServer::AsEnvelopeCurve(nString value)
{
    nArray<nString> tokens;
    value.Tokenize(" ",tokens);
    n_assert(tokens.Size() == 9);
    return nEnvelopeCurve(tokens[0].AsFloat(),tokens[1].AsFloat(),tokens[2].AsFloat(),tokens[3].AsFloat(),
                            tokens[4].AsFloat(),tokens[5].AsFloat(),tokens[6].AsFloat(),tokens[7].AsFloat(),tokens[8].AsInt());
};
//------------------------------------------------------------------------------
/**
*/
nVector3EnvelopeCurve
nToolkitServer::AsColorCurve(nString value)
{
    nArray<nString> tokens;
    value.Tokenize(" ",tokens);
    n_assert(tokens.Size() == 14);
    return nVector3EnvelopeCurve(vector3(tokens[0].AsFloat(),tokens[1].AsFloat(),tokens[2].AsFloat()),
                                 vector3(tokens[3].AsFloat(),tokens[4].AsFloat(),tokens[5].AsFloat()),
                                 vector3(tokens[6].AsFloat(),tokens[7].AsFloat(),tokens[8].AsFloat()),
                                 vector3(tokens[9].AsFloat(),tokens[10].AsFloat(),tokens[11].AsFloat()),
                                 tokens[12].AsFloat(),tokens[13].AsFloat());
};


//------------------------------------------------------------------------------
/**
*/
bool 
nToolkitServer::ChangeTypeParticle2(nMaterialNode* node,nString parameterID,nString value)
{
    bool result = true;

    if(node->IsA(nKernelServer::Instance()->FindClass("nparticleshapenode2")))
    {
        nParticleShapeNode2 *partNode = (nParticleShapeNode2*) node;

        // Gravity
        if(parameterID == "ParticleGravity")
        {
            partNode->SetGravity(value.AsFloat());
        };
        // Activity Distance
        if(parameterID == "ActivityDistance")
        {
            partNode->SetActivityDistance(value.AsFloat());
        };
        // Particle Stretch
        if(parameterID == "ParticleStretch")
        {
            partNode->SetParticleStretch(value.AsFloat());
        };
        // Stretch Detail
        if(parameterID == "StretchDetail")
        {
            partNode->SetStretchDetail(value.AsInt());
        };
        // ParticleStartVelocityRandomize
        if(parameterID == "ParticleStartVelocityRandomize")
        {
            partNode->SetParticleVelocityRandomize(value.AsFloat());
        };
        // ParticleInitialRotationMin
        if(parameterID == "ParticleInitialRotationMin")
        {
            partNode->SetStartRotationMin(value.AsFloat());
        };
        // ParticleInitialRotationMax
        if(parameterID == "ParticleInitialRotationMax")
        {
            partNode->SetStartRotationMax(value.AsFloat());
        };
        // ParticleRotationRandomize
        if(parameterID == "ParticleRotationRandomize")
        {
            partNode->SetParticleRotationRandomize(value.AsFloat());
        };
        // ParticleSizeRandomize
        if(parameterID == "ParticleSizeRandomize")
        {
            partNode->SetParticleSizeRandomize(value.AsFloat());
        };
        // EmissionFrequency
        if(parameterID == "rlEmissionFrequency")
        {
            partNode->SetCurve(nParticle2Emitter::EmissionFrequency,this->AsEnvelopeCurve(value));
        };
        // ParticleLifeTime
        if(parameterID == "rlParticleLifeTime")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleLifeTime,this->AsEnvelopeCurve(value));
        };
        // ParticleStartVelocity
        if(parameterID == "rlParticleStartVelocity")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleStartVelocity,this->AsEnvelopeCurve(value));
        };
        // ParticleSpreadMax
        if(parameterID == "rlParticleSpreadMax")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleSpreadMax,this->AsEnvelopeCurve(value));
        };
        // ParticleSpreadMin
        if(parameterID == "rlParticleSpreadMin")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleSpreadMin,this->AsEnvelopeCurve(value));
        };
        // ParticleRotationVelocity
        if(parameterID == "rlParticleRotationVelocity")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleRotationVelocity,this->AsEnvelopeCurve(value));
        };
        // ParticleScale
        if(parameterID == "rlParticleSize")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleScale,this->AsEnvelopeCurve(value));
        };
        // ParticleAlpha
        if(parameterID == "rlParticleAlpha")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleAlpha,this->AsEnvelopeCurve(value));
        };
        // ParticleAirResistance
        if(parameterID == "rlParticleAirResistance")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleAirResistance,this->AsEnvelopeCurve(value));
        };
        // ParticleVelocityFactor
        if(parameterID == "rlParticleVelocityFactor")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleVelocityFactor,this->AsEnvelopeCurve(value));
        };
        // ParticleMass
        if(parameterID == "rlParticleMass")
        {
            partNode->SetCurve(nParticle2Emitter::ParticleMass,this->AsEnvelopeCurve(value));
        };
        // TimeManipulator
        if(parameterID == "rlTimeManipulator")
        {
            partNode->SetCurve(nParticle2Emitter::TimeManipulator,this->AsEnvelopeCurve(value));
        };
        // TileTexture
        if(parameterID == "TileTexture")
        {
            partNode->SetTileTexture(value.AsBool());
        };
        // StretchToStart
        if(parameterID == "StretchToStart")
        {
            partNode->SetStretchToStart(value.AsBool());
        };
        // ViewAngleFade
        if(parameterID == "ViewAngleFade")
        {
            partNode->SetViewAngleFade(value.AsBool());
        };
        // RenderOldestFirst
        if(parameterID == "RenderOldestFirst")
        {
            partNode->SetRenderOldestFirst(value.AsBool());
        };
        // RandomRotDir
        if(parameterID == "RandomRotDir")
        {
            partNode->SetRandomRotDir(value.AsBool());
        };
        // ColorCurve
        if(parameterID == "rlParticleColor")
        {
            partNode->SetRGBCurve(this->AsColorCurve(value));
        };
        // Loop
        if(parameterID == "Loop")
        {
            partNode->SetLoop(value.AsBool());
        };
        // Emission Duration
        if(parameterID == "EmissionDuration")
        {
            partNode->SetEmissionDuration(value.AsFloat());
        };
        // Precalc Time
        if(parameterID == "PrecalcTime")
        {
            partNode->SetPrecalcTime(value.AsFloat());
        };
        // Start Delay
        if(parameterID == "ParticleStartDelay")
        {
            partNode->SetStartDelay(value.AsFloat());
        };

    } else {
        result = false;
    };
    return result;
};
