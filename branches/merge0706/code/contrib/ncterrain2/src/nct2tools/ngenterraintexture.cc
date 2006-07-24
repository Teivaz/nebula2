//------------------------------------------------------------------------------
//  ngenterraintexture.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nct2tools/ngenterraintexture.h"
#include "tinyxml/tinyxml.h"

//------------------------------------------------------------------------------
/**
*/
nGenTerrainTexture::nGenTerrainTexture(nKernelServer* ks) :
    btFile(ks),
    texFile(ks),
    btFilename("terrain.nt"),
    texFilename("texture.tga"),
    configFilename("terrain.cfg"),
    texSize(512),
    weightMode(false),
    material(NumMaterials)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGenTerrainTexture::~nGenTerrainTexture()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Run the texture generation. Reads the bt file and writes to tga file.
*/
bool
nGenTerrainTexture::Run()
{
    // parse config file
    if (!this->ParseConfigFile())
    {
        this->SetError("Could not parse config file '%s'!\n", this->configFilename.Get());
        return false;
    }

    // open input and output files
    if (!this->btFile.Open(this->btFilename.Get()))
    {
        this->SetError("Could not open bt file '%s' for reading!\n", this->btFilename.Get());
        return false;
    }
    this->texFile.SetWidth(this->texSize);
    this->texFile.SetHeight(this->texSize);
    if (!this->texFile.OpenWrite(this->texFilename.Get()))
    {
        this->SetError("Could not open tga file '%s' for writing!\n", this->texFilename.Get());
        return false;
    }

    // allocate image data block, write image in chunks of 256 columns
    const int imgCacheColumns = 256;
    uint* imgData = n_new_array(uint, imgCacheColumns * this->texSize);

    // per-pixel loop
    vector3 btSize = btFile.GetSize();
    vector3 pos, normal;
    float dx = btSize.x / float(this->texSize);
    float dz = btSize.z / float(this->texSize);
    int iz;
    float fz = 0.0f;
    int texColumns = 0;
    for (iz = 0; iz < this->texSize; iz++, fz+=dz, texColumns++)
    {
        int ix;
        float fx = 0.0f;
        for (ix = 0; ix < this->texSize; ix++, fx+=dx)
        {
            // sample a normal and a height
            pos.set(fx, this->btFile.SampleHeight(fx, fz), fz);
            normal = this->btFile.SampleNormal(fx, fz);
            vector4 light = this->ComputeLight(normal);
            vector4 color = this->ComputeColor(pos, normal);
            int r = int(light.x * color.x * 255.0f);
            int g = int(light.y * color.y * 255.0f);
            int b = int(light.z * color.z * 255.0f);
            int a = int(light.w * color.w * 255.0f);
            uint argb = N_ARGB(a, r, g, b);
            if (texColumns >= imgCacheColumns)
            {
                // write image data to tga file
                this->texFile.WriteChunk(iz - texColumns, 0, texColumns, this->texSize, (char*) imgData);
                texColumns = 0;
            }
            // write data upside down
            imgData[((texSize - 1 - ix) * imgCacheColumns) + texColumns] = argb;
        }
    }

    // write final chunk of data
    this->texFile.WriteChunk(iz - texColumns, 0, texColumns, this->texSize, (char*) imgData);

    // close and exit
    n_delete_array(imgData);
    this->texFile.Close();
    this->btFile.Close();

    return true;
}

