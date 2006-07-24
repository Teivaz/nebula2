//-------------------------------------------------------------------
//  npixelformat.cc
//  (C) 1999 Andre Weissflog
//-------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "map/npixelformat.h"

typedef unsigned long ulong ;

//-------------------------------------------------------------------
//  nPixelFormat(bpp,palette)
//  19-Nov-98   floh    created
//  17-Dec-99   floh    updated
//-------------------------------------------------------------------
nPixelFormat::nPixelFormat(int _bpp, nPalEntry *_pal)
{
    n_assert(_bpp == 8);
    int pal_size = (1<<_bpp) * sizeof(nPalEntry);

    this->type = N_PFTYPE_CLUT;
    this->bpp  = _bpp;
    this->pal = (nPalEntry *) n_malloc(pal_size);
    if (_pal) memcpy(this->pal,_pal,pal_size);
    this->r_mask = this->g_mask = this->b_mask = this->a_mask = 0;
    this->r_shift = this->g_shift = this->b_mask = this->a_mask = 0;
}

//-------------------------------------------------------------------
//  nPixelFormat(bpp,r_mask,g_mask,b_mask,a_mask)
//  19-Nov-98   floh    created
//  17-Dec-99   floh    updated
//-------------------------------------------------------------------
nPixelFormat::nPixelFormat(int _bpp,
                           unsigned int _r_mask,
                           unsigned int _g_mask,
                           unsigned int _b_mask,
                           unsigned int _a_mask)
{
    n_assert((_bpp==16)||(_bpp==24)||(_bpp==32));
    unsigned int r,g,b,a;
    unsigned int m;

    this->type = N_PFTYPE_RGBA;
    this->bpp  = _bpp;
    this->pal  = NULL;
    this->r_mask = _r_mask;
    this->g_mask = _g_mask;
    this->b_mask = _b_mask;
    this->a_mask = _a_mask;

    // compute shift values to shift normalized 8 bit components
    // to the correct position in the pixel format
    if (this->r_mask) {
        for (r=0, m=_r_mask; !(m&1); r++,m>>=1);
        this->r_shift = r;
        for (r=0; m&1; r++,m>>=1);
        this->r_shift -= (8-r);
    } else this->r_shift = 0;
    if (this->g_mask) {
        for (g=0, m=_g_mask; !(m&1); g++,m>>=1);
        this->g_shift = g;
        for (g=0; m&1; g++,m>>=1);
        this->g_shift -= (8-g);
    } else this->g_shift = 0;
    if (this->b_mask) {
        for (b=0, m=_b_mask; !(m&1); b++,m>>=1);
        this->b_shift = b;
        for (b=0; m&1; b++,m>>=1);
        this->b_shift -= (8-b);
    } else this->b_shift = 0;
    if (this->a_mask) {
        for (a=0, m=_a_mask; !(m&1); a++,m>>=1);
        this->a_shift = a;
        for (a=0; m&1; a++,m>>=1);
        this->a_shift -= (8-a);
    } else this->a_shift = 0;
}

//-------------------------------------------------------------------
//  nPixelFormat(nPixelFormat *)
//  19-Nov-98   floh    created
//  17-Dec-99   floh    updated
//-------------------------------------------------------------------
nPixelFormat::nPixelFormat(nPixelFormat *pf)
{
    this->type = pf->type;
    this->bpp  = pf->bpp;
    if (pf->pal) {
        ulong pal_size = (1<<pf->bpp) * sizeof(nPalEntry);
        this->pal = (nPalEntry *) n_malloc(pal_size);
        memcpy(this->pal,pf->pal,pal_size);
    } else this->pal = NULL;
    this->r_mask = pf->r_mask;
    this->g_mask = pf->g_mask;
    this->b_mask = pf->b_mask;
    this->a_mask = pf->a_mask;
    this->r_shift = pf->r_shift;
    this->g_shift = pf->g_shift;
    this->b_shift = pf->b_shift;
    this->a_shift = pf->a_shift;
    this->pf_from = NULL;
}

//-------------------------------------------------------------------
//  ~nPixelFormat()
//  24-Nov-98   floh    created
//-------------------------------------------------------------------
nPixelFormat::~nPixelFormat()
{
    if (this->pal) n_free(this->pal);
}

//-------------------------------------------------------------------
//  BeginConv()
//  24-Nov-98   floh    created
//  17-Dec-99   floh    updated
//-------------------------------------------------------------------
bool nPixelFormat::BeginConv(nPixelFormat *from)
{
    this->pf_from = from;

    // check for illegal combinations...
    if ((from->type==N_PFTYPE_RGBA) && (this->type==N_PFTYPE_CLUT)) {
        n_message("nPixelFormat: illegal conversion, rgba->clut!");
        return false;
    }

    // copy palette, if necessary
    if ((this->type==N_PFTYPE_CLUT) && (from->type==N_PFTYPE_CLUT)) {
        memcpy(this->pal,from->pal,(1<<from->bpp)*sizeof(nPalEntry));
    }
    return true;
}

//-------------------------------------------------------------------
//  EndConv()
//  24-Nov-98   floh    created
//-------------------------------------------------------------------
void nPixelFormat::EndConv(void)
{ }

