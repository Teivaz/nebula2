//------------------------------------------------------------------------------
//  ncaptureserver_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "misc/ncaptureserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "gfx2/ngfxserver2.h"
#include "scene/nsceneserver.h"
#include "resource/nresourceserver.h"

nNebulaScriptClass(nCaptureServer, "nroot");
nCaptureServer* nCaptureServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nCaptureServer::nCaptureServer() :
    isCapturing(false),
    frameTime(0.04),
    baseDirName("temp:"),
    sessionIndex(0),
    frameIndex(0)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nCaptureServer::~nCaptureServer()
{
    if (this->IsCapturing())
    {
        this->Stop();
    }
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nCaptureServer::Start()
{
    n_assert(!this->IsCapturing());

    // find next session index
    this->SetToNextSessionIndex();

    // create the session directory
    if (this->CreateSessionDirectory())
    {
        // lock frame rate
        nTimeServer::Instance()->LockDeltaT(this->frameTime);
        this->isCapturing = true;
        this->frameIndex = 0;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCaptureServer::Stop()
{
    n_assert(this->IsCapturing());
    nTimeServer::Instance()->LockDeltaT(0.0);
    this->isCapturing = false;
}

//------------------------------------------------------------------------------
/**
    Returns the name of the current session.
*/
nString
nCaptureServer::GetSessionDirectoryPath()
{
    nString dirName;
    dirName.Format("%s/%04d", this->baseDirName.Get(), this->sessionIndex);
    return dirName;
}

//------------------------------------------------------------------------------
/**
    Create a directory for the current session.
*/
bool
nCaptureServer::CreateSessionDirectory()
{
    nString dirName = this->GetSessionDirectoryPath();
    return nFileServer2::Instance()->MakePath(dirName);
}

//------------------------------------------------------------------------------
/**
    This finds the next free session index by iterating through session numbers
    and checking whether a session directory of that name already exists.
*/
void
nCaptureServer::SetToNextSessionIndex()
{
    nFileServer2* fileServer = nFileServer2::Instance();
    nString dirName;
    do
    {
        this->sessionIndex++;
        dirName = this->GetSessionDirectoryPath();
    }
    while (fileServer->DirectoryExists(dirName));
}

//------------------------------------------------------------------------------
/**
    Capture the current frame to a file.
*/
bool
nCaptureServer::CaptureFrame()
{
    n_assert(this->IsCapturing());

    // create file name
    nString filename;
    filename.Format("%s/%05d.jpg", this->GetSessionDirectoryPath().Get(), ++this->frameIndex);

    // write screen shot
    bool success = nGfxServer2::Instance()->SaveScreenshot(filename.Get(), nTexture2::JPG);
    return success;
}

//------------------------------------------------------------------------------
/**
    Trigger the frame grabber. This will just call CaptureFrame() if a capture
    session is currently open.
*/
bool
nCaptureServer::Trigger()
{
    if (this->IsCapturing())
    {
        return this->CaptureFrame();
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Toggle capturing.
*/
void
nCaptureServer::Toggle()
{
    if (this->IsCapturing())
    {
        this->Stop();
    }
    else
    {
        this->Start();
    }
}

//------------------------------------------------------------------------------
/**
    Creates a high-resolution screenshot by subdividing the current
    view into a number of tiles, and rendering and saving each tile into a
    separate screenshot file.
*/
bool
nCaptureServer::SaveTiledScreenShot(int numTilesX, int numTilesY)
{
    n_assert(numTilesX > 0);
    n_assert(numTilesY > 0);
    bool retval = true;
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nSceneServer* sceneServer = nSceneServer::Instance();

    // prepare screenshot capture
    bool origGuiEnabled = sceneServer->GetGuiEnabled();
    bool origOcclusionQuery = sceneServer->GetOcclusionQuery();
    bool origClipPlaneFencing = sceneServer->GetClipPlaneFencing();
    bool origCamerasEnabled = sceneServer->GetCamerasEnabled();
    sceneServer->SetGuiEnabled(false);
    sceneServer->SetOcclusionQuery(false);
    sceneServer->SetClipPlaneFencing(false);
    sceneServer->SetCamerasEnabled(false);

    this->SetToNextSessionIndex();
    this->CreateSessionDirectory();

    // get current camera from gfx server
    nCamera2 origCamera = gfxServer->GetCamera();

    // derive view volume width and height from the projection matrix
    const matrix44& origProj = origCamera.GetProjection();
	sceneServer->SaveProjectionMatrix(origProj);

    // use hard coded Near/Far-Planes to improve Z-Buffer accuracy
    float zNear = 0.5f;
    float zFar  = 1500.0f;
    float w = (2.0f * zNear) / origProj.m[0][0];
    float h = (2.0f * zNear) / origProj.m[1][1];

    // compute view volume top/left values and step sizes
    float y = -h * 0.5f;
    float dy = h / numTilesY;
    int tileY;
    for (tileY = 0; tileY < numTilesY; tileY++)
    {
        int tileX;
        float x = -w * 0.5f;
        float dx = w / numTilesX;
        for (tileX = 0; tileX < numTilesX; tileX++)
        {
            // create tile projection matrix
            matrix44 tileProj;
            tileProj.perspOffCenterRh(x, x + dx, y, y + dy, zNear, zFar);

            // manipulate camera entity and render a complete frame
            nCamera2 tileCamera;
            tileCamera.SetProjectionMatrix(tileProj);
			this->SetCorrectRenderpathOffset(numTilesX, numTilesY, tileX, tileY, false);

			gfxServer->SetCamera(tileCamera);
            gfxServer->BeginFrame();
            sceneServer->RenderScene();
            gfxServer->EndScene();

            // write screenshot, NOTE: screenshot copies the backbuffer, thus it
            // is important that we don't do a screen flip before taking the shot
            nString filename;
            filename.Format("%s/Y%02d_X%02d.png", this->GetSessionDirectoryPath().Get(), tileY, tileX);
            gfxServer->SaveScreenshot(filename.Get(), nTexture2::PNG);

            gfxServer->PresentScene();
            gfxServer->EndFrame();
            x += dx;
        }
        y += dy;
    }

    // restore original settings...
    gfxServer->SetCamera(origCamera);
    sceneServer->SetGuiEnabled(origGuiEnabled);
    sceneServer->SetOcclusionQuery(origOcclusionQuery);
    sceneServer->SetClipPlaneFencing(origClipPlaneFencing);
    sceneServer->SetCamerasEnabled(origCamerasEnabled);
	this->SetCorrectRenderpathOffset(0, 0, 0, 0, true);
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
void
nCaptureServer::SetCorrectRenderpathOffset(int numTilesX, int numTilesY, int tileXNum, int tileYNum, bool reset)
{
	nFloat4 offset;
	if (reset)
	{
        offset.x = 0.0f;
        offset.y = 1.0f;
        offset.z = 0.0f;
        offset.w = 1.0f;
	}
	else
	{
        offset.x = (float)tileXNum / (float)numTilesX;
        offset.y =  offset.x + 1.0f / (float)numTilesX;
        offset.z = (float)tileYNum / (float)numTilesY;
        offset.w = offset.z + 1.0f / (float)numTilesY;
	}
    nShader2* shaderParameter = (nShader2*)nResourceServer::Instance()->FindResource("shared", nResource::Shader);
	shaderParameter->SetFloat4(nShaderState::RenderTargetOffset, offset);
}
