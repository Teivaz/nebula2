#ifndef N_LIGHT_H
#define N_LIGHT_H
//------------------------------------------------------------------------------
/**
    Describes a light source.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nLight
{
public:
    /// light types
    enum Type
    {
        Point,
        Directional,
        Spot,

        NumTypes,
        InvalidType,
    };

    /// constructor
    nLight();
    /// destructor
    ~nLight();
    /// set world space transform
    void SetTransform(const matrix44& m);
    /// get world space transform
    const matrix44& GetTransform() const;
    /// set light type
    void SetType(Type t);
    /// get light type
    Type GetType() const;
    /// set light's diffuse color
    void SetDiffuse(const vector4& c);
    /// get light's diffuse color
    const vector4& GetDiffuse() const;
    /// set light's specular color
    void SetSpecular(const vector4& c);
    /// get light's specular color
    const vector4& GetSpecular() const;
    /// set light's ambient color
    void SetAmbient(const vector4& c);
    /// get light's ambient color
    const vector4& GetAmbient() const;
    /// set secondary diffuse color
    void SetSecondaryDiffuse(const vector4& c);
    /// get secondary diffuse color
    const vector4& GetSecondaryDiffuse() const;
    /// set secondary specular color
    void SetSecondarySpecular(const vector4& c);
    /// get secondary specular color
    const vector4& GetSecondarySpecular() const;

private:
    matrix44 transform;
    Type type;
    vector4 diffuse;
    vector4 specular;
    vector4 ambient;
    vector4 secDiffuse;
    vector4 secSpecular;
};

//------------------------------------------------------------------------------
/**
*/
inline
nLight::nLight() :
    type(Point),
    diffuse(1.0f, 1.0f, 1.0f, 1.0f),
    specular(1.0f, 1.0f, 1.0f, 1.0f),
    ambient(0.0f, 0.0f, 0.0f, 0.0f),
    secDiffuse(0.0f, 0.0f, 0.0f, 0.0f),
    secSpecular(0.0f, 0.0f, 0.0f, 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nLight::~nLight()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetTransform(const matrix44& m)
{
    this->transform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nLight::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetType(Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nLight::Type
nLight::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetDiffuse(const vector4& c)
{
    this->diffuse = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLight::GetDiffuse() const
{
    return this->diffuse;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetSpecular(const vector4& c)
{
    this->specular = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLight::GetSpecular() const
{
    return this->specular;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetAmbient(const vector4& c)
{
    this->ambient = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLight::GetAmbient() const
{
    return this->ambient;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetSecondaryDiffuse(const vector4& c)
{
    this->secDiffuse = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLight::GetSecondaryDiffuse() const
{
    return this->secDiffuse;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLight::SetSecondarySpecular(const vector4& c)
{
    this->secSpecular;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLight::GetSecondarySpecular() const
{
    return this->secSpecular;
}

//------------------------------------------------------------------------------
#endif    
