//------------------------------------------------------------------------------
//  ccsceneserver_main.cc
//  (C) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/ccsceneserver.h"
#include "scene/nscenenode.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nautoref.h"
#include "variable/nvariableserver.h"
#include "scene/nrendercontext.h"
#include "kernel/ntimeserver.h"
#include "scene/nshapenode.h"
#include "BombsquadBruce/general.h"

nNebulaScriptClass(CCSceneServer, "nstdsceneserver");

// global data for qsort() compare function
CCSceneServer* CCSceneServer::ccself = 0;
nVariable::Handle CCSceneServer::m_AlphaFlagHandle = nVariable::InvalidHandle;
//------------------------------------------------------------------------------
/**
*/
CCSceneServer::CCSceneServer( ) 
{
    n_assert2( !ccself, "There can be only one CCSceneServer!" );
    ccself = this;
    m_AlphaFlagHandle = nVariableServer::Instance()->GetVariableHandleByName( "alphaVar_sfoiawersf" );
    m_LODDistHandle = nVariableServer::Instance()->GetVariableHandleByName( "LODDistVar_ngfgdfsre" );
    n_assert( 2 == m_NumLODShapes );
    m_LODShapeHandles[0] = nVariableServer::Instance()->GetVariableHandleByName( "LowPolyShape" );
    m_LODShapeHandles[1] = nVariableServer::Instance()->GetVariableHandleByName( "BillboardShape" );
}

//------------------------------------------------------------------------------
nVariable::Handle CCSceneServer::GetLODVarHandle(int i) const 
{ 
    return ( 0 <= i && i < m_NumLODShapes )
        ? m_LODShapeHandles[i]
        : nVariable::InvalidHandle;
}

