//------------------------------------------------------------------------------
//  ncaptureserver_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "misc/ncaptureserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "gfx2/ngfxserver2.h"

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
    return nFileServer2::Instance()->MakePath(dirName.Get());
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
    while (fileServer->DirectoryExists(dirName.Get()));
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
    bool success = nGfxServer2::Instance()->SaveScreenshot(filename.Get());
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