//------------------------------------------------------------------------------
/**
    This parses the config xml file.
*/
bool
nGenTerrainTexture::ParseConfigFile()
{
    n_assert(!this->configFilename.IsEmpty());
    nString mangledPath = nFileServer2::Instance()->ManglePath(this->configFilename.Get());

    TiXmlDocument doc(mangledPath.Get());
    if (!doc.LoadFile())
    {
        n_printf("Failed to load '%s' as XML file!\n", this->configFilename.Get());
        return false;
    }

    TiXmlHandle docHandle(&doc);

    // configure globals
    TiXmlElement* elm = docHandle.FirstChild("TerrainConfig").Element();
    n_assert(elm);
    this->SetBtFilename(this->GetStringAttr(elm, "btFile"));
    this->SetTexFilename(this->GetStringAttr(elm, "tgaFile"));
    this->SetTexSize(this->GetIntAttr(elm, "size"));
    this->SetEnableWeightMode(this->GetIntAttr(elm, "weightMode") != 0);

    // configure light source
    elm = docHandle.FirstChild("TerrainConfig").FirstChild("Light").Element();
    n_assert(elm);
    this->light.polarCoords.theta = n_deg2rad(this->GetFloatAttr(elm, "altitudeDegree"));
    this->light.polarCoords.rho   = n_deg2rad(this->GetFloatAttr(elm, "azimutDegree"));
    this->light.dir = this->light.polarCoords.get_cartesian();
    this->light.diffuseColor = this->GetVector4Attr(elm, "diffuse");
    this->light.ambientColor = this->GetVector4Attr(elm, "ambient");

    // configure Snow material
    elm = docHandle.FirstChild("TerrainConfig").FirstChild("Snow").Element();
    n_assert(elm);
    this->material[Snow].minSlope     = n_sin(n_deg2rad(this->GetFloatAttr(elm, "slopeDegree")));
    this->material[Snow].maxSlope     = this->material[Snow].minSlope;
    this->material[Snow].minHeight    = this->GetFloatAttr(elm, "minHeight");
    this->material[Snow].maxHeight    = this->GetFloatAttr(elm, "maxHeight");
    this->material[Snow].heightRange  = this->GetFloatAttr(elm, "heightRange");
    this->material[Snow].color        = this->GetVector4Attr(elm, "color");

    // configure Grass material
    elm = docHandle.FirstChild("TerrainConfig").FirstChild("Grass").Element();
    n_assert(elm);
    this->material[Grass].minSlope     = n_sin(n_deg2rad(this->GetFloatAttr(elm, "slopeDegree0")));
    this->material[Grass].maxSlope     = n_sin(n_deg2rad(this->GetFloatAttr(elm, "slopeDegree1")));
    this->material[Grass].minHeight    = this->GetFloatAttr(elm, "minHeight");
    this->material[Grass].maxHeight    = this->GetFloatAttr(elm, "maxHeight");
    this->material[Grass].heightRange  = this->GetFloatAttr(elm, "heightRange");
    this->material[Grass].color        = this->GetVector4Attr(elm, "color");

    // configure Moss material
    elm = docHandle.FirstChild("TerrainConfig").FirstChild("Moss").Element();
    n_assert(elm);
    this->material[Moss].minSlope     = n_sin(n_deg2rad(this->GetFloatAttr(elm, "slopeDegree0")));
    this->material[Moss].maxSlope     = n_sin(n_deg2rad(this->GetFloatAttr(elm, "slopeDegree1")));
    this->material[Moss].minHeight    = this->GetFloatAttr(elm, "minHeight");
    this->material[Moss].maxHeight    = this->GetFloatAttr(elm, "maxHeight");
    this->material[Moss].heightRange  = this->GetFloatAttr(elm, "heightRange");
    this->material[Moss].color        = this->GetVector4Attr(elm, "color");

    // configure Rock material
    elm = docHandle.FirstChild("TerrainConfig").FirstChild("Rock").Element();
    n_assert(elm);
    this->material[Rock].minSlope     = 0.0f;
    this->material[Rock].maxSlope     = 0.0f;
    this->material[Rock].minHeight    = this->GetFloatAttr(elm, "minHeight");
    this->material[Rock].maxHeight    = this->GetFloatAttr(elm, "maxHeight");
    this->material[Rock].heightRange  = this->GetFloatAttr(elm, "heightRange");
    this->material[Rock].color = this->GetVector4Attr(elm, "color");

    return true;
}