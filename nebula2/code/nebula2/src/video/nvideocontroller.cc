//------------------------------------------------------------------------------
//  nvideocontroller.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "video/nvideocontroller.h"
#include "gfx2/ntexture2.h"


//------------------------------------------------------------------------------
/**
*/
nVideoController::nVideoController() :
    upperLeft(0.0f, 0.0f),
    lowerRight(1.0f, 1.0f),
    refHwnd("/sys/env/hwnd"),
    interrupted(false),
    interruptible(false),
    looping(false),
    running(false),
    scaling(false),
    texture(0)
{
}

//------------------------------------------------------------------------------
/**
*/
nVideoController::~nVideoController()
{
    n_assert(!Running());
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoController::SetRenderTarget(nTexture2* v)
{
    n_assert(v != 0);
    n_assert(v->GetUsage() & nTexture2::Dynamic);
    this->texture = v;
}

//------------------------------------------------------------------------------
/**
*/
nTexture2*
nVideoController::RenderTarget() const
{
    n_assert(HasRenderTarget());
    return this->texture;
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoController::HasRenderTarget() const
{
    return this->texture != 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoController::Start()
{
    n_assert(!this->Running());

    this->running = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoController::Stop()
{
    n_assert(this->Running());

    this->running = false;
}

//------------------------------------------------------------------------------
/**
*/
nString
nVideoController::FullPathName(const nString& fn) const
{
    wchar_t widePath[N_MAXPATH];
    nString result = nFileServer2::Instance()->ManglePath(fn.Get());
    mbstowcs(widePath, result.Get(), result.Length() + 1);

    return result;
}

