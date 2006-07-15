//------------------------------------------------------------------------------
//  nbtfile.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/nbtfile.h"

//------------------------------------------------------------------------------
/**
*/
nBtFile::nBtFile(nKernelServer* ks) :
    kernelServer(ks),
    file(0),
    columns(0),
    rows(0),
    dataSize(0),
    isFloatData(false),
    units(Meters),
    error(Success),
    cacheCol0(0),
    cacheCol1(1),
    cache(0),
    numCacheUpdates(0)
{
    n_assert(this->kernelServer);
}

//------------------------------------------------------------------------------
/**
*/
nBtFile::~nBtFile()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
    Open the given bt file and read header. Fills error code and returns false
    if something goes wrong.
*/
bool
nBtFile::Open(const char* filename)
{
    n_assert(!this->IsOpen());
    n_assert(filename);
    n_assert(0 == this->cache);

    // open bt file
    this->file = kernelServer->GetFileServer()->NewFileObject();
    if (!this->file->Open(filename, "rb"))
    {
        this->SetError(FileNotFound);
        this->file->Release();
        this->file = 0;
        return false;
    }

    // read header, just make sure it's a bt file, don't care about the version
    char magic[8] = { 0 };
    this->file->Read(magic, 7);
    if (strcmp("binterr", magic) != 0)
    {
        this->SetError(InvalidFileFormat);
        this->file->Release();
        this->file = 0;
        return false;
    }
    this->file->Seek(10, nFile::START);

    // read bt attributes
    this->columns     = this->file->GetInt();
    this->rows        = this->file->GetInt();
    this->dataSize    = this->file->GetShort();
    this->isFloatData = (1 == this->file->GetShort());
    short unitsFlag   = this->file->GetShort();
    switch (unitsFlag)
    {
        case 0:     this->units = Degrees; break;
        case 1:     this->units = Meters; break;
        case 2:     this->units = IntFeet; break;
        case 3:     this->units = USFeet; break;
        default:    this->units = Meters; break;
    }

    // skip UTM zone and datum
    this->file->Seek(2 * sizeof(short), nFile::CURRENT);

    // read extents
    double leftExtent   = this->file->GetDouble();
    double rightExtent  = this->file->GetDouble();
    double bottomExtent = this->file->GetDouble();
    double topExtent    = this->file->GetDouble();

    // skip external projection flag
    this->file->Seek(sizeof(short), nFile::CURRENT);

    // read vertical scale in meters
    this->heixelSize.y = this->file->GetFloat();
    if (0 == this->heixelSize.y)
    {
        this->heixelSize.y = 1.0f;
    }

    // compute horizontal size of one heixel in meters, extents are always in degree
    this->heixelSize.x = n_abs(float(leftExtent - rightExtent) / this->columns);   // ????
    this->heixelSize.z = n_abs(float(topExtent - bottomExtent) / this->rows);

    // update the cache
    this->numCacheUpdates = 0;
    this->UpdateCache(0);

    return true;
}

//------------------------------------------------------------------------------
/**
    Close bt file and cleanup everything.
*/
void
nBtFile::Close()
{
    if (this->cache)
    {
        n_free(this->cache);
        this->cache = 0;
    }
    if (this->file)
    {
        this->file->Release();
        this->file = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Update the cache so that the requested column is in memory.
*/
void
nBtFile::UpdateCache(int col)
{
    n_assert(this->file);

n_printf("nBtFile: UpdateCache(%d)\n", col);

    this->numCacheUpdates++;
    if (0 == this->cache)
    {
        int cacheSize = 256 * this->rows * this->dataSize;
        this->cache = n_malloc(cacheSize);
    }

    // read 256 columns
    col = col - 4;
    if (col < 0) col = 0;
    this->cacheCol0 = n_iclamp(col, 0, this->columns - 1);
    this->cacheCol1 = n_iclamp(col + 256, 0, this->columns);
    int bytesToRead = (this->cacheCol1 - this->cacheCol0) * this->rows * this->dataSize;

    int filePos = DataOffset + this->rows * col * this->dataSize;
    this->file->Seek(filePos, nFile::START);
    int bytesRead = this->file->Read(this->cache, bytesToRead);
    n_assert(bytesRead == bytesToRead);
}




