/**
   @file networkdemoapp.cc
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Definition of NetworkDemoApp class.
   @brief $Id$

   This file is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#include "networkdemo/networkdemoapp.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "network/nsessionservercontext.h"
//------------------------------------------------------------------------------
/**
*/
NetworkDemoApp::NetworkDemoApp(nKernelServer* ks) :
    kernelServer(ks),
    featureSetOverride(nGfxServer2::InvalidFeatureSet),
    isOpen(false),
    netserverSessionOpen("netserverSessionOpen", nArg::Bool),
    netserverSessionNumClients("netserverSessionNumClients", nArg::Int),
    netclientSessionOpen("netclientSessionOpen", nArg::Bool),
    netclientSessionNumServers("netclientSessionNumServers", nArg::Int),
    netclientSessionName("netclientSessionName", nArg::String),
    index(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
NetworkDemoApp::~NetworkDemoApp()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
NetworkDemoApp::Open()
{
    nString result;

    n_assert(!this->isOpen);

    // initialize Nebula servers
    this->refScriptServer   = (nScriptServer*)    kernelServer->New("ntclserver",      "/sys/servers/script");
    this->refGfxServer      = (nGfxServer2*)      kernelServer->New("nd3d9server",     "/sys/servers/gfx");
    this->refResourceServer = (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
    this->refConServer      = (nConServer*)       kernelServer->New("nconserver",      "/sys/servers/console");
    this->refSceneServer    = (nSceneServer*)     kernelServer->New("nstdsceneserver", "/sys/servers/scene");
    this->refVarServer      = (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
    this->refSessionServer  = (nSessionServer*)   kernelServer->New("nsessionserver",  "/sys/servers/sessionserver");
    this->refSessionClient  = (nSessionClient*)   kernelServer->New("nsessionclient",  "/sys/servers/sessionclient");
    this->refNetServer      = (NetServerDemo*)    kernelServer->New("netserverdemo",   "/sys/servers/netserver");
    this->refNetClient      = (NetClientDemo*)    kernelServer->New("netclientdemo",   "/sys/servers/netclient");

    n_assert( this->refNetClient->GetClientStatus() == nNetClient::Invalid );

    // set the gfx server feature set override
    if (this->featureSetOverride != nGfxServer2::InvalidFeatureSet)
    {
        this->refGfxServer->SetFeatureSetOverride(this->featureSetOverride);
    }

    kernelServer->GetFileServer()->SetAssign("proj", kernelServer->GetFileServer()->GetAssign("home"));
    this->refScriptServer->RunScript("home:bin/startup.tcl", result);

    // create scene graph root node
    this->refRootNode = (nTransformNode*) kernelServer->New("ntransformnode",  "/usr/scene");

    // initialize graphics
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->camera);
    this->refGfxServer->OpenDisplay();

    // define the input mapping
    // late initialization of input server, because it relies on
    // refGfxServer->OpenDisplay having been called
    this->refInputServer    = (nInputServer*)     kernelServer->New("ndi8server", "/sys/servers/input");
    this->refScriptServer->RunScript("home:bin/stdinput.tcl", result);

    this->renderContext.SetRootNode(this->refRootNode.get());
    this->refRootNode->RenderContextCreated(&this->renderContext);
    this->refConServer->Watch( "net*" );

    this->netclientSessionName->SetS("<no name>");

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkDemoApp::Close()
{
    n_assert(this->isOpen);

    // disable any possibility of future rendering
    this->refGfxServer->CloseDisplay();
    this->refGfxServer->Release();
    this->refScriptServer->Release();
    this->refResourceServer->Release();
    this->refConServer->Release();
    this->refSceneServer->Release();
    this->refSessionServer->Release();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
NetworkDemoApp::Run()
{

    // run the render loop
    running = true;
    nTime prevTime = 0.0;
    int prevServers = 0;
    while (this->refGfxServer->Trigger() && running)
    {
        kernelServer->GetTimeServer()->Trigger();
        nTime time = kernelServer->GetTimeServer()->GetTime();
        if (prevTime == 0.0)
        {
            prevTime = time;
        }
        float frameTime = (float) (time - prevTime);

        // handle input
        this->refInputServer->Trigger(time);
        this->HandleInput(frameTime);

        if( this->netserverSessionOpen->GetB() )
        {
            this->refSessionServer->SetTime(time);
            this->refSessionServer->Trigger();
            this->netserverSessionNumClients->SetI( this->refSessionServer->GetNumClients() );
        }

        if( this->netclientSessionOpen->GetB() )
        {
            this->refSessionClient->SetTime(time);
            this->refSessionClient->Trigger();
            int num = this->refSessionClient->GetNumServers();
            if( num != prevServers )
            {
                if( num > 0 )
                {
                    nSessionServerContext* server;
                    server = this->refSessionClient->GetServerAt( 0 );
                    this->netclientSessionName->SetS( server->GetHostName() );
                }
                this->netclientSessionNumServers->SetI( num );
                prevServers = num;
            }
        }

        if( this->refNetClient->IsOpen() )
        {
            this->refNetClient->SetTime(time);
            this->refNetClient->Trigger();
        }

        if( this->refNetServer->IsOpen() )
        {
            this->refNetServer->Trigger();
        }

        if (this->refSceneServer->BeginScene(viewMatrix))
        {
            this->refSceneServer->Attach(&this->renderContext);
            this->refSceneServer->EndScene();
            this->refSceneServer->RenderScene();             // renders the 3d scene
            this->refGfxServer->Clear(nGfxServer2::ColorBuffer, 0.0f, 0.4f, 0.5f, 0.0f, 1.0f, 0 );
            this->refConServer->Render();
            this->refSceneServer->PresentScene();            // present the frame
        }

        // flush input events
        this->refInputServer->FlushEvents();

        if( this->refScriptServer->GetQuitRequested() )
        {
            running = false;
        }
        // sleep for a very little while because we
        // are multitasking friendly
        n_sleep(0.0);
    }
}

//------------------------------------------------------------------------------
/**
    Handle general input
*/
void
NetworkDemoApp::HandleInput(float frameTime)
{
    nInputServer* inputServer = this->refInputServer.get();

    if (inputServer->GetButton("exit"))
    {
        running = false;
    }

    if (inputServer->GetButton("init_server") && ! this->refSessionServer->IsOpen() )
    {
        this->refSessionServer->SetAppName( "NetworkDemo" );
        this->refSessionServer->SetAppVersion( "0.1" );
        this->refSessionServer->SetMaxNumClients( 10 );
        this->refSessionServer->SetServerAttr( "PlayerName", "God" );
        this->refSessionServer->Open();
        this->netserverSessionOpen->SetB( true );
        this->refConServer->Watch( "netserver*" );
    }

    if (inputServer->GetButton("init_client") && ! this->refSessionClient->IsOpen() )
    {
        this->refSessionClient->SetAppName( "NetworkDemo" );
        this->refSessionClient->SetAppVersion( "0.1" );
        this->refSessionClient->SetClientAttr( "PlayerName", "Dilbert" );
        this->refSessionClient->Open();
        this->netclientSessionOpen->SetB( true );
        this->refConServer->Watch( "netclient*" );
    }

    if (inputServer->GetButton("join") )
    {
        if( this->refSessionClient->GetNumServers() > 0 )
        {
            nSessionServerContext* server;
            server = this->refSessionClient->GetServerAt( 0 );
            this->refSessionClient->JoinSession( server->GetSessionGuid() );
        }
    }

    if (inputServer->GetButton("start") && this->refSessionServer->IsOpen() )
    {
        this->refSessionServer->Start( );
    }

    if (inputServer->GetButton("message") )
    {
        this->refNetClient->SendMessage("Hello World!");
    }

    if (inputServer->GetButton("index_up") )
    {
        ++ this->index;
    }

    if (inputServer->GetButton("index_down") )
    {
        -- this->index;
    }

    // toggle console
    if( inputServer->GetButton("console") )
    {
        this->refConServer->Toggle();
    }
}
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
