#ifndef N_BMPFILE_H
#define N_BMPFILE_H
//-------------------------------------------------------------------
/**
    @class nBmpFile
    @ingroup NMapContribModule
    @brief loader/saver class for BMP file format
*/
//-------------------------------------------------------------------
#include "map/nimagefile.h"

typedef unsigned long ulong;

//-------------------------------------------------------------------
struct nBitmapFileHeader {
    ushort type;
    ulong  size;
    ushort reserved1;
    ushort reserved2;
    ulong  offbits;
};

struct nBitmapInfoHeader {
    ulong size;
    long  width;
    long  height;
    short planes;
    short bitcount;
    ulong compression;
    ulong sizeimage;
    ulong xpelspermeter;
    ulong ypelspermeter;
    ulong clrused;
    ulong clrimportant;
};

//-------------------------------------------------------------------
class nBmpFile : public nImageFile {
    enum {
        N_BI_RGB       = 0,
        N_BI_RLE8      = 1,
        N_BI_RLE4      = 2,
        N_BI_BITFIELDS = 3,
    };
    int line_numbytes;
    uchar *line_buffer;
    int act_y;

public:
    nBmpFile();
    virtual ~nBmpFile();
    virtual bool Open(const char *, const char *mode);
    virtual void Close(void);
    virtual uchar *ReadLine(int&);
    virtual int WriteLine(uchar *);
private:
    void readBitmapFileHeader(FILE *, nBitmapFileHeader *);
    void readBitmapInfoHeader(FILE *, nBitmapInfoHeader *);
    void writeBitmapFileHeader(FILE *, nBitmapFileHeader *);
    void writeBitmapInfoHeader(FILE *, nBitmapInfoHeader *);
};
//-------------------------------------------------------------------
#endif
