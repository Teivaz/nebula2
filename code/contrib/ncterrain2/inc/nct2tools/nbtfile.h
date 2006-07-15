#ifndef N_BTFILE_H
#define N_BTFILE_H
//------------------------------------------------------------------------------
/**
    @class nBtFile
    @ingroup NCTerrain2Tools

    @brief A BT (binary terrain) file access class.

    The BT file data must fit into memory (why do BT files have such a
    strange data layout???)

    Information on the BT format can be found at
    http://www.vterrain.org/Implementation/Formats/BT.html

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class nBtFile
{
public:
    /// error codes
    enum Error
    {
        Success,
        FileNotFound,
        InvalidFileFormat,
        InvalidCoords,
    };
    
    // units used in bt file
    enum Unit
    {
        Degrees = 0,    // 1 degree is 111.2 km on the equator
        Meters,
        IntFeet,        // international feet (0.3045 meters)
        USFeet,         // U.S. feet (1200 / 3937 meters)
    };

    /// constructor
    nBtFile(nKernelServer* kernelServer);
    /// destructor
    ~nBtFile();
    /// open file and read header
    bool Open(const char* filename);
    /// close the file
    void Close();
    /// currently open?
    bool IsOpen() const;
    /// get the dimensions of the file in meters (xz is the horizontal plane)
    vector3 GetSize() const;
    /// get the size of one heixel
    const vector3& GetHeixelSize() const;
    /// get number of columns in file (east-west)
    int GetNumColumns() const;
    /// get number of rows in file (north-south)
    int GetNumRows() const;
    /// sample a height value at the given location (everything in meters)
    float SampleHeight(float x, float z);
    /// sample a normal at the given location (everything in meters)
    vector3 SampleNormal(float x, float z);
    /// return detailed error code
    Error GetError() const;
    /// return true if data is in float format
    bool IsFloatData() const;
    /// return data size
    int GetDataSize() const;
    /// get short data value at coordinate
    short GetDirectShortValueAt(int row, int col);
    /// get height at given row/column
    float GetHeight(int row, int col);

private:
    /// convert a coordinate in meter units to row/column units
    float ConvertCoord(float c) const;
    /// set the error code
    void SetError(Error code);
    /// update the data cache
    void UpdateCache(int col);

    // const values
    enum
    {
        DataOffset = 256,       // offset of data from beginning of file in bytes
    };

    nKernelServer* kernelServer;
    nFile* file;
    int columns;                // width (east-west) dimension of the file
    int rows;                   // height (north-south) dimension of the file
    short dataSize;             // bytes per elevation grid point
    bool isFloatData;           // true if data in file are floating values
    Unit units;                 // units of data in file
    Error error;                // current error code
    vector3 heixelSize;         // the size of one height element in meters
    int cacheCol0;              // left column in cache
    int cacheCol1;              // right column in cache
    void* cache;
    int numCacheUpdates;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBtFile::IsFloatData() const
{
    return this->isFloatData;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBtFile::GetDataSize() const
{
    return this->dataSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBtFile::IsOpen() const
{
    return (0 != this->file);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nBtFile::ConvertCoord(float c) const
{
    return c / this->heixelSize.x;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBtFile::GetNumColumns() const
{
    return this->columns;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBtFile::GetNumRows() const
{
    return this->rows;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nBtFile::GetHeixelSize() const
{
    return this->heixelSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3
nBtFile::GetSize() const
{
    return vector3(this->columns * this->heixelSize.x,
                   0.0f,                  
                   this->rows * this->heixelSize.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBtFile::SetError(Error code)
{
    this->error = code;
}

//------------------------------------------------------------------------------
/**
*/
inline
nBtFile::Error
nBtFile::GetError() const
{
    return this->error;
}

//------------------------------------------------------------------------------
/**
    Returns the height as a short value. Works only for files with short
    data format.
*/
inline
short
nBtFile::GetDirectShortValueAt(int row, int col)
{
    n_assert(2 == this->dataSize);

    row = n_iclamp(row, 0, this->rows - 1);
    col = n_iclamp(col, 0, this->columns - 1);

    // check for cache exception
    if ((col < this->cacheCol0) || (col >= this->cacheCol1))
    {
        this->UpdateCache(col);
    }
    col -= this->cacheCol0;
    short* ptr = (short*) this->cache;
    return ptr[row + col * this->rows];
}

//------------------------------------------------------------------------------
/**
    Get float height at a given row/column. Note that bt files are
    organized in row-first order. The returned value will include the
    vertical scale.
*/
inline
float
nBtFile::GetHeight(int row, int col)
{
    row = n_iclamp(row, 0, this->rows - 1);
    col = n_iclamp(col, 0, this->columns - 1);

    // check for cache exception
    if ((col < this->cacheCol0) || (col >= this->cacheCol1))
    {
        this->UpdateCache(col);
    }
    col -= this->cacheCol0;

    float h;
    if (this->isFloatData)
    {
        float* ptr = (float*) this->cache;
        h = ptr[row + col * this->rows] * this->heixelSize.y;
    }
    else
    {
        if (2 == this->dataSize)
        {
            short* ptr = (short*) this->cache;
            h = float(ptr[row + col * this->rows]) * this->heixelSize.y;
        }
        else
        {
            int* ptr = (int*) this->cache;
            h = float(ptr[row + col * this->rows]) * this->heixelSize.y;
        }
    }
    return h;
}

//------------------------------------------------------------------------------
/**
    Sample height at given location (location is given in meters).
*/
inline
float
nBtFile::SampleHeight(float x, float z)
{
    // convert location into row/column float values
    float cx = this->ConvertCoord(x);
    float cz = this->ConvertCoord(z);

    // get integer row/column index and float fracts
    int ix = n_fchop(cx);
    int iz = n_fchop(cz);
    float fx = cx - float(ix);
    float fz = cz - float(iz);

    // read 4 float heights for bilinear interpolation
    float h00 = this->GetHeight(ix, iz);
    float h01 = this->GetHeight(ix, iz + 1);
    float h10 = this->GetHeight(ix + 1, iz);
    float h11 = this->GetHeight(ix + 1, iz + 1);

    float h0 = h00 + (h01 - h00) * fx;
    float h1 = h10 + (h11 - h10) * fx;

    float h = h0 + (h1 - h0) * fz;
    return h;
}

//------------------------------------------------------------------------------
/**
    Sample a normal.
*/
inline
vector3
nBtFile::SampleNormal(float x, float z)
{
    float h00 = this->SampleHeight(x, z);
    float h01 = this->SampleHeight(x, z + this->heixelSize.z);
    float h10 = this->SampleHeight(x + this->heixelSize.x, z);
    float h11 = this->SampleHeight(x + this->heixelSize.x, z + this->heixelSize.z);

    float slopeX = (((h00 - h10) + (h01 - h11)) * 0.5f) / this->heixelSize.x;
    float slopeZ = (((h00 - h01) + (h10 - h11)) * 0.5f) / this->heixelSize.z;

    vector3 xVec(1.0f, -slopeX, 0.0f);
    vector3 zVec(0.0f, slopeZ, 1.0f);
    vector3 yVec = zVec * xVec;
    yVec.norm();
    return yVec;
}

//------------------------------------------------------------------------------
#endif    




