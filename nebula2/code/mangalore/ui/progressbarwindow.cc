//------------------------------------------------------------------------------
//  progressbarwindow.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/progressbarwindow.h"
#include "graphics/lightentity.h"
#include "graphics/cameraentity.h"
#include "graphics/server.h"
#include "scene/nsceneserver.h"
#include "ui/server.h"
#include "kernel/nfileserver2.h"

namespace UI
{
ImplementRtti(UI::ProgressBarWindow, UI::Window);
ImplementFactory(UI::ProgressBarWindow);

//------------------------------------------------------------------------------
/**
*/
ProgressBarWindow::ProgressBarWindow() :
    rangeMaxValue(100),
    rangeCurValue(0),
    debugTextEnabled(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ProgressBarWindow::~ProgressBarWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This setup a message of the day string from data/messageoftheday.txt.
*/
void
ProgressBarWindow::SetupMessageOfTheDay()
{
    this->messageOfTheDay.Clear();
    nFile* file = nFileServer2::Instance()->NewFileObject();
    if (file->Open("data:messageoftheday.txt", "r"))
    {
        char line[1024];
        if (file->GetS(line, sizeof(line)))
        {
            this->messageOfTheDay = line;
        }
        file->Close();
    }
    file->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
ProgressBarWindow::Open()
{
    // reset values
    this->rangeMaxValue = 100;
    this->rangeCurValue = 0;
    this->text.Clear();

    // create our own camera entity
    this->cameraEntity = Graphics::CameraEntity::Create();
    this->cameraEntity->OnActivate();

    // get optional message of the day
    this->SetupMessageOfTheDay();

    // call parent class
    Window::Open();

    // clear time source of canvas, so that the graphics server's time is used
    this->canvas->GetGraphicsEntity()->SetTimeSource(0);
}

//------------------------------------------------------------------------------
/**
*/
void
ProgressBarWindow::Close()
{
    // destroy camera entity
    this->cameraEntity->OnDeactivate();
    this->cameraEntity = 0;

    // call parent class
    Window::Close();
}

//------------------------------------------------------------------------------
/**
    This does a full autonomous render loop for this window.
*/
void
ProgressBarWindow::Present()
{
    // establish links between our graphics entity, the UI light entity
    // and the current camera entity
    Graphics::Entity* gfxEntity = this->canvas->GetGraphicsEntity();
    if (gfxEntity)
    {
        Graphics::LightEntity* lightEntity = UI::Server::Instance()->GetLightEntity();
        // we need to convert the progress into a time between 0.0 and 1.0,
        // so the "progress" must be visualized in the graphics resource
        // with an animation between 0 and 1
        nTime activationTime = gfxEntity->GetActivationTime();
        nTime progressTime = nTime(this->rangeCurValue) / nTime(this->rangeMaxValue);
        Graphics::Server::Instance()->SetTime(activationTime + progressTime);

        // clear and rebuild graphics links, the light link between
        // light and gfx entity are already established
        lightEntity->ClearLinks(Graphics::Entity::CameraLink);
        this->cameraEntity->ClearLinks(Graphics::Entity::CameraLink);
        gfxEntity->ClearLinks(Graphics::Entity::CameraLink);

        gfxEntity->AddLink(Graphics::Entity::CameraLink, this->cameraEntity);
        this->cameraEntity->AddLink(Graphics::Entity::CameraLink, gfxEntity);

        lightEntity->AddLink(Graphics::Entity::CameraLink, this->cameraEntity);
        this->cameraEntity->AddLink(Graphics::Entity::CameraLink, lightEntity);
    }

    // render the scene
    nSceneServer* sceneServer = nSceneServer::Instance();
    if (sceneServer->BeginScene(this->cameraEntity->GetTransform()))
    {
        this->cameraEntity->OnRenderBefore();
        this->cameraEntity->Render();

        if (this->debugTextEnabled)
        {
            // draw optional message of the day
            if (this->messageOfTheDay.IsValid())
            {
                nGfxServer2* gfxServer = nGfxServer2::Instance();
                rectangle rect(vector2(0.0f, 0.65f), vector2(1.0f, 0.1f));
                vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
                gfxServer->DrawText(this->messageOfTheDay, color, rect, nFont2::Center|nFont2::VCenter|nFont2::NoClip, false);
            }

            // draw the text
            if (this->text.IsValid())
            {
                nGfxServer2* gfxServer = nGfxServer2::Instance();
                rectangle rect(vector2(0.0f, 0.7f), vector2(1.0f, 0.1f));
                vector4 color(1.0f, 0.5f, 0.0f, 1.0f);
                gfxServer->DrawText(this->text, color, rect, nFont2::Center|nFont2::VCenter|nFont2::NoClip, false);
            }
        }

        // finish rendering
        sceneServer->EndScene();
        sceneServer->RenderScene();
        sceneServer->PresentScene();
    }
}

} // namespace UI
