//-------------------------------------------------------------------
//  nbmpfile.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "map/nbmpfile.h"

//-------------------------------------------------------------------
//  nBmpFile()
//  17-Dec-99   floh    created
//-------------------------------------------------------------------
nBmpFile::nBmpFile()
{
    this->line_numbytes = 0;
    this->line_buffer   = NULL;
    this->act_y         = 0;
}

//-------------------------------------------------------------------
//  ~nBmpFile()
//  17-Dec-99   floh    created
//-------------------------------------------------------------------
nBmpFile::~nBmpFile()
{
    n_assert(NULL == this->line_buffer);
}

//-------------------------------------------------------------------
//  readBitmapFileHeader()
//  27-Nov-99   floh    created
//-------------------------------------------------------------------
void nBmpFile::readBitmapFileHeader(FILE *fp, nBitmapFileHeader *bfh)
{
#define n_readelm(fp,x) fread(&(x),sizeof(x),1,fp)
    n_readelm(fp,bfh->type);
    n_readelm(fp,bfh->size);
    n_readelm(fp,bfh->reserved1);
    n_readelm(fp,bfh->reserved2);
    n_readelm(fp,bfh->offbits);
}

//-------------------------------------------------------------------
//  writeBitmapFileHeader()
//  17-Dec-99   floh    created
//-------------------------------------------------------------------
void nBmpFile::writeBitmapFileHeader(FILE *fp, nBitmapFileHeader *bfh)
{
#define n_writeelm(fp,x) fwrite(&(x),sizeof(x),1,fp)
    n_writeelm(fp,bfh->type);
    n_writeelm(fp,bfh->size);
    n_writeelm(fp,bfh->reserved1);
    n_writeelm(fp,bfh->reserved2);
    n_writeelm(fp,bfh->offbits);
}

//--------------------------------------------------------------------
//  readBitmapInfoHeader()
//  17-Dec-99   floh    created
//--------------------------------------------------------------------
void nBmpFile::readBitmapInfoHeader(FILE *fp, nBitmapInfoHeader *bih)
{
    n_readelm(fp,bih->size);
    n_readelm(fp,bih->width);
    n_readelm(fp,bih->height);
    n_readelm(fp,bih->planes);
    n_readelm(fp,bih->bitcount);
    n_readelm(fp,bih->compression);
    n_readelm(fp,bih->sizeimage);
    n_readelm(fp,bih->xpelspermeter);
    n_readelm(fp,bih->ypelspermeter);
    n_readelm(fp,bih->clrused);
    n_readelm(fp,bih->clrimportant);
}

//--------------------------------------------------------------------
//  writeBitmapInfoHeader()
//  17-Dec-99   floh    created
//--------------------------------------------------------------------
void nBmpFile::writeBitmapInfoHeader(FILE *fp, nBitmapInfoHeader *bih)
{
    n_writeelm(fp,bih->size);
    n_writeelm(fp,bih->width);
    n_writeelm(fp,bih->height);
    n_writeelm(fp,bih->planes);
    n_writeelm(fp,bih->bitcount);
    n_writeelm(fp,bih->compression);
    n_writeelm(fp,bih->sizeimage);
    n_writeelm(fp,bih->xpelspermeter);
    n_writeelm(fp,bih->ypelspermeter);
    n_writeelm(fp,bih->clrused);
    n_writeelm(fp,bih->clrimportant);
}

