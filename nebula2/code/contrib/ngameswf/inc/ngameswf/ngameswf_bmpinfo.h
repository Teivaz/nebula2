//------------------------------------------------------------------------------
//  ngameswf_bmpinfo.h
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#ifndef N_GAMESWF_BMPINFO_H
#define N_GAMESWF_BMPINFO_H

#include <base/image.h>
#include <gameswf/gameswf.h>
#include <gameswf/gameswf_types.h>

#include "kernel/ntypes.h"
#include "kernel/nautoref.h"
#include "mathlib/matrix.h"
#include "resource/nresourceloader.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"
#include "gfx2/nsurface.h"
#include "variable/nvariableserver.h"
#include "gfx2/nmesh2.h"

//-----------------------------------------------------------------------------
/**
    @struct bitmap_info_nebula
    @ingroup nGameSwfGroup

    @brief bitmap_info_nebula class derived from gameswf::bitmap_info.
           This class have information and functionality ngameswf needs
           to render using textures
*/
struct bitmap_info_nebula : public gameswf::bitmap_info
{
    nRef<nTexture2>   ref_texture;

    char resource_loader_path[N_MAXPATH];

    nTexture2::Format pixel_format;

    uchar* image_data;

    int image_width;
    int image_height;
    int image_pitch;

    bitmap_info_nebula ();
    bitmap_info_nebula (int, int, uchar*, nGfxServer2*, const char*, int);
    bitmap_info_nebula (image::rgb*, nGfxServer2*, const char*, int);
    bitmap_info_nebula (image::rgba*, nGfxServer2*, const char*, int);

    virtual ~bitmap_info_nebula();

    virtual void load (nGfxServer2*);

};//end of bitmap_info_nebula

#endif /*N_GAMESWF_BMPINFO_H*/