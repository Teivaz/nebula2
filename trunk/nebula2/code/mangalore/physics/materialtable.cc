//------------------------------------------------------------------------------
//  physics/materialtable.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "physics/materialtable.h"
#include "xml/nxmlspreadsheet.h"

namespace Physics
{
int MaterialTable::materialCount;
nArray<struct MaterialTable::material> MaterialTable::materials;
nArray<struct MaterialTable::interaction> MaterialTable::interactions;

//------------------------------------------------------------------------------
/**
    Materials used in ODE

    Unit defintions are:
    Density:                t/m^3
    Force (MegaNewton):     t*m/s^2
    Momentum:               t*m/s
    Moment of intertia:     t*m^2
    Moment of momentum:     t*m^2/s
    Moment of force:        N*m
    Pressure (MegaPascal):  MN/m^2
    Weight (MegaNewton):    t*m/s^2
    Work (MegaJoule):       MN*m
    Energy (MegaJoule):     MN*m

    Note: I'm not really sure that this is right... ;-)
*/
void
MaterialTable::Setup()
{
    nXmlSpreadSheet materialDb;
    materialDb.SetFilename("data:tables/materials.xml");
    materialDb.Open();
    n_assert(materialDb.IsOpen());

    /* load tables */
    nXmlTable* matTable = materialDb.FindTable("materials");
    n_assert(0 != matTable);

    materialCount = matTable->NumRows() - 2;

    /* allocate memory */
    materials.SetFixedSize(materialCount);
    interactions.SetFixedSize(materialCount * materialCount);

    /* load materials and material properties */
    for (int row = 2; row < matTable->NumRows(); ++row)
    {
        struct material& material = materials[row - 2];
        material.name = matTable->Cell(row, "Name").AsString();
        material.density = matTable->Cell(row, "Density").AsFloat();
    }

    /* load friction data */
    nXmlTable* frictionTable = materialDb.FindTable("friction");
    n_assert(0 != frictionTable);
    n_assert(frictionTable->NumRows() == materialCount + 2);
    n_assert(frictionTable->NumColumns() == materialCount + 1);
    for (int row = 2; row < frictionTable->NumRows(); ++row)
    {
        MaterialType mat1 = StringToMaterialType(frictionTable->Cell(row, 0).AsString());

        for (int col = row - 2 + 1; col < frictionTable->NumColumns(); ++col)
        {
            MaterialType mat2 = StringToMaterialType(frictionTable->Cell(0, col).AsString());

            interactions[mat1 * materialCount + mat2].friction = frictionTable->Cell(row, col).AsFloat();
            interactions[mat2 * materialCount + mat1].friction = frictionTable->Cell(row, col).AsFloat();
        }
    }

    /* load bounce data */
    nXmlTable* bounceTable = materialDb.FindTable("bouncyness");
    n_assert(0 != frictionTable);
    n_assert(bounceTable->NumRows() == materialCount + 2);
    n_assert(bounceTable->NumColumns() == materialCount + 1);
    for (int row = 2; row < bounceTable->NumRows(); ++row)
    {
        MaterialType mat1 = StringToMaterialType(bounceTable->Cell(row, 0).AsString());

        for (int col = row - 2 + 1; col < bounceTable->NumColumns(); ++col)
        {
            MaterialType mat2 = StringToMaterialType(bounceTable->Cell(0, col).AsString());

            interactions[mat1 * materialCount + mat2].bouncyness = bounceTable->Cell(row, col).AsFloat();
            interactions[mat2 * materialCount + mat1].bouncyness = bounceTable->Cell(row, col).AsFloat();
        }
    }

    /* load sound data */
    nXmlTable* soundTable = materialDb.FindTable("sound");
    if (0 != soundTable)
    {
        n_assert(soundTable->NumRows() == materialCount + 2);
        n_assert(soundTable->NumColumns() == materialCount + 1);
        for (int row = 2; row < soundTable->NumRows(); ++row)
        {
            MaterialType mat1 = StringToMaterialType(soundTable->Cell(row, 0).AsString());

            for (int col = row - 2 + 1; col < soundTable->NumColumns(); ++col)
            {
                MaterialType mat2 = StringToMaterialType(soundTable->Cell(0, col).AsString());

                if (soundTable->Cell(row, col).AsString() != "")
                {
                    interactions[mat1 * materialCount + mat2].collSound = soundTable->Cell(row, col).AsString();
                    interactions[mat2 * materialCount + mat1].collSound = soundTable->Cell(row, col).AsString();
                }
                else
                {
                    interactions[mat1 * materialCount + mat2].collSound.Clear();
                    interactions[mat2 * materialCount + mat1].collSound.Clear();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
nString
MaterialTable::MaterialTypeToString(MaterialType t)
{
    if (-1 == t)
    {
        return "InvalidMaterial";
    }
    else
    {
        n_assert(t >= 0 && t < materialCount);
        return materials[t].name;
    }
}

//------------------------------------------------------------------------------
/**
*/
MaterialType
MaterialTable::StringToMaterialType(const nString& str)
{
    for (int i = 0; i < materialCount; ++i)
    {
        if (materials[i].name == str)
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
*/
float
MaterialTable::GetDensity(MaterialType t)
{
    n_assert(t >= 0 && t < materialCount);
    return materials[t].density;
}

//------------------------------------------------------------------------------
/**
*/
float
MaterialTable::GetFriction(MaterialType t0, MaterialType t1)
{
    n_assert(t0 >= 0 && t0 < materialCount);
    n_assert(t1 >= 0 && t1 < materialCount);
    return interactions[t0 * materialCount + t1].friction;
}

//------------------------------------------------------------------------------
/**
*/
float
MaterialTable::GetBounce(MaterialType t0, MaterialType t1)
{
    n_assert(t0 >= 0 && t0 < materialCount);
    n_assert(t1 >= 0 && t1 < materialCount);
    return interactions[t0 * materialCount + t1].bouncyness;
}

//------------------------------------------------------------------------------
/**
*/
const nString&
MaterialTable::GetCollisionSound(MaterialType t0, MaterialType t1)
{
    n_assert(t0 >= 0 && t0 < materialCount);
    n_assert(t1 >= 0 && t1 < materialCount);
    return interactions[t0 * materialCount + t1].collSound;
}

} // namespace Physics
