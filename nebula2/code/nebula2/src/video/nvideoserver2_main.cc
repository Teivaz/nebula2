//------------------------------------------------------------------------------
//  nvideoserver2_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "video/nvideocontroller.h"
#include "video/nvideoserver2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nVideoServer2, "nroot");

nVideoServer2* nVideoServer2::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nVideoServer2::nVideoServer2() :
    refInputServer("/sys/servers/input"),
    isOpen(false),
    needCheckDialogBoxMode(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nVideoServer2::~nVideoServer2()
{
    n_assert(CountVideos() == 0);
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::Open()
{
    n_assert(!IsOpen());

    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::Close()
{
    n_assert(IsOpen());

    RemoveVideos();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::AddVideo(const nString& vn)
{
    n_assert(IsOpen());
    n_assert(!HasVideo(vn));

    this->controller.PushBack(MakeController(vn));

    n_assert(HasVideo(vn));
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::RemoveVideo(const nString& vn)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    int index = IndexOfController(vn);
    if (this->controller[index]->Running())
    {
        this->controller[index]->Stop();
    }
    delete this->controller[index];
    this->controller.Erase(index);

    n_assert(!HasVideo(vn));
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::RemoveVideos()
{
    for (int i = 0; i < this->controller.Size(); i++)
    {
        if (this->controller[i]->Running())
        {
            this->controller[i]->Stop();
        }
        delete this->controller[i];
        this->controller[i] = 0;
    }
    this->controller.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoServer2::HasVideo(const nString& vn) const
{
    n_assert(IsOpen());
    return IndexOfController(vn) >= 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::SetInterruptible(const nString& vn, bool v)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    this->controller[IndexOfController(vn)]->SetInterruptible(v);
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoServer2::IsInterruptible(const nString& vn) const
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    return this->controller[IndexOfController(vn)]->IsInterruptible();
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::SetLooping(const nString& vn, bool v)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    this->controller[IndexOfController(vn)]->SetLooping(v);
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoServer2::IsLooping(const nString& vn) const
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    return this->controller[IndexOfController(vn)]->IsLooping();
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::SetScaling(const nString& vn, bool v)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    this->controller[IndexOfController(vn)]->SetScaling(v);
}

//------------------------------------------------------------------------------
/**
*/
bool
nVideoServer2::IsScaling(const nString& vn) const
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    return this->controller[IndexOfController(vn)]->IsScaling();
}

//------------------------------------------------------------------------------
/**
    Align in current window. Set upper left corner to `ul' and lower right
    to `lr'. All values in percent ([0..1]).
*/
void
nVideoServer2::SetVideoPosition(const nString& vn, const vector2& ul, const vector2& lr)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    this->controller[IndexOfController(vn)]->SetPosition(ul, lr);
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::SetRenderTarget(const nString& vn, nTexture2* v)
{
    n_assert(HasVideo(vn));
    n_assert(v != 0);
    n_assert(v->GetUsage() & nTexture2::Dynamic);

    this->controller[IndexOfController(vn)]->SetRenderTarget(v);
}

//------------------------------------------------------------------------------
/**
    Video must be available!
*/
void
nVideoServer2::StartVideo(const nString& vn)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));
    n_assert(!VideoRunning(vn));

    int index = IndexOfController(vn);
    this->controller[index]->Start();

    if (!this->controller[index]->HasRenderTarget())
    {
        this->needCheckDialogBoxMode = true;
    }
}

//------------------------------------------------------------------------------
/**
    Video must be available!
*/
void
nVideoServer2::StopVideo(const nString& vn)
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));
    n_assert(VideoRunning(vn));

    this->controller[IndexOfController(vn)]->Stop();
}

//------------------------------------------------------------------------------
bool
nVideoServer2::VideoRunning(const nString& vn) const
{
    n_assert(IsOpen());
    n_assert(HasVideo(vn));

    return this->controller[IndexOfController(vn)]->Running();
}

//------------------------------------------------------------------------------
/**
    Update all existing video controller.
*/
void
nVideoServer2::Trigger()
{
    n_assert(IsOpen());

    // Check for user interruption.
    nInputEvent* i;
    for (i = this->refInputServer->FirstEvent();
         i != 0;
         i = this->refInputServer->NextEvent(i))
    {
        if ((i->GetType() == N_INPUT_KEY_DOWN) &&
            ((i->GetKey() == N_KEY_ESCAPE) || (i->GetKey() == N_KEY_SPACE)))
        {
            for (int j = 0; j < this->controller.Size(); j++)
            {
                if (this->controller[j]->Running() &&
                    this->controller[j]->IsInterruptible())
                {
                    this->controller[j]->Interrupt();
                }
            }
            break;
        }
    }

    // Refresh state of all controllers.
    for (int j = 0; j < this->controller.Size(); j++)
    {
        if (this->controller[j]->Running())
        {
            this->controller[j]->Update();
        }
    }

    // Check if gfx server dialog box mode is needed. Enable/disable
    // mode if nessecary.
    if (this->needCheckDialogBoxMode)
    {
        CheckDialogBoxMode();
    }
}

//------------------------------------------------------------------------------
/**
*/
nVideoController*
nVideoServer2::MakeController(const nString& vn) const
{
    n_assert(false); // "Pure virtual"...
    return 0;
}

//------------------------------------------------------------------------------
/**
    Index in controller array if controller associated with `vn' exists. Otherwise
    -1.
*/
int
nVideoServer2::IndexOfController(const nString& vn) const
{
    for (int i = 0; i < this->controller.Size(); i++)
    {
        if (this->controller[i]->VideoName() == vn)
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
*/
void
nVideoServer2::CheckDialogBoxMode()
{
    bool needDialogBoxMode = false;
    for (int i = 0; i < this->controller.Size(); i++)
    {
        if (!this->controller[i]->HasRenderTarget() &&
            this->controller[i]->Running())
        {
            needDialogBoxMode = true;
            break;
        }
    }
    if (needDialogBoxMode)
    {
        if (!nGfxServer2::Instance()->InDialogBoxMode())
        {
            nGfxServer2::Instance()->EnterDialogBoxMode();
        }
    }
    else
    {
        if (nGfxServer2::Instance()->InDialogBoxMode())
        {
            nGfxServer2::Instance()->LeaveDialogBoxMode();
            this->needCheckDialogBoxMode = false;
        }
    }
}
