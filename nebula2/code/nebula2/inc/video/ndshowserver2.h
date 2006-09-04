#ifndef N_DSHOWSERVER2_H
#define N_DSHOWSERVER2_H
//------------------------------------------------------------------------------
/**
    *** OBSOLETE *** OBSOLETE *** OBSOLETE ***

    @class nDShowServer2
    @ingroup Video

    Server objects to playback video streams via video controllers implemented
    with DirectShow.

    (C) 2005 Radon Labs GmbH
*/
#include "video/nvideoserver2.h"

//------------------------------------------------------------------------------
class nDShowServer2 : public nVideoServer2
{
public:
    /// constructor
    nDShowServer2();
    /// destructor
    virtual ~nDShowServer2();

    /// Initialize.
    virtual void Open();
    /// Finish.
    virtual void Close();

protected:
    /// New video controller using DirectShow.
    virtual nVideoController* MakeController(const nString& vn) const;

};


//------------------------------------------------------------------------------
#endif

