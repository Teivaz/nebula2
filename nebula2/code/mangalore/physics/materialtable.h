#ifndef PHYSICS_MATERIALTABLE_H
#define PHYSICS_MATERIALTABLE_H
//------------------------------------------------------------------------------
/**
    @class Physics::MaterialTable

    Contains material properties and friction coefficients.

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
namespace Physics
{
typedef int MaterialType;
const MaterialType InvalidMaterial = -1;

class MaterialTable
{
public:
    /// initialize material table
    static void Setup();

    /// translate material type to string
    static nString MaterialTypeToString(MaterialType t);
    /// translate string to material type
    static MaterialType StringToMaterialType(const nString& str);

    /// get density for a material type
    static float GetDensity(MaterialType t);
    /// get friction coefficient for 2 materials
    static float GetFriction(MaterialType t0, MaterialType t1);
    /// get bounce for 2 materials
    static float GetBounce(MaterialType t0, MaterialType t1);
    /// get collision sound for two material
    static const nString& GetCollisionSound(MaterialType t0, MaterialType t1);

private:
    /// constructor
    MaterialTable();
    /// destructor
    ~MaterialTable();

    struct material {
        material() : name(), density(0.0f) {}
        nString name;
        float density;
    };
    struct interaction {
        interaction() : friction(0.0f), bouncyness(0.0f), collSound() {}
        float friction;
        float bouncyness;
        nString collSound;
    };

    static int materialCount;
    static nArray<struct material> materials;
    static nArray<struct interaction> interactions;
};

} // namespace Physics
//------------------------------------------------------------------------------
#endif
