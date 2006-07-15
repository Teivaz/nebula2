//-------------------------------------------------------------------
//  nimagefile.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include <string.h>
#include "map/nimagefile.h"

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
nImageFile::nImageFile()
{
    this->fp       = NULL;
    this->width    = 0;
    this->height   = 0;
    this->pf       = NULL;
    this->acc_mode = N_NONE;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
nImageFile::~nImageFile()
{
    n_assert(NULL == this->fp);
    n_assert(NULL == this->pf);
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
bool nImageFile::Open(const char *fname, const char *mode)
{
    n_assert(fname);
    n_assert(mode);
    n_assert(NULL == this->fp);
    this->fp = fopen(fname,mode);
    if (this->fp) {
        if (strchr(mode,'w')) this->acc_mode = N_WRITE;
        else                  this->acc_mode = N_READ;
        return true;
    } else {
        this->acc_mode = N_NONE;
        n_printf("nImageFile::Open(): could not open '%s' with mode '%s'\n",fname,mode);
        return false;
    }
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nImageFile::Close(void)
{
    if (this->pf) {
        n_delete(this->pf);
        this->pf = NULL;
    }
    if (this->fp) {
        fclose(this->fp);
        this->fp = NULL;
    }
    this->acc_mode = N_NONE;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
bool nImageFile::IsOpen(void)
{
    return (this->acc_mode == N_NONE) ? false : true;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nImageFile::SetWidth(int w)
{
    this->width = w;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
int nImageFile::GetWidth(void)
{
    return this->width;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nImageFile::SetHeight(int h)
{
    this->height = h;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
int nImageFile::GetHeight(void)
{
    return this->height;
}

//-------------------------------------------------------------------
/**
   - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nImageFile::SetPixelFormat(int bpp, nPalEntry *pal)
{
    n_assert(NULL == this->pf);
    this->pf = n_new(nPixelFormat(bpp, pal));
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
void nImageFile::SetPixelFormat(int bpp, int r_mask, int g_mask, int b_mask, int a_mask)
{
    n_assert(NULL == this->pf);
    this->pf = n_new(nPixelFormat(bpp, r_mask, g_mask, b_mask, a_mask));
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
nPixelFormat *nImageFile::GetPixelFormat(void)
{
    n_assert(this->pf);
    return this->pf;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
uchar *nImageFile::ReadLine(int&)
{
    n_error("nImageFile::ReadLine(): Pure virtual function called!\n");
    return NULL;
}

//-------------------------------------------------------------------
/**
    - 17-Dec-99   floh    created
*/
//-------------------------------------------------------------------
int nImageFile::WriteLine(uchar *)
{
    n_error("nImageFile::WriteLine(): Pure virtual function called!\n");
    return 0;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

