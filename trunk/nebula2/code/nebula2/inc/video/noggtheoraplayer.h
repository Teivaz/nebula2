#ifndef N_OGGTHEORAPLAYER_H
#define N_OGGTHEORAPLAYER_H
//------------------------------------------------------------------------------
/**
    @class nOggTheoraPlayer
    @ingroup Video

    A videoplayer for *.ogg files
    Most parts are just copied out of the decoder-example of the theora-package
    and slightly modified to match the nVideoPlayer interface

    (C) 2005 RadonLabs GmbH
*/
#include "video/nvideoplayer.h"
#include "ogg/theora.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
class nOggTheoraPlayer : public nVideoPlayer
{
public:
    /// constructor
    nOggTheoraPlayer();
    /// destructor
    virtual ~nOggTheoraPlayer();
    /// open the video player
    virtual bool Open();
    /// close the video player
    virtual void Close();

    /// Decode the next Frame (stores frame into the texture)
    virtual void DecodeNextFrame();
    /// Decode the next Frames if necessary (and stores frame into the texture)
    virtual void Decode(nTime deltaTime);
    /// Rewinds the video
    virtual void Rewind();

protected:
    /// Helper; just grab some more compressed bitstream and sync it for page extraction
    int buffer_data(nFile *in,ogg_sync_state *oy);
    /// helper: push a page into the steam for packetization */
    int queue_page(ogg_page *page);
    /// Decodes a YUV frame into RGB data
    void DecodeYUV(yuv_buffer& yuv,unsigned char* rgbBuffer);
    /// shuts down theora
    void StopTheora();
    /// Store texturedata
    void UpdateTexture();


    /* Ogg and codec state for demux/decode */
    ogg_sync_state   oy;
    ogg_page         og;
    ogg_stream_state vo;
    ogg_stream_state to;
    theora_info      ti;
    theora_comment   tc;
    theora_state     td;

    int              theora_p;
    int              stateflag;

    /* single frame video buffering */
    int          videobuf_ready;
    ogg_int64_t  videobuf_granulepos;
    double       videobuf_time;
    int          frameNr;

    ogg_packet      op;
    nFile           *infile;
    yuv_buffer      yuv;

    unsigned char   *rgbBuffer;
    int             fileVideoDataOffset;
    bool            theoraLoaded;
    bool            isPlaying;
    nTime           currentTime;
    uint            decodedFrames;
};

#endif

