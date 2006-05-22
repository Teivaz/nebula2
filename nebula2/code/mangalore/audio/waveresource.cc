//------------------------------------------------------------------------------
//  audio/waveresource.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "audio/waveresource.h"

namespace Audio
{
ImplementRtti(Audio::WaveResource, Foundation::RefCounted);
ImplementFactory(Audio::WaveResource);

//------------------------------------------------------------------------------
/**
*/
WaveResource::WaveResource() :
    volume(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
WaveResource::~WaveResource()
{
    int i;
    int num = this->sounds.Size();
    for (i = 0; i < num; i++)
    {
        this->sounds[i]->Release();
        this->sounds[i].invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
WaveResource::IsPlaying() const
{
    int i;
    int num = this->sounds.Size();
    for (i = 0; i < num; i++)
    {
        if (this->sounds[i]->IsPlaying())
        {
            return true;
        }
    }
    return false;
}

} // namespace Audio