//-------------------------------------------------------------------
//  getPixel()
//  26-Nov-98   floh    created
//-------------------------------------------------------------------
static ulong getPixel(int bpp, uchar *src)
{
    ulong p = 0;
    switch(bpp) {
        case 16:
            p = (ulong) *(ushort *)src;
            break;
        case 24:
            {
                ulong p0 = (ulong) *src++;
                ulong p1 = (ulong) *src++;
                ulong p2 = (ulong) *src++;
                p = (p0<<16)|(p1<<8)|(p2);
            }
            break;
        case 32:
            p = *(ulong *)src;
            break;
    };
    return p;
}

//-------------------------------------------------------------------
//  putPixel()
//  26-Nov-98   floh    created
//-------------------------------------------------------------------
static void putPixel(int bpp, uchar *tar, ulong p)
{
    ulong p0,p1,p2;
    switch(bpp) {
        case 16:
            *(ushort *)tar = (ushort) p;
            break;
        case 24:
            p0 = p>>16 & 0xff;
            p1 = p>>8 & 0xff;
            p2 = p & 0xff;
            *tar++=(uchar)p0; *tar++=(uchar)p1; *tar=(uchar)p2;
            break;
        case 32:
            *(ulong *)tar = p;
            break;
    };
}

//-------------------------------------------------------------------
//  Conv()
//  24-Nov-98   floh    created
//  25-Nov-98   floh    + wenn Source keinen Alpha-Kanal hat, wird
//                        dieser jetzt automatisch auf voll
//                        undurchsichtig gesetzt.
//  17-Dec-99   floh    + updated
//-------------------------------------------------------------------
void nPixelFormat::Conv(uchar *from_buf, uchar *to_buf, int num_pixels)
{
    int i;
    int from_addr = 0;
    int to_addr   = 0;
    uchar *src,*tar;

    // Fallunterscheidung
    // RGBA->RGBA
    if ((this->pf_from->type==N_PFTYPE_RGBA) && (this->type==N_PFTYPE_RGBA)) {
        // special case: identical pixel formats
        if ((this->pf_from->bpp == this->bpp) &&
            (this->pf_from->r_mask == this->r_mask) &&
            (this->pf_from->g_mask == this->g_mask) &&
            (this->pf_from->b_mask == this->b_mask) &&
            (this->pf_from->a_mask == this->a_mask))
        {
            memcpy(to_buf,from_buf,num_pixels*(this->bpp/8));
        } else {
            int rs,gs,bs,as;
            rs = this->r_shift - this->pf_from->r_shift;
            bs = this->b_shift - this->pf_from->b_shift;
            gs = this->g_shift - this->pf_from->g_shift;
            as = this->a_shift - this->pf_from->a_shift;
            src = from_buf;
            tar = to_buf;
            for (i=0; i<num_pixels; i++) {
                unsigned int s,t,r,g,b,a;
                // read source rgba pixel
                s = getPixel(this->pf_from->bpp,src);
                // generate target pixel by masking and shifting
                r = s & this->pf_from->r_mask;
                g = s & this->pf_from->g_mask;
                b = s & this->pf_from->b_mask;
                if (!this->pf_from->a_mask) a = 0xffffffff;
                else                        a = s & this->pf_from->a_mask;
                if (rs >= 0) r<<=rs; else r>>=-rs;
                if (gs >= 0) g<<=gs; else g>>=-gs;
                if (bs >= 0) b<<=bs; else b>>=-bs;
                if (as >= 0) a<<=as; else a>>=-as;
                t  = r & this->r_mask;
                t |= g & this->g_mask;
                t |= b & this->b_mask;
                t |= a & this->a_mask;
                putPixel(this->bpp,tar,t);
                // pointers to next pixel
                from_addr += this->pf_from->bpp;
                to_addr   += this->bpp;
                src = from_buf + (from_addr/8);
                tar = to_buf   + (to_addr/8);
            }
        }

    // CLUT->RGBA
    } else if ((pf_from->type==N_PFTYPE_CLUT) && (type==N_PFTYPE_RGBA)) {
        int rs,gs,bs,as;
        rs = this->r_shift;
        bs = this->b_shift;
        gs = this->g_shift;
        as = this->a_shift;
        src = from_buf;
        tar = to_buf;
        for (i=0; i<num_pixels; i++) {
            unsigned int s,t,r,g,b,a;
            // read source pixel
            s = *src;
            // split to rgba format
            r = this->pf_from->pal[s].r;
            g = this->pf_from->pal[s].g;
            b = this->pf_from->pal[s].b;
            a = 0xffffffff; // CLUT generally has no alpha channel
            if (rs >= 0) r<<=rs; else r>>=-rs;
            if (gs >= 0) g<<=gs; else g>>=-gs;
            if (bs >= 0) b<<=bs; else b>>=-bs;
            if (as >= 0) a<<=as; else a>>=-as;
            t  = r & this->r_mask;
            t |= g & this->g_mask;
            t |= b & this->b_mask;
            t |= a & this->a_mask;
            putPixel(this->bpp,tar,t);
            to_addr += this->bpp;
            src++;
            tar = to_buf + (to_addr/8);
        }

    // CLUT->CLUT
    } else memcpy(to_buf,from_buf,num_pixels);
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
