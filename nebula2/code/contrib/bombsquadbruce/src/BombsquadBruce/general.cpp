#include "BombsquadBruce/general.h"
#include "mathlib/nmath.h"
#include "kernel/nautoref.h"
#include "kernel/nscriptserver.h"
// the following are for BBSkinData
#include "scene/nskinshapenode.h"
#include "variable/nvariableserver.h"
#include "scene/nskinanimator.h"
#include "scene/nrendercontext.h"

static nScriptServer * pScriptSrv = 0;

float BBUtil::Normalize(float angleInRadians)
{
	while(0.0f > angleInRadians)
	{
		angleInRadians += 2.0f * PI;
	}
	return fmodf(angleInRadians, 2.0f * PI);
}

void InitScriptServer()
{
    if( !pScriptSrv ) // first call -- initialize
	{
		nAutoRef<nScriptServer> rScriptSrv( "/sys/servers/script" );
		if( rScriptSrv.isvalid() )
		{
			pScriptSrv = rScriptSrv;
		}
	}
    n_assert( pScriptSrv );
}

void BBUtil::RunScript( const char* fileName )
{
    InitScriptServer( );
	if( pScriptSrv && fileName )
	{
		nString result;
		n_verify( pScriptSrv->RunScript( fileName, result) );
	}
}

void BBUtil::RunScriptFunction( const char* functionName )
{
    InitScriptServer( );
	if( pScriptSrv && functionName )
	{
		nString result;
		n_verify( pScriptSrv->Run( functionName, result) );
	}
}

bool BBUtil::ApproximatelyEqual( float lhs, float rhs ) 
{
	const float FUDGE_FACTOR = 0.001f;
	return lhs < rhs + FUDGE_FACTOR && lhs > rhs - FUDGE_FACTOR; 
}

void BBUtil::SaveStrCmd::operator() ( const char* strToSave, uint cmdId )
{
    if( strToSave )
    {
        nCmd* cmd = m_pPersistServer->GetCmd(m_pCaller, cmdId);
        cmd->In()->SetS(strToSave);
        m_pPersistServer->PutCmd(cmd);
    }
}

BBSkinData::BBSkinData() :     
    m_pSkinAnimatorTimeVar( 0 ),
    m_pSkinAnimatorStateVar( 0 ),
    m_SpeedFactor( 1.0f )
{
}

BBSkinData::~BBSkinData()
{
    if( m_pSkinAnimatorTimeVar )
    {
        m_rSkinAnimator->RenderContextDestroyed( m_pRenderContext );
    }
}

void BBSkinData::Init( nRenderContext& renderContext )
{    
    nSceneNode* sceneNode = renderContext.GetRootNode();
    if( sceneNode->IsA( nKernelServer::Instance()->FindClass( "nskinshapenode" ) ) )
    {
        nKernelServer::Instance()->PushCwd( sceneNode );
        int initialAnimState = 0;  // always start with anim state 0 -- they can change itlater  if they want something else
        m_pRenderContext = &renderContext;
        nSkinShapeNode* pSkinNode = (nSkinShapeNode*)( sceneNode );
        m_rSkinAnimator = pSkinNode->GetSkinAnimator();
        m_rSkinAnimator->RenderContextCreated( m_pRenderContext );

        nVariable::Handle channelHandle = nVariableServer::Instance()->GetVariableHandleByName( m_rSkinAnimator->GetChannel() );
        const nVariable timeVar( channelHandle, 0.0f );
        m_pRenderContext->AddVariable( timeVar );
        m_pSkinAnimatorTimeVar = m_pRenderContext->GetVariable( channelHandle );

        channelHandle = nVariableServer::Instance()->GetVariableHandleByName( m_rSkinAnimator->GetStateChannel() );
        const nVariable stateVar( channelHandle, initialAnimState );
        m_pRenderContext->AddVariable( stateVar );
        m_pSkinAnimatorStateVar = m_pRenderContext->GetVariable( channelHandle );

        const char* weightChannelName;
        m_rSkinAnimator->GetClipAt( initialAnimState, 0, weightChannelName );
        const nVariable weightVar( nVariableServer::Instance()->GetVariableHandleByName( weightChannelName ), 1.0f );
        m_pRenderContext->AddVariable( weightVar );
        nKernelServer::Instance()->PopCwd();
    }
}

void BBSkinData::SetState( int state )
{
    if( m_pSkinAnimatorStateVar )
        m_pSkinAnimatorStateVar->SetInt( state );
}

void BBSkinData::SetSpeedFactor( float factor )
{
    m_SpeedFactor = factor;
}

void BBSkinData::StepTime( float dt )
{
    if( m_pSkinAnimatorTimeVar )
        m_pSkinAnimatorTimeVar->SetFloat( m_SpeedFactor * dt + m_pSkinAnimatorTimeVar->GetFloat() );
}
