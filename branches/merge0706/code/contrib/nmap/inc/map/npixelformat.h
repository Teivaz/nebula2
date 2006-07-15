#ifndef N_PIXELFORMAT_H
#define N_PIXELFORMAT_H
//-------------------------------------------------------------------
/**
    @class nPixelFormat
    @ingroup NMapContribModule
    @brief describe a pixelformat and convert between then

    restrictions:
    -------------
     - N_PFTYPE_CLUT (paletted formats) other then 8 bpp are illegal
     - N_PFTYPE_RGBA formats other then 16,24 or 32 bpp are illegal
     - only the following conversions are valid:
       - N_PFTYPE_RGBA -> N_PFTYPE_RGBA
       - N_PFTYPE_CLUT -> N_PFTYPE_RGBA
       - N_PFTYPE_CLUT -> N_PFTYPE_CLUT
       the latter is always a straight copy
*/
//-------------------------------------------------------------------
#include "kernel/ntypes.h"

//-------------------------------------------------------------------
enum nPixelFormatType {
    N_PFTYPE_CLUT,          // Farbpaletten-Format
    N_PFTYPE_RGBA,          // ein RGB(A) FOrmat
};

struct nPalEntry {
    uchar r,g,b,a;
};

class nPixelFormat {
public:
    nPixelFormatType type;
    int bpp;                
    struct nPalEntry *pal;
    unsigned int r_mask,g_mask,b_mask,a_mask;
    unsigned int r_shift,g_shift,b_shift,a_shift;
    nPixelFormat *pf_from;
    
    nPixelFormat(int bpp, nPalEntry *pal);    // impliziert N_PFTYPE_CLUT
    nPixelFormat(int bpp,                     // impliziert N_PFTYPE_RGBA
                 unsigned int r_mask,
                 unsigned int g_mask,
                 unsigned int b_mask,
                 unsigned int a_mask);
    nPixelFormat(nPixelFormat *);
    ~nPixelFormat();
    bool BeginConv(nPixelFormat *from);
    void Conv(uchar *from, uchar *to, int num_pixels);
    void EndConv(void);
};
//-------------------------------------------------------------------
#endif                 
