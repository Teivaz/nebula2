#ifndef N_STREAMINGANIMATION_H
#define N_STREAMINGANIMATION_H
//------------------------------------------------------------------------------
/**
    @class nStreamingAnimation
    @ingroup Anim2

    @brief A streaming animation streams its animation data from disk into
    a small memory buffer. They can generally not be shared like
    nMemoryAnimation objects (since animations with different sample
    times would thrash their stream buffers all the time). The advantage
    is of course that stream animations can be very long (only limited
    by available disk space).

    Streaming animations are generally created through the
    nAnimationServer::NewStreamingAnimation() method.

    See the nAnimation class documentation for more information.

    Note that ascii <tt>nanim2</tt> files cannot be streamed, only binary
    <tt>nax2</tt> files.

    (C) 2003 RadonLabs GmbH
*/
#include "animation/nanimation.h"

//------------------------------------------------------------------------------
class nStreamingAnimation : public nAnimation
{
public:
    /// constructor
    nStreamingAnimation();
    /// destructor
    virtual ~nStreamingAnimation();
    /// load the resource (sets the valid flag)
    virtual bool Load();
    /// unload the resource (clears the valid flag)
    virtual void Unload();
    /// sample value of given curve at given time
    virtual void Sample(int curveIndex, float time, vector4& result);

private:
    /// open an nax2 file and read initial data
    bool OpenNax2(const char* filename, const char* groupName);
    /// fill the stream buffer with a new data block
    bool StreamData(int firstKey, int numKeys);

    nFile* streamFile;          //< file object pointing to the stream file
    int groupSeekPos;           //< seek pos to start of animation group keys
    float4* streamBuffer;       //< stream buffer, layout is the same as described in nmemoryanimation.h
    int bufferStartKey;         //< the key index at the start of the stream buffer
    int bufferNumKeys;          //< number of keys in buffer
};
//------------------------------------------------------------------------------
#endif
