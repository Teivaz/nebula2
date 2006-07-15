#ifndef N_GENTERRAINTEXTURE_H
#define N_GENTERRAINTEXTURE_H
//------------------------------------------------------------------------------
/**
    @class nGenTerrainTexture
    @ingroup NCTerrain2Tools

    @brief Generate a terrain texture controlled by an xml config file from
    an input BT file.

    (C) 2003 RadonLabs GmbH
*/    
#include "kernel/ntypes.h"
#include "nct2tools/ntgafile.h"
#include "nct2tools/nbtfile.h"
#include "mathlib/polar.h"
#include "tinyxml/tinyxml.h"
#include "mathlib/noise.h"
#include <stdarg.h>

//------------------------------------------------------------------------------
class nGenTerrainTexture
{
public:
    /// constructor
    nGenTerrainTexture(nKernelServer* kernelServer);
    /// destructor
    ~nGenTerrainTexture();
    /// set name of input bt file
    void SetBtFilename(const char* name);
    /// set name of output tga file
    void SetTexFilename(const char* name);
    /// set name of config file
    void SetConfigFilename(const char* name);
    /// set size of output texture
    void SetTexSize(int s);
    /// enable/disable texture weight mode (does not write colors but material weights)
    void SetEnableWeightMode(bool b);
    /// run the texture generation
    bool Run();
    /// get error string
    const char* GetError() const;

private:
    /// compute lighting for a given normal
    vector4 ComputeLight(const vector3& normal) const;
    /// compute a color for a given position and normal
    vector4 ComputeColor(const vector3& pos, const vector3& normal) const;
    /// compute possibility that a location is covered by snow (0.0 .. 1.0)
    float ComputeSnowPossibility(const vector3& pos, const vector3& normal) const;
    /// compute possibility that a location is covered by grass (0.0 .. 1.0)
    float ComputeGrassPossibility(const vector3& pos, const vector3& normal) const;
    /// compute possibility that a location is covered by moss
    float ComputeMossPossibility(const vector3& pos, const vector3& normal) const;
    /// set error string
    void __cdecl SetError(const char* err, ...);
    /// parse the config file name
    bool ParseConfigFile();
    /// get vector4 xml attribute
    vector4 GetVector4Attr(TiXmlElement* elm, const char* attr);
    /// get vector3 xml attribute
    vector3 GetVector3Attr(TiXmlElement* elm, const char* attr);
    /// get float xml attribute
    float GetFloatAttr(TiXmlElement* elm, const char* attr);
    /// get string xml attribute
    const char* GetStringAttr(TiXmlElement* elm, const char* attr);
    /// get int xml attribute
    int GetIntAttr(TiXmlElement* elm, const char* attr);

    /// materials
    enum
    {
        Snow = 0,
        Grass,
        Moss,
        Rock,

        NumMaterials,
    };

    /// a structure describing surface materials
    struct MaterialDesc
    {
        float minSlope;
        float maxSlope;
        float minHeight;
        float maxHeight;
        float heightRange;
        vector4 color;
    };

    // a structure describing the sunlight
    struct LightDesc
    {
        polar2 polarCoords;
        vector3 dir;
        vector4 diffuseColor;
        vector4 ambientColor;
    };

    nBtFile btFile;
    nTgaFile texFile;
    nString btFilename;
    nString texFilename;
    nString configFilename;
    nString error;
    int texSize;
    bool weightMode;
    noise noiseGenerator;

