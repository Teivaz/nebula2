//------------------------------------------------------------------------------
//  viewer/viewerapp.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "viewer/viewerapp.h"
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"
#include "gui/nguilabel.h"
#include "gui/nguitextlabel.h"
#include "application/gamestatehandler.h"
#include "kernel/nfileserver2.h"
#include "physics/materialtable.h"
#include "physics/level.h"
#include "properties/videocameraproperty.h"
#include "properties/lightproperty.h"
#include "properties/graphicsproperty.h"
#include "properties/physicsproperty.h"
#include "scene/nsceneserver.h"
#include "managers/entitymanager.h"
#include "managers/factorymanager.h"
#include "managers/focusmanager.h"
#include "cegui/CEGUI.h"

namespace Viewer
{
ViewerApp* ViewerApp::Singleton = 0;

//------------------------------------------------------------------------------
/**
	Constructor
*/
ViewerApp::ViewerApp()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
ViewerApp::~ViewerApp()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Return application name.
*/
nString
ViewerApp::GetAppName() const
{
    return "Mangalore Viewer";
}

//------------------------------------------------------------------------------
/**
    Return a version string.
*/
nString
ViewerApp::GetAppVersion() const
{
    return "1.0";
}

//------------------------------------------------------------------------------
/**
    Return the application vendor. This is usually the publishers company name.
*/
nString
ViewerApp::GetVendorName() const
{
    return "RadonLabs";
}

//------------------------------------------------------------------------------
/**
    Sets up default state which may be modified by command line args and
    user profile.
*/
void
ViewerApp::SetupFromDefaults()
{
    App::SetupFromDefaults();
}

//------------------------------------------------------------------------------
/**
    This initializes some objects owned by DsaApp.
*/
bool
ViewerApp::Open()
{
    //DebugBreak();
    //this->SetupFromCmdLineArgs();
    if (App::Open())
    {
        // FIXME: turn off clip plane fencing and occlusion query (FOR NOW)
        // because of compatibility problems on Radeon cards
        nSceneServer::Instance()->SetClipPlaneFencing(false);
        nSceneServer::Instance()->SetOcclusionQuery(false);

        //Setup GUI
        SetupGui();
        SetupCeGui();

        // redirect resource assigns if necessary, this
        // is the case if the viewer acts as toolkit previewer
        if (this->forceTempAssigns)
        {
            this->RedirectResourceForPreview();
        }
        if (0 != App::GetCmdLineArgs().GetStringArg("-obj"))
        {
            this->SetupCamera();
            this->SetupLightsInScene();
            this->SetupGround();
            if (!this->CreateViewedEntity())
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Clean up objects created by Open().
*/
void
ViewerApp::Close()
{
    App::Close();
}

#ifdef __WIN32__
//------------------------------------------------------------------------------
/*
    Win32 specific method which checks the registry for the Nebula2
    Toolkit's project path. If the reg keys are not found, the
    routine just returns 0.
*/
nString
ViewerApp::ReadProjRegistryKey()
{
    // read the project directory from the registry
    HKEY hKey;
    LONG err;
    err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\RadonLabs\\Nebula2", 0, KEY_QUERY_VALUE, &hKey);
    if (ERROR_SUCCESS != err)
    {
        return nString();
    }
    DWORD keyType;
    char projectDir[512];
    size_t projectDirSize = sizeof(projectDir);
    err = RegQueryValueEx(hKey, "project", 0, &keyType, (LPBYTE) &projectDir, (LPDWORD)&projectDirSize);
    if (ERROR_SUCCESS != err)
    {
        return nString();
    }
    return nString(projectDir);
}
#endif

//------------------------------------------------------------------------------
/**
    Does setup from command line args. This is called after SetupFromDefaults().
*/
void
ViewerApp::SetupFromCmdLineArgs()
{
	App::SetupFromCmdLineArgs();
	const nCmdLineArgs& args = App::GetCmdLineArgs();

	nString objArg = args.GetStringArg("-obj");
    bool forceTempAssigns = args.GetBoolArg("-forcetemp");
    nString projDirArg = args.GetStringArg("-projdir");
    vector3 eyePos(args.GetFloatArg("-eyeposx"), args.GetFloatArg("-eyeposy"), args.GetFloatArg("-eyeposz", 9.0f));
    vector3 eyeCoi(args.GetFloatArg("-eyecoix"), args.GetFloatArg("-eyecoiy"), args.GetFloatArg("-eyecoiz"));
    vector3 eyeUp(args.GetFloatArg("-eyeupx"), args.GetFloatArg("-eyeupy", 1.0f), args.GetFloatArg("-eyeupz"));

#ifdef __WIN32__
    if (0 == projDirArg)
    {
        nString regProjDir = this->ReadProjRegistryKey();
        if (regProjDir.IsValid())
        {
            projDirArg = regProjDir;
        }
    }
#endif
    this->SetProjectDirectory(projDirArg);
    this->SetObjectResourceName(objArg);
    this->SetCameraPos(eyePos);
    this->SetCameraCoi(eyeCoi);
    this->SetCameraUpVec(eyeUp);
    this->SetForceTempAssigns(forceTempAssigns);

}
//------------------------------------------------------------------------------
/**
    This method is called once per-frame by App::Run(). It is used here
    to check if reset is requested and that case to evoke it.
*/
void
ViewerApp::OnFrame()
{
    Input::Server* inputServer = Input::Server::Instance();
    if (inputServer->GetButton("timeReset"))
    {
        if (this->loadedEntity)
        {
            //delete old entity
            Managers::EntityManager::Instance()->RemoveEntity(this->loadedEntity);
            //recreate entity
            CreateViewedEntity();
        }
    }
}
//------------------------------------------------------------------------------
/**
*/
void
ViewerApp::SetupStateHandlers()
{
	// initialize application state handlers
    Ptr<Application::GameStateHandler> gameStateHandler = n_new(Application::GameStateHandler);
    gameStateHandler->SetName("Game");
    gameStateHandler->SetExitState("Exit");
    if (0 != App::GetCmdLineArgs().GetStringArg("-obj"))
    {
        gameStateHandler->SetSetupMode(Application::GameStateHandler::EmptyWorld);
    }
    else
    {
        gameStateHandler->SetSetupMode(Application::GameStateHandler::NewGame);
    }
    this->AddStateHandler(gameStateHandler);
	this->SetState("Game");

    // if a startup level has been set, directly start with the location state
    if (this->GetStartupLevel().IsValid())
    {
		gameStateHandler->SetLevelName(this->GetStartupLevel());
    }
}
//------------------------------------------------------------------------------
/**
*/
void
ViewerApp::SetupLightsInScene()
{
    //create new Entity for light in scene, create lightProperty and attach it to the entity
	Ptr<Game::Entity> entity = Managers::FactoryManager::Instance()->CreateEntityByClassName("Entity");
    Ptr<Game::Property> lightProperty = Managers::FactoryManager::Instance()->CreateProperty("LightProperty");
    entity->AttachProperty(lightProperty);

    //set position of lightsource
    matrix44 lightTransform;
    lightTransform.translate(vector3(100.0f, 100.0f, 100.0f));
    entity->SetMatrix44(Attr::Transform, lightTransform);

    //set Attributes of the lightsource
    entity->SetString(Attr::LightType, "Point");
    entity->SetVector4(Attr::LightColor, vector4(1.0f, 1.0f, 1.0f, 1.0f));
    entity->SetFloat(Attr::LightRange, 1000.0f);
    entity->SetVector4(Attr::LightAmbient, vector4(0.0f, 0.0f, 0.0f, 0.0f));
    entity->SetBool(Attr::LightCastShadows, false);

    //attach lightproperty to entity
    Managers::EntityManager::Instance()->AttachEntity(entity);
    return;
}
//------------------------------------------------------------------------------
/**
*/
void ViewerApp::SetupCamera()
{
    //create new Entity as a camera, attach a videocameraporperty
	Ptr<Game::Entity> entity = Managers::FactoryManager::Instance()->CreateEntityByClassName("Entity");
    Ptr<Game::Property> cameraProperty = Managers::FactoryManager::Instance()->CreateProperty("VideoCameraProperty");
    Ptr<Game::Property> inputProperty = Managers::FactoryManager::Instance()->CreateProperty("InputProperty");
    Ptr<Game::Property> mouseGripperProperty = Managers::FactoryManager::Instance()->CreateProperty("MouseGripperProperty");
    entity->AttachProperty(cameraProperty);
    entity->AttachProperty(inputProperty);
    entity->AttachProperty(mouseGripperProperty);

    //set View as in Maya scene
    matrix44 cameraTransform;
    cameraTransform.translate(this->GetCameraPos());
    entity->SetMatrix44(Attr::Transform, cameraTransform);
    entity->SetVector3(Attr::VideoCameraCenterOfInterest, this->GetCameraCoi());
    entity->SetVector3(Attr::VideoCameraDefaultUpVec, this->GetCameraUpVec());

    //attach camera property to entity
    Managers::EntityManager::Instance()->AttachEntity(entity);
    Managers::FocusManager::Instance()->SetFocusEntity(entity);
}
//------------------------------------------------------------------------------
/**
*/
bool ViewerApp::CreateViewedEntity()
{
    // create a simple entity
    if (this->GetObjectResourceName().IsValid())
    {
        // create a new game entity and the properties to attach
    	Ptr<Game::Entity> entity = Managers::FactoryManager::Instance()->CreateEntityByClassName("Entity");
        Ptr<Properties::PhysicsProperty> physicsProperty = (Properties::PhysicsProperty*) Managers::FactoryManager::Instance()->CreateProperty("PhysicsProperty");
        Ptr<Game::Property> graphicProperty = Managers::FactoryManager::Instance()->CreateProperty("GraphicsProperty");

        //make sure that there is a physics file available
        nFileServer2* fileServer = nFileServer2::Instance();
        nString filename;
        filename.Append("physics:");
        filename.Append(this->GetObjectResourceName());
        filename.Append(".xml");

        if (fileServer->FileExists(filename))
        {
            //if there is physic attach the physics property to the entity
            entity->AttachProperty(physicsProperty);
            //configure physics attribute
            entity->SetString(Attr::Physics, this->GetObjectResourceName());
        }

        // attach properties
        entity->AttachProperty(graphicProperty);

        // configure graphics attribute
        entity->SetString(Attr::Graphics, this->GetObjectResourceName());
        matrix44 entityTransform;
        entity->SetMatrix44(Attr::Transform, entityTransform);

        // attach to world
        Managers::EntityManager::Instance()->AttachEntity(entity);
        this->loadedEntity = entity;

        if (fileServer->FileExists(filename))
        {
            // activate physics entity
            Physics::Entity* physicsEntity = physicsProperty->GetPhysicsEntity();
            n_assert(physicsEntity);
            physicsEntity->SetEnabled(true);
        }
        return true;
    }
    return false;

}
//------------------------------------------------------------------------------
/**
*/
void ViewerApp::SetupGround()
{
    // create a box shape as ground plane
    // FIXME: this is currently invisible... better make a entity with graphic and implement a new physics property for this task
    Physics::Server* physicsServer = Physics::Server::Instance();

    //create a box as ground
    matrix44 m;
    m.translate(vector3(0.0f, -0.5f, 0.0f));
    Ptr<Physics::BoxShape> ground = physicsServer->CreateBoxShape(m, Physics::MaterialTable::StringToMaterialType("Soil"), vector3(100.0f, 0.1f, 100.0f));

    //attach ground to level
    physicsServer->GetLevel()->AttachShape(ground);

}
//------------------------------------------------------------------------------
/**
*/
void ViewerApp::RedirectResourceForPreview(){

    // redirect resource assigns if necessary, this
    // is the case if the viewer acts as toolkit previewer
    nFileServer2* fileServer = nFileServer2::Instance();
    fileServer->SetAssign("physics", "temp:");
    fileServer->SetAssign("meshes", "temp:");
    fileServer->SetAssign("anims", "temp:");
    fileServer->SetAssign("gfxlib", "temp:");
    fileServer->SetAssign("levels", "temp:");
    fileServer->SetAssign("db", "temp:");

    return;
}

//------------------------------------------------------------------------------
/**
    This does not setup the dragbox object.
*/
void
ViewerApp::SetupGui()
{
    // initialize Nebula2 Gui server
    nGuiServer* guiServer = nGuiServer::Instance();
    nKernelServer* kernelServer = nKernelServer::Instance();
    guiServer->SetRootPath("/res/gui");
    guiServer->SetDisplaySize(vector2(float(this->displayMode.GetWidth()), float(this->displayMode.GetHeight())));

    // initialize the overlay gui
    const float borderSize = 0.02f;

    kernelServer->PushCwd(nGuiServer::Instance()->GetRootWindowPointer());

    // create 2 logo labels
    nGuiLabel* leftLabel = (nGuiLabel*) kernelServer->New("nguilabel", "LeftLogo");
    n_assert(leftLabel);
    vector2 leftLabelSize = guiServer->ComputeScreenSpaceBrushSize("leftlogo");
    rectangle leftRect;
    leftRect.v0.set(0.0f + borderSize, 1.0f - leftLabelSize.y - borderSize);
    leftRect.v1.set(leftLabelSize.x + borderSize, 1.0f - borderSize);
    leftLabel->SetRect(leftRect);
    leftLabel->SetDefaultBrush("leftlogo");
    leftLabel->SetPressedBrush("leftlogo");
    leftLabel->SetHighlightBrush("leftlogo");
    leftLabel->OnShow();

    nGuiLabel* rightLabel = (nGuiLabel*) kernelServer->New("nguilabel", "RightLogo");
    n_assert(rightLabel);
    vector2 rightLabelSize = guiServer->ComputeScreenSpaceBrushSize("rightlogo");
    rectangle rightRect;
    rightRect.v0.set(1.0f - rightLabelSize.x - borderSize, 1.0f - rightLabelSize.y - borderSize);
    rightRect.v1.set(1.0f - borderSize, 1.0f - borderSize);
    rightLabel->SetRect(rightRect);
    rightLabel->SetDefaultBrush("rightlogo");
    rightLabel->SetPressedBrush("rightlogo");
    rightLabel->SetHighlightBrush("rightlogo");
    rightLabel->OnShow();

    // create a help text label
    nGuiTextLabel* textLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "HelpLabel");
    n_assert(textLabel);
    textLabel->SetText("F1: toggle GUI\nCtrl+Space: center view\nCtrl+LMB: rotate\nCtrl+MMB: pan\nCtrl+RMB: zoom");
    textLabel->SetFont("GuiSmall");
    textLabel->SetAlignment(nGuiTextLabel::Left);
    textLabel->SetColor(vector4(1.0f, 1.0f, 1.0f, 1.0f));
    textLabel->SetClipping(false);
    vector2 textExtent = textLabel->GetTextExtent();
    rectangle textRect(vector2(0.0f, 0.0f), textExtent);
    textLabel->SetRect(textRect);
    textLabel->OnShow();

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
ViewerApp::SetupCeGui()
{
    CEGUI::SchemeManager::getSingleton().loadScheme("cegui:schemes/TaharezLook.scheme");
    CEGUI::FontManager::getSingleton().createFont("cegui:fonts/Commonwealth-10.font");
    CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

    ceuiServer->DisplayGui("cegui:layouts/Demo7Windows.layout");
}

}; // namespace Viewer
