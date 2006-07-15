//------------------------------------------------------------------------------
//  nterraingrassnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "ncterrain2/nterraingrassnode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "scene/nrendercontext.h"
#include "variable/nvariableserver.h"

nNebulaScriptClass(nTerrainGrassNode, "nmaterialnode");

//------------------------------------------------------------------------------
/**
*/
nTerrainGrassNode::nTerrainGrassNode()
{
    nVariableServer* varServer = nVariableServer::Instance();
    this->attrsVarHandle = varServer->GetVariableHandleByName("TerrainGrassAttrs");
}

//------------------------------------------------------------------------------
/**
*/
nTerrainGrassNode::~nTerrainGrassNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This node can render geometry.
*/
bool
nTerrainGrassNode::HasGeometry() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Do the actual rendering. 
*/
bool
nTerrainGrassNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    n_assert(sceneServer);
    n_assert(renderContext);
	nGfxServer2* gfxServer = nGfxServer2::Instance();

    // works only on DX9 cards
    if (gfxServer->GetFeatureSet() < nGfxServer2::DX9)
    {
        return false;
    }
    
    // get rendering attributes from application
    nVariable* attrsVar = renderContext->GetVariable(this->attrsVarHandle);
    if (!attrsVar)
    {
        n_printf("nTerrainGrassNode: warning, no render context attributes for '%s'\n", this->GetName());
        return false;
    }
    Attrs* attrs = (Attrs*) attrsVar->GetObj();
    n_assert(attrs);

    // get current shader
    nShader2* curShader = gfxServer->GetShader();
    n_assert(curShader);

    // set texture attributes
    if (attrs->texture)
    {
        if (curShader->IsParameterUsed(nShaderState::AmbientMap0))
        {
            curShader->SetTexture(nShaderState::AmbientMap0, attrs->texture);
        }
    }
    if (curShader->IsParameterUsed(nShaderState::TexGenS))
    {
        curShader->SetFloat4(nShaderState::TexGenS, attrs->texGenS);
    }
    if (curShader->IsParameterUsed(nShaderState::TexGenT))
    {
        curShader->SetFloat4(nShaderState::TexGenT, attrs->texGenT);
    }

    // render the mesh
    gfxServer->SetMesh(attrs->mesh);
    gfxServer->SetVertexRange(0, attrs->mesh->GetNumVertices());
    gfxServer->DrawNS(nGfxServer2::PointList);
    gfxServer->SetMesh(0);
    return true;
}