    LightDesc light;
    nFixedArray<MaterialDesc> material;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGenTerrainTexture::SetEnableWeightMode(bool b)
{
    this->weightMode = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGenTerrainTexture::SetBtFilename(const char* name)
{
    this->btFilename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGenTerrainTexture::SetTexFilename(const char* name)
{
    this->texFilename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGenTerrainTexture::SetConfigFilename(const char* name)
{
    this->configFilename = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGenTerrainTexture::SetTexSize(int s)
{
    this->texSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
void __cdecl
nGenTerrainTexture::SetError(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), msg, argList);
    va_end(argList);
    this->error = buf;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGenTerrainTexture::GetError() const
{
    return this->error.Get();
}

//------------------------------------------------------------------------------
/**
    Compute lighting for a given normal.
*/
inline
vector4
nGenTerrainTexture::ComputeLight(const vector3& normal) const
{
    // light is ambient + N.L
    float lightIntensity = n_saturate(normal % this->light.dir);
    vector4 c = this->light.ambientColor + this->light.diffuseColor * lightIntensity;
    c.x = n_saturate(c.x);
    c.y = n_saturate(c.y);
    c.z = n_saturate(c.z);
    c.w = n_saturate(c.w);
    return c;
}

//------------------------------------------------------------------------------
/**
    Computes the possibility that a location is covered by snow. Takes
    height, slope and "southness" into account.
*/
inline
float
nGenTerrainTexture::ComputeSnowPossibility(const vector3& pos, const vector3& normal) const
{
    static const vector3 southVector(0.0f, 0.0f, 1.0f);

    // slope
    float w = 1.0f;
    if (normal.y > this->material[Snow].minSlope)
    {
        w = 0.0f;
        return w;
    }

    // "northness" and "southness"
    float southness = normal % southVector;
    float heightRange = this->material[Snow].heightRange;
    float minHeight = this->material[Snow].minHeight + heightRange * southness;
    float maxHeight = this->material[Snow].maxHeight + heightRange * southness;
    if ((pos.y < minHeight) || (pos.y > maxHeight))
    {
        w = 0.0f;
    }
    return w;
}

//------------------------------------------------------------------------------
/**
    Compute grass possibility.
*/
inline
float
nGenTerrainTexture::ComputeGrassPossibility(const vector3& pos, const vector3& normal) const
{
    const vector3 northVector(0.0f, 0.0f, -1.0f);

    // "northness"
    float northness = normal % northVector;
    const float minHeight = this->material[Grass].minHeight + this->material[Grass].heightRange * northness;
    const float maxHeight = this->material[Grass].maxHeight + this->material[Grass].heightRange * northness;
    float w = 0;
    if ((pos.y > maxHeight) || (pos.y < minHeight))
    {
        w = 0.0f;
    }
    else
    {
        w = n_saturate((pos.y - maxHeight) / (minHeight - maxHeight));
    }

    // slope
    w *= (normal.y * normal.y);
/*
    float n0 = this->noiseGenerator.gen(pos.x * 0.00125f, pos.z * 0.00025f, 0.0f);
    //float n1 = (this->noiseGenerator.gen(pos.x * 0.005f, pos.z * 0.001f, 0.12f) + 1.0f);
    w *= n0;
*/
    return w;
}

//------------------------------------------------------------------------------
/**
    Compute moss possibility.
*/
inline
float
nGenTerrainTexture::ComputeMossPossibility(const vector3& pos, const vector3& normal) const
{
    const vector3 northVector(0.0f, 0.0f, -1.0f);

    // "northness"
    float northness = normal % northVector;
    const float minHeight = this->material[Moss].minHeight + this->material[Moss].heightRange * northness;
    const float maxHeight = this->material[Moss].maxHeight + this->material[Moss].heightRange * northness;
    float w = 0;
    if ((pos.y > maxHeight) || (pos.y < minHeight))
    {
        w = 0.0f;
    }
    else
    {
        w = n_saturate((pos.y - maxHeight) / (minHeight - maxHeight));
    }

    // slope
    w *= n_saturate(normal.y * 5.0f);

/*
    float minSlope = this->material[Moss].minSlope;
    float maxSlope = this->material[Moss].maxSlope;
    float slopeFactor = n_saturate((normal.y - minSlope) / (maxSlope - minSlope));
    w *= slopeFactor;
*/

/*
    float n0 = (this->noiseGenerator.gen(pos.x * 0.005f, pos.z * 0.005f, 0.2f) + 1.0f) * 0.5f;
    float n1 = (this->noiseGenerator.gen(pos.x * 0.001f, pos.z * 0.001f, 0.3f) + 1.0f) * 0.25f;
    w *= n1 + n0;
*/

    return w;
}

//------------------------------------------------------------------------------
/**
    Compute the ground color of a pixel.
*/
inline
vector4
nGenTerrainTexture::ComputeColor(const vector3& pos, const vector3& normal) const
{
    float snowWeight  = this->ComputeSnowPossibility(pos, normal);
    static vector4 color;
    if (snowWeight > 0.0f)
    {
        if (this->weightMode)
        {
            color.x = 0.0f;
            color.y = 0.0f;
            color.z = 0.0f;
            color.w = snowWeight;
        }
        else
        {
            color = this->material[Snow].color;
        }
    }
    else
    {
        float grassWeight = this->ComputeGrassPossibility(pos, normal);
        float mossWeight  = this->ComputeMossPossibility(pos, normal);
        mossWeight = n_saturate(mossWeight - grassWeight);
        float rockWeight  = n_saturate(1.0f - grassWeight - mossWeight);
        /*
        float il = 1.0f / (mossWeight + grassWeight + rockWeight);
        mossWeight *= il;
        grassWeight *= il;
        rockWeight *= il;
        */
        if (this->weightMode)
        {
            color.x = n_saturate(grassWeight);
            color.y = n_saturate(rockWeight);
            color.z = n_saturate(mossWeight);
            color.w = 0.0f;
        }
        else
        {
            // normalize rock, ground and grass weights
            color = this->material[Grass].color * n_saturate(grassWeight) +
                    this->material[Moss].color * n_saturate(mossWeight) +
                    this->material[Rock].color * n_saturate(rockWeight);
        }
    }
    return color;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nGenTerrainTexture::GetVector4Attr(TiXmlElement* elm, const char* attr)
{
    n_assert(elm && attr);
    vector4 v;
    nString val = elm->Attribute(attr);
    n_assert2(!val.IsEmpty(), attr);
    nArray<nString> tokens;
    val.Tokenize(N_WHITESPACE, tokens);
    n_assert(tokens.Size() == 4);
    v.x = float(atof(tokens[0].Get()));
    v.y = float(atof(tokens[1].Get()));
    v.z = float(atof(tokens[2].Get()));
    v.w = float(atof(tokens[3].Get()));
    return v;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3
nGenTerrainTexture::GetVector3Attr(TiXmlElement* elm, const char* attr)
{
    n_assert(elm && attr);
    vector3 v;
    nString val = elm->Attribute(attr);
    n_assert2(!val.IsEmpty(), attr);
    nArray<nString> tokens;
    val.Tokenize(N_WHITESPACE, tokens);
    n_assert(tokens.Size() == 3);
    v.x = float(atof(tokens[0].Get()));
    v.y = float(atof(tokens[1].Get()));
    v.z = float(atof(tokens[2].Get()));
    return v;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGenTerrainTexture::GetFloatAttr(TiXmlElement* elm, const char* attr)
{
    n_assert(elm && attr);
    const char* val = elm->Attribute(attr);
    n_assert2(val, attr);
    return float(atof(val));
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGenTerrainTexture::GetStringAttr(TiXmlElement* elm, const char* attr)
{
    n_assert(elm && attr);
    const char* val = elm->Attribute(attr);
    return val;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGenTerrainTexture::GetIntAttr(TiXmlElement* elm, const char* attr)
{
    n_assert(elm && attr);
    const char* val = elm->Attribute(attr);
    n_assert2(val, attr);
    return atoi(val);
}

//------------------------------------------------------------------------------
#endif