//-------------------------------------------------------------------
//  Open()
//  17-Dec-99   floh    created
//  14-Nov-00   floh    lines not properly aligned in read mode
//-------------------------------------------------------------------
bool nBmpFile::Open(const char *fname, const char *mode)
{
    bool success = nImageFile::Open(fname,mode);
    if (success) {
        if (N_READ == this->acc_mode) {

            // *** READ MODE ***
            n_assert(NULL == this->line_buffer);

            uchar bmp_pal[256*4];
            struct nPalEntry pf_pal[256];
            struct nBitmapFileHeader bfh;
            struct nBitmapInfoHeader bih;
            this->act_y = 0;
            
            // read bitmap file header
            this->readBitmapFileHeader(this->fp,&bfh);
            this->readBitmapInfoHeader(this->fp,&bih);
            if (bih.clrused == 0) bih.clrused = (1<<bih.bitcount);

            // check for correct file type
            if (bfh.type != 0x4d42) {
                n_printf("nBmpFile::Open('%s'): not a BMP file!\n",fname);
                goto error;
            }
            int w = bih.width;
            int h = bih.height;
            if (h < 0) h = -h;
            this->SetWidth(w);
            this->SetHeight(h);

            if ((bih.bitcount!=8)&&(bih.bitcount!=16)&&(bih.bitcount!=24)&&(bih.bitcount!=32)) {
                n_printf("nBmpFile::Open('%s'): bitdepth not accepted!\n",fname);
                goto error;
            }

            switch(bih.compression) {
                case N_BI_RGB:
                    // nicht komprimiert
                    break;
                case N_BI_RLE8:
                case N_BI_RLE4:
                    n_printf("nBmpFile::Open('%s'): rle compression not supported!\n",fname);
                    goto error;
                    break;
                case N_BI_BITFIELDS:
                    n_printf("nBmpFile::Open('%s'): bitfields not supported!\n",fname);
                    goto error;
                    break;
            }

            // if necessary, load and convert color palette...
            if (bih.bitcount==8) {
                ulong i;
                uchar *ptr = bmp_pal;
                int pal_size = bih.clrused * 4;
                fread(&bmp_pal,pal_size,1,fp);
                for (i=0; i<bih.clrused; i++) {
                    pf_pal[i].b = *ptr++;
                    pf_pal[i].g = *ptr++;
                    pf_pal[i].r = *ptr++;
                    pf_pal[i].a = 255;
                    ptr++;
                }
                this->SetPixelFormat(8,pf_pal);
            } else if (bih.bitcount==24) {
                this->SetPixelFormat(bih.bitcount,
                                     0x000000ff,
                                     0x0000ff00,
                                     0x00ff0000,
                                     0x00000000);
            } else {
                n_printf("nBmpFile::Open('%s'): Error, not an 8 or 24 bpp format\n",fname);
                goto error;
            }

            // allocate read buffer (lines are 4 byte aligned)
            this->line_numbytes = ((bih.bitcount/8 * w)+3) & (~3);
            this->line_buffer = (uchar *) n_malloc(this->line_numbytes);
            return true;

        } else {
            
            // *** WRITE MODE ***
            int w = this->GetWidth();
            int h = this->GetHeight();
            n_assert(w > 0);
            n_assert(h > 0);
            n_assert(NULL == this->line_buffer);

            struct nBitmapFileHeader bfh;
            struct nBitmapInfoHeader bih;

            // compute number of bytes per line (must be 4 byte aligned)
            this->line_numbytes = (w*3+3) & (~3);

            // fill out bitmap file header
            bfh.type = 0x4d42;
            bfh.size = 0x36 + (this->line_numbytes * h);
            bfh.reserved1 = 0;
            bfh.reserved2 = 0;
            bfh.offbits   = 0x36;
                            

            // fill out the bitmap info header
            bih.size          = sizeof(nBitmapInfoHeader);
            bih.width         = w;
            bih.height        = h;
            bih.planes        = 1;
            bih.bitcount      = 24;
            bih.compression   = N_BI_RGB;
            bih.sizeimage     = 0;
            bih.xpelspermeter = 0;
            bih.ypelspermeter = 0;
            bih.clrused       = 0;
            bih.clrimportant  = 0;

            // write bitmap header and bitmap info to file
            this->writeBitmapFileHeader(this->fp,&bfh);
            this->writeBitmapInfoHeader(this->fp,&bih);

            // prepare other stuff for writing...
            this->SetPixelFormat(24,
                                 0x000000ff,
                                 0x0000ff00,
                                 0x00ff0000,
                                 0x00000000);
            this->act_y = 0;
            return true;
        }
    }

error:
    this->Close();
    return false;
}

//-------------------------------------------------------------------
//  Close()
//  17-Dec-99   floh    created
//-------------------------------------------------------------------
void nBmpFile::Close(void)
{
    if (this->line_buffer) {
        n_free(this->line_buffer);
        this->line_buffer = NULL;
    }
    nImageFile::Close();
}

//--------------------------------------------------------------------
//  ReadLine()
//  27-Nov-99   floh    created
//  17-Dec-99   floh    updated
//--------------------------------------------------------------------
uchar *nBmpFile::ReadLine(int& y)
{
    n_assert(this->fp);
    n_assert(this->pf);
    n_assert(this->line_buffer);
    if (this->act_y < this->height) {
        fread(this->line_buffer,1,this->line_numbytes,this->fp);
        y = this->act_y++;
        return this->line_buffer;
    } else {
        y = -1;
        return NULL;
    }
}

//--------------------------------------------------------------------
//  WriteLine()
//  17-Dec-99   floh    created
//--------------------------------------------------------------------
int nBmpFile::WriteLine(uchar *buf)
{
    n_assert(this->fp);
    n_assert(this->pf);
    n_assert(this->act_y < this->height);
    int len = 3*this->width;
    fwrite(buf,1,len,this->fp);

    // need to append pad cells???
    if (this->line_numbytes > len) {
        ulong null = 0;
        n_assert((this->line_numbytes-len)<(int)sizeof(null));
        fwrite(&null,1,this->line_numbytes-len,this->fp);
    }
    return this->act_y++;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