//------------------------------------------------------------------------------
/**
    This sets standard parameters, like the wind.
*/
void
CCSceneServer::UpdateShader(nShader2* shd, nRenderContext* renderContext)
{
    nStdSceneServer::UpdateShader( shd, renderContext );
    if (shd->IsParameterUsed(nShaderState::Wind))
    {
        nVariable::Handle windHandle = nVariableServer::Instance()->FindVariableHandleByFourCC( FOURCC( 'wind' ) );
        if( nVariable::InvalidHandle !=  windHandle )
        {
            nVariable* wind = nVariableServer::Instance()->GetGlobalVariableContext().GetVariable( windHandle );
            if( wind )
            {
                shd->SetFloat4(nShaderState::Wind,  wind->GetFloat4() ); // obviously, this should be generated and put in a variable somewhere else, i just haven't got to that yet
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    The actual sort call is separated out to make overriding Compare possible.
*/
void
CCSceneServer::DoSort( ushort* indexPtr, int numIndices )
{
    qsort(indexPtr, numIndices, sizeof(ushort), (int(__cdecl *)(const void *, const void *)) CCCompare);
}
//------------------------------------------------------------------------------
/**
    The scene node sorting compare function.  Nodes that use alpha blending
    come last and render from back to front (otherwise their writes to the
    Z-Buffer might prevent something behind them (that should be visible
    through the transparency) from being rendered).  Other nodes
    are rendered front to back for efficiency.

    FIXME: compare shaders
*/
int
__cdecl
CCSceneServer::CCCompare(const ushort* i1, const ushort* i2)
{
    CCSceneServer* sceneServer = CCSceneServer::ccself;
    const nSceneServer::Group& g1 = sceneServer->groupArray[*i1];
    const nSceneServer::Group& g2 = sceneServer->groupArray[*i2];
    int cmp;

    // alpha?
    nVariable* alphaVar1 = g1.renderContext->GetVariable(m_AlphaFlagHandle);
    nVariable* alphaVar2 = g2.renderContext->GetVariable(m_AlphaFlagHandle);
    if( alphaVar1 && !alphaVar2 )
    { // the first uses alpha, the second doesn't -- alpha users come last
        n_assert( alphaVar1->GetType() == nVariable::Int );
        return 1;
    }
    else if( !alphaVar1 && alphaVar2 )
    {
        return -1;
    }

    // render priority
    cmp = g1.sceneNode->GetRenderPri() - g2.sceneNode->GetRenderPri();
    if (cmp != 0)
    {
        return cmp;
    }

    // distance to viewer 
    static vector3 dist1;
    static vector3 dist2;
    dist1.set(viewerPos.x - g1.modelTransform.M41,
              viewerPos.y - g1.modelTransform.M42,
              viewerPos.z - g1.modelTransform.M43);
    dist2.set(viewerPos.x - g2.modelTransform.M41,
              viewerPos.y - g2.modelTransform.M42,
              viewerPos.z - g2.modelTransform.M43);
    float diff = dist1.lensquared() - dist2.lensquared();
     
    if( alphaVar1 ) // then both use alpha (if alphaVar2 were 0, we'd already have returned above)
    { // farthest first
        if (diff > 0.001f)      return -1;
        else if (diff < 0.001f) return +1;
    }
    else
    { // nearest first
        if (diff > 0.001f)      return +1;
        else if (diff < 0.001f) return -1;
    }

    // by identical scene node
    cmp = int(g1.sceneNode) - int(g2.sceneNode);
    if (cmp != 0)
    {
        return cmp;
    }

    // nodes are identical
    return 0;
}
//------------------------------------------------------------------------------
/** 
    Split the collected scene nodes into light and shape nodes. Fills
    the lightArray[] and shapeArray[] members. This method is available
    as a convenience method for subclasses.
*/  
void
CCSceneServer::SplitNodes(uint shaderFourCC)
{   // identical to nSceneServer::SplitNodes, except for 
    // the addition of SetLOD()
    this->shapeBucket.Clear();
    ushort num = this->groupArray.Size();
    for (ushort i = 0; i < num; ++i)
    {
        Group& group = this->groupArray[i];
        n_assert(group.sceneNode);

        if (group.sceneNode->HasGeometry())
        {
            if( SetLOD( group ) )
            {
                nMaterialNode* shapeNode = (nMaterialNode*) group.sceneNode;
                nShader2* shader = shapeNode->GetShaderObject(shaderFourCC);
                if (shader)
                {
                    uint shaderBucket = shader->GetShaderIndex();
                    this->shapeBucket[shaderBucket].Append(i);
                }
            }
        }
        if (group.sceneNode->HasLight())
        {
            this->lightArray.Append(i);
        }
    }
}
//------------------------------------------------------------------------------
/**
    Render all light/shape interactions matching a given fourcc shader code.
*/
//void
//CCSceneServer::RenderLightShapes(uint shaderFourCC)
//{
//    nGfxServer2* gfxServer = this->refGfxServer;
//
//    // for each bucket...
//    int bucketIndex;
//    int numBuckets = this->shapeBucket.Size();
//    for (bucketIndex = 0; bucketIndex < numBuckets; bucketIndex++)
//    {
//        const nArray<ushort>& shapeArray = this->shapeBucket[bucketIndex];
//        int numShapes = shapeArray.Size();
//
//        // activate bucket shader
//        if (numShapes > 0)
//        {
//            nShader2* curShader = this->GetBucketShader(bucketIndex, shaderFourCC);
//            if (curShader)
//            {
//                // for each shader pass...
//                int numPasses = curShader->Begin();
//                int curPass;
//                for (curPass = 0; curPass < numPasses; curPass++)
//                {
//                    curShader->Pass(curPass);
//
//                    // for each shape in bucket...
//                    int shapeIndex;
//                    for (shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
//                    {
//                        Group& shapeGroup = this->groupArray[shapeArray[shapeIndex]];
//                        if( SetLOD( shapeGroup ) )
//                        {
//                            nMaterialNode* shapeNode = (nMaterialNode*) shapeGroup.sceneNode;
//
//                            // set the modelview matrix for the shape
//                            gfxServer->SetTransform(nGfxServer2::Model, shapeGroup.modelTransform);
//
//                            // prepare shader
//                            shapeNode->RenderShader(shaderFourCC, this, shapeGroup.renderContext);
//                            this->UpdateShader(curShader, shapeGroup.renderContext);
//
//                            // render the "most important light" into the shader (just the first light)
//                            int numLights = this->lightArray.Size();
//                            if (numLights > 0)
//                            {
//                                Group& lightGroup = this->groupArray[this->lightArray[0]];
//                                lightGroup.sceneNode->RenderLight(this, lightGroup.renderContext, lightGroup.modelTransform);
//                            }
//
//                            // render!
//                            shapeNode->RenderGeometry(this, shapeGroup.renderContext);
//                        }
//                    }
//                }
//                curShader->End();
//            }
//        }
//    }
//}
//
//------------------------------------------------------------------------------
/**
    Determine which level of detail should be used for a given node, based on 
    its distance from the viewer and mesh, and set the scene node accordingly.
    Returns false if the node should not be rendered at all.
*/

bool CCSceneServer::SetLOD( Group& shapeGroup )
{
    bool retVal = true;
    nVariable* LODDistVar = shapeGroup.renderContext->GetVariable( m_LODDistHandle );
    if( LODDistVar )
    {   // Look through the array of LOD threshhold values
        // The standard model will be used for a distance-to-camera of up to LODThreshholds[0]
        // Between LODThreshholds[0] and LODThreshholds[1], an alternative (presumably lower poly) model will be substituted
        // Between LODThreshholds[1] and LODThreshholds[2], a billboard will be used
        // Beyond LODThreshholds[2], the object will not be displayed.
        // Since we have an nFloat4 instead of a float4, we use x, y, z to correspond to indices 0, 1, 2
        vector3 distVec(viewerPos.x - shapeGroup.modelTransform.M41,
                viewerPos.y - shapeGroup.modelTransform.M42,
                viewerPos.z - shapeGroup.modelTransform.M43);

        n_assert( LODDistVar->GetType() == nVariable::Float4 );
        nFloat4 LODThreshholds = LODDistVar->GetFloat4();
        float distSquared = distVec.lensquared();
        if( distSquared > LODThreshholds.x )
        {
            if( distSquared > LODThreshholds.y )
            {
                if( distSquared > LODThreshholds.z )
                    retVal = false; // cull this node
                else
                    SubInBillboard(shapeGroup, 1);
            }
            else
            {
                SubInLowPolyModel(shapeGroup, 0);
            }
        }
    }
    return retVal;
}

void CCSceneServer::SubInLowPolyModel( Group& shapeGroup, int LODShapeIndex )
{
    nVariable* LODVar = shapeGroup.renderContext->GetVariable( m_LODShapeHandles[LODShapeIndex] );
    n_assert( LODVar && LODVar->GetType() && LODVar->GetObj() && ((nRoot*)LODVar->GetObj())->IsA( kernelServer->FindClass( "nscenenode" ) ) );
    shapeGroup.sceneNode = (nSceneNode*)LODVar->GetObj();
    if (!shapeGroup.sceneNode->AreResourcesValid())
    {
        shapeGroup.sceneNode->LoadResources();
    }
}

void CCSceneServer::SubInBillboard( Group& shapeGroup, int LODShapeIndex )
{   // substitute a billboard image (whose nShapeNode is stored in 
    // a variable in the render context) for the standard model
    const vector3& pos = shapeGroup.renderContext->GetTransform().pos_component();
    matrix44 billboardMatrix;
    billboardMatrix.set_translation( viewerPos );
    billboardMatrix.billboard( pos, CCUtil::UP_VECTOR ); 
    billboardMatrix.set_translation( pos );
    shapeGroup.renderContext->SetTransform( billboardMatrix );
    n_assert( !shapeGroup.parent ); // must think about how to handle non-top-level nodes -- can we just steal the code from nSceneServer::BeginGroup, or will that mess up billboarding?
    shapeGroup.modelTransform = shapeGroup.renderContext->GetTransform(); // this handles only the simplest case, e.g. no fancy stuff in RenderTransform.
    SubInLowPolyModel( shapeGroup, LODShapeIndex );
}