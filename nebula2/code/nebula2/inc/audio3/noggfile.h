#ifndef N_OGGFILE_H
#define N_OGGFILE_H
//------------------------------------------------------------------------------
/**
    @class nOggFile
    @ingroup Audio3
    @brief Provide read-access to a ogg file.
    
    (C) 2005 Radon Labs GmbH
*/
#include "audio3/naudiofile.h"
#include "kernel/nfile.h"

#include <vorbis/codec.h>

//------------------------------------------------------------------------------
class nOggFile : public nAudioFile
{
public:
    /// constructor
    nOggFile();
    /// destructor
    virtual ~nOggFile();

    /// open the file
    virtual bool Open(const nString& filename);
    /// close the file
    virtual void Close();

    /// return size of audio data in file in bytes
    virtual int GetSize() const;
    /// return pointer to format description
    virtual WAVEFORMATEX* GetFormat() const;

    /// read bytes from file
    virtual uint Read(void* buffer, uint bytesToRead);
    /// reset to beginning of audio data
    virtual bool Reset();

private:
    static const int    INPUTBUFFER = 4096;

    bool                InitOGG();    
    bool                ReleaseOGG();    

    bool                m_bFileEndReached;
    int                 size;               /* ogg filesize in bytes */

    nFile*              file;               /* Input File ; reads from Memory or File */
    WAVEFORMATEX        wfx;                /* audio format */

    int                 bytesIn;
    char*               bufferIn;

    ogg_sync_state      oSyncState;         /* sync and verify incoming physical bitstream */
    ogg_stream_state    oStreamState;       /* take physical pages, weld into a logical stream of packets */
    ogg_page            oBitStreamPage;     /* one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet          oRawPacket;         /* one raw packet of data for decode */

    vorbis_info         vBitStreamInfo;     /* struct that stores all the static vorbis bitstream settings */
    vorbis_comment      vComments;          /* struct that stores all the bitstream user comments */
    vorbis_dsp_state    vDecoderState;      /* central working state for the packet->PCM decoder */
    vorbis_block        vDecoderWorkSpace;  /* local working space for packet->PCM decode */
    
    bool                readEarlyLoopExit;  /* read:important to continue read-loop correctly */
    bool                endOfStream;        /* read:remember end of the stream */
    int                 keepAlive;
};
//------------------------------------------------------------------------------
#endif
    
