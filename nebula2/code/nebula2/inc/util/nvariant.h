#ifndef N_VARIANT_H
#define N_VARIANT_H
//------------------------------------------------------------------------------
/**
    @class nVariant
    @ingroup Util

    An "any type" variable.

    Since the nVariant class has a rich set of assignment and cast operators,
    a variant variable can most of the time be used like a normal C++ variable.

    (C) 2004 RadonLabs GmbH.
*/
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nVariant
{
public:
    /// variant types
    enum Type
    {
        Void,
        Bool,
        Int,
        Float,
        Vector3,
        Vector4,
        Matrix44,
        String,
    };

    /// default constructor
    nVariant();
    /// copy constructor
    nVariant(const nVariant& other);
    /// bool constructor
    nVariant(bool b);
    /// int constructor
    nVariant(int i);
    /// float constructor
    nVariant(float f);
    /// vector3 constructor
    nVariant(const vector3& vec);
    /// vector4 constructor
    nVariant(const vector4& vec);
    /// matrix44 constructor
    nVariant(const matrix44& mat);
    /// string constructor
    nVariant(const char* str);
    /// destructor
    ~nVariant();
    /// get type
    Type GetType() const;

    /// assignment operator
    void operator=(const nVariant& rhs);
    /// bool assigment operator
    void operator=(bool rhs);
    /// int assignment operator
    void operator=(int rhs);
    /// float assignment operator
    void operator=(float rhs);
    /// vector3 assignment operator
    void operator=(const vector3& rhs);
    /// vector4 assignment operator
    void operator=(const vector4& rhs);
    /// matrix44 assignment operator
    void operator=(const matrix44& rhs);
    /// string assignment operator
    void operator=(const char* rhs);

    /// equality operator
    bool operator==(const nVariant& rhs) const;
    /// bool equality operator
    bool operator==(bool rhs) const;
    /// int equality operator
    bool operator==(int rhs) const;
    /// float equality operator
    bool operator==(float rhs) const;
    /// vector3 equality operator
    bool operator==(const vector3& rhs) const;
    /// vector4 equality operator
    bool operator==(const vector4& rhs) const;
    /// matrix44 equality operator
    bool operator==(const matrix44& rhs) const;
    /// string equality operator
    bool operator==(const char* rhs) const;

    /// inequality operator
    bool operator!=(const nVariant& rhs) const;
    /// bool inequality operator
    bool operator!=(bool rhs) const;
    /// int inequality operator
    bool operator!=(int rhs) const;
    /// float inequality operator
    bool operator!=(float rhs) const;
    /// vector3 inequality operator
    bool operator!=(const vector3& rhs) const;
    /// vector4 inequality operator
    bool operator!=(const vector4& rhs) const;
    /// matrix44 inequality operator
    bool operator!=(const matrix44& rhs) const;
    /// string inequality operator
    bool operator!=(const char* rhs) const;

    /// get bool content
    bool GetBool() const;
    /// get integer content
    int GetInt() const;
    /// get float content
    float GetFloat() const;
    /// get vector3 content
    const vector3& GetVector3() const;
    /// get vector4 content
    const vector4& GetVector4() const;
    /// get matrix44 content
    const matrix44& GetMatrix44() const;
    /// get string content
    const char* GetString() const;

    /// convert type to string
    static const char* TypeToString(Type t);
    /// convert string to type
    static Type StringToType(const char* str);

private:
    /// delete current content
    void Delete();
    /// copy current content
    void Copy(const nVariant& rhs);

    Type type;
    union
    {
        bool b;
        int i;
        float4 f;
        matrix44* mat;
        const char* str;
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant() :
    type(Void),
    mat(0) // union initialize 'str(0)' doesn't needed
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::Delete()
{
    if (Matrix44 == this->type)
	{
		n_assert(this->mat);
		n_delete(this->mat);
		this->mat = 0;
	}
    else if (String == this->type)
    {
        n_assert(this->str);
        n_free((void*)this->str);
        this->str = 0;
    }
    this->type = Void;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::Copy(const nVariant& other)
{
    n_assert(Void == this->type);
    this->type = other.type;
    switch (other.type)
    {
        case Void:
            break;
        case Bool:
            this->b = other.b;
            break;
        case Int:
            this->i = other.i;
            break;
        case Float:
            this->f[0] = other.f[0];
            break;
        case Vector3:
            this->f[0] = other.f[0];
            this->f[1] = other.f[1];
            this->f[2] = other.f[2];
            break;
        case Vector4:
            this->f[0] = other.f[0];
            this->f[1] = other.f[1];
            this->f[2] = other.f[2];
            this->f[3] = other.f[3];
            break;
        case Matrix44:
            this->mat = n_new(matrix44(*other.mat));
            break;
        case String:
            this->str = n_strdup(other.str);
            break;
        default:
            n_error("nVariant::Copy(): invalid type!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(const nVariant& rhs) :
    type(Void)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(bool rhs) :
    type(Bool),
    b(rhs)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(int rhs) :
    type(Int),
    i(rhs)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(float rhs) :
    type(Float)
{
    this->f[0] = rhs;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(const vector3& rhs) :
    type(Vector3)
{
    this->f[0] = rhs.x;
    this->f[1] = rhs.y;
    this->f[2] = rhs.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(const vector4& rhs) :
    type(Vector4)
{
    this->f[0] = rhs.x;
    this->f[1] = rhs.y;
    this->f[2] = rhs.z;
    this->f[3] = rhs.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(const matrix44& rhs) :
	type(Matrix44)
{
	this->mat = n_new(matrix44(rhs));
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant(const char* rhs) :
    type(String)
{
    this->str = n_strdup(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::~nVariant()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::Type
nVariant::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const nVariant& rhs)
{
    this->Delete();
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(bool rhs)
{
    this->Delete();
    this->type = Bool;
    this->b = rhs;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(int rhs)
{
    this->Delete();
    this->type = Int;
    this->i = rhs;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(float val)
{
    this->Delete();
    this->type = Float;
    this->f[0] = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const vector3& rhs)
{
    this->Delete();
    this->type = Vector3;
    this->f[0] = rhs.x;
    this->f[1] = rhs.y;
    this->f[2] = rhs.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const vector4& rhs)
{
    this->Delete();
    this->type = Vector4;
    this->f[0] = rhs.x;
    this->f[1] = rhs.y;
    this->f[2] = rhs.z;
    this->f[3] = rhs.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const matrix44& rhs)
{
	if (Matrix44 == this->type)
	{
		*this->mat = rhs;
	}
	else
	{
		this->Delete();
		this->mat = n_new(matrix44(rhs));
	}
	this->type = Matrix44;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const char* rhs)
{
    this->Delete();
    this->type = String;
    this->str = n_strdup(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(const nVariant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
            case Void:
                return true;
            case Bool:
                return (this->b == rhs.b);
            case Int:
                return (this->i == rhs.i);
            case Float:
                return (this->f[0] == rhs.f[0]);
            case Vector3:
                return ((this->f[0] == rhs.f[0]) &&
                        (this->f[1] == rhs.f[1]) &&
                        (this->f[2] == rhs.f[2]));
            case Vector4:
                return ((this->f[0] == rhs.f[0]) &&
                        (this->f[1] == rhs.f[1]) &&
                        (this->f[2] == rhs.f[2]) &&
                        (this->f[3] == rhs.f[3]));
            case Matrix44:
                return ((this->mat->M11 == rhs.mat->M11) &&
                        (this->mat->M12 == rhs.mat->M12) &&
                        (this->mat->M13 == rhs.mat->M13) &&
                        (this->mat->M14 == rhs.mat->M14) &&
                        (this->mat->M21 == rhs.mat->M21) &&
                        (this->mat->M22 == rhs.mat->M22) &&
                        (this->mat->M23 == rhs.mat->M23) &&
                        (this->mat->M24 == rhs.mat->M24) &&
                        (this->mat->M31 == rhs.mat->M31) &&
                        (this->mat->M32 == rhs.mat->M32) &&
                        (this->mat->M33 == rhs.mat->M33) &&
                        (this->mat->M34 == rhs.mat->M34) &&
                        (this->mat->M41 == rhs.mat->M41) &&
                        (this->mat->M42 == rhs.mat->M42) &&
                        (this->mat->M43 == rhs.mat->M43) &&
                        (this->mat->M44 == rhs.mat->M44));
            case String:
                return (0 == strcmp(this->str, rhs.str));
            default:
                n_error("nVariant::operator==(): invalid variant type!");
                return false;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(bool rhs) const
{
    n_assert(Bool == this->type);
    return (this->b == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(int rhs) const
{
    n_assert(Int == this->type);
    return (this->i == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(float rhs) const
{
    n_assert(Float == this->type);
    return (this->f[0] == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(const vector3& rhs) const
{
    n_assert(Vector3 == this->type);
    return ((this->f[0] == rhs.x) &&
            (this->f[1] == rhs.y) &&
            (this->f[2] == rhs.z));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(const vector4& rhs) const
{
    n_assert(Vector4 == this->type);
    return ((this->f[0] == rhs.x) &&
            (this->f[1] == rhs.y) &&
            (this->f[2] == rhs.z) &&
            (this->f[4] == rhs.w));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(const matrix44& rhs) const
{
    n_assert(Matrix44 == this->type);
    return ((this->mat->M11 == rhs.M11) &&
            (this->mat->M12 == rhs.M12) &&
            (this->mat->M13 == rhs.M13) &&
            (this->mat->M14 == rhs.M14) &&
            (this->mat->M21 == rhs.M21) &&
            (this->mat->M22 == rhs.M22) &&
            (this->mat->M23 == rhs.M23) &&
            (this->mat->M24 == rhs.M24) &&
            (this->mat->M31 == rhs.M31) &&
            (this->mat->M32 == rhs.M32) &&
            (this->mat->M33 == rhs.M33) &&
            (this->mat->M34 == rhs.M34) &&
            (this->mat->M41 == rhs.M41) &&
            (this->mat->M42 == rhs.M42) &&
            (this->mat->M43 == rhs.M43) &&
            (this->mat->M44 == rhs.M44));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator==(const char* rhs) const
{
    n_assert(String == this->type);
    return (0 == strcmp(rhs, this->str));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(const nVariant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
            case Void:
                return true;
            case Bool:
                return (this->b != rhs.b);
            case Int:
                return (this->i != rhs.i);
            case Float:
                return (this->f[0] != rhs.f[0]);
            case Vector3:
                return ((this->f[0] != rhs.f[0]) ||
                        (this->f[1] != rhs.f[1]) ||
                        (this->f[2] != rhs.f[2]));
            case Vector4:
                return ((this->f[0] != rhs.f[0]) ||
                        (this->f[1] != rhs.f[1]) ||
                        (this->f[2] != rhs.f[2]) ||
                        (this->f[3] != rhs.f[3]));
            case Matrix44:
                return ((this->mat->M11 != rhs.mat->M11) ||
                        (this->mat->M12 != rhs.mat->M12) ||
                        (this->mat->M13 != rhs.mat->M13) ||
                        (this->mat->M14 != rhs.mat->M14) ||
                        (this->mat->M21 != rhs.mat->M21) ||
                        (this->mat->M22 != rhs.mat->M22) ||
                        (this->mat->M23 != rhs.mat->M23) ||
                        (this->mat->M24 != rhs.mat->M24) ||
                        (this->mat->M31 != rhs.mat->M31) ||
                        (this->mat->M32 != rhs.mat->M32) ||
                        (this->mat->M33 != rhs.mat->M33) ||
                        (this->mat->M34 != rhs.mat->M34) ||
                        (this->mat->M41 != rhs.mat->M41) ||
                        (this->mat->M42 != rhs.mat->M42) ||
                        (this->mat->M43 != rhs.mat->M43) ||
                        (this->mat->M44 != rhs.mat->M44));
            case String:
                return (0 != strcmp(this->str, rhs.str));
            default:
                n_error("nVariant::operator==(): invalid variant type!");
                return true;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(bool rhs) const
{
    n_assert(Bool == this->type);
    return (this->b != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(int rhs) const
{
    n_assert(Int == this->type);
    return (this->i != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(float rhs) const
{
    n_assert(Float == this->type);
    return (this->f[0] != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(const vector3& rhs) const
{
    n_assert(Vector3 == this->type);
    return ((this->f[0] != rhs.x) ||
            (this->f[1] != rhs.y) ||
            (this->f[2] != rhs.z));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(const vector4& rhs) const
{
    n_assert(Vector4 == this->type);
    return ((this->f[0] != rhs.x) ||
            (this->f[1] != rhs.y) ||
            (this->f[2] != rhs.z) ||
            (this->f[4] != rhs.w));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(const matrix44& rhs) const
{
    n_assert(Matrix44 == this->type);
    return ((this->mat->M11 != rhs.M11) ||
            (this->mat->M12 != rhs.M12) ||
            (this->mat->M13 != rhs.M13) ||
            (this->mat->M14 != rhs.M14) ||
            (this->mat->M21 != rhs.M21) ||
            (this->mat->M22 != rhs.M22) ||
            (this->mat->M23 != rhs.M23) ||
            (this->mat->M24 != rhs.M24) ||
            (this->mat->M31 != rhs.M31) ||
            (this->mat->M32 != rhs.M32) ||
            (this->mat->M33 != rhs.M33) ||
            (this->mat->M34 != rhs.M34) ||
            (this->mat->M41 != rhs.M41) ||
            (this->mat->M42 != rhs.M42) ||
            (this->mat->M43 != rhs.M43) ||
            (this->mat->M44 != rhs.M44));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::operator!=(const char* rhs) const
{
    n_assert(String == this->type);
    return (0 != strcmp(rhs, this->str));
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nVariant::GetBool() const
{
    n_assert(Bool == this->type);
    return this->b;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nVariant::GetInt() const
{
    n_assert(Int == this->type);
    return this->i;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nVariant::GetFloat() const
{
    n_assert(Float == this->type);
    return this->f[0];
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nVariant::GetVector3() const
{
    n_assert(Vector3 == this->type);
    return *(vector3*)this->f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nVariant::GetVector4() const
{
    n_assert(Vector4 == this->type);
    return *(vector4*)this->f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nVariant::GetMatrix44() const
{
	n_assert(Matrix44 == this->type);
	return *(this->mat);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nVariant::GetString() const
{
    n_assert(String == this->type);
    return this->str;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nVariant::TypeToString(Type t)
{
    switch (t)
    {
        case Void:      return "void";
        case Bool:      return "bool";
        case Int:       return "int";
        case Float:     return "float";
        case Vector3:   return "vector3";
        case Vector4:   return "vector4";
        case Matrix44:  return "matrix44";
        case String:    return "string";
        default:
            n_error("nVariant::TypeToString(): invalid type enum '%d'!", t);
            return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::Type
nVariant::StringToType(const char* str)
{
    if      (0 == strcmp(str, "void"))    return Void;
    else if (0 == strcmp(str, "bool"))    return Bool;
    else if (0 == strcmp(str, "int"))     return Int;
    else if (0 == strcmp(str, "float"))   return Float;
    else if (0 == strcmp(str, "vector3")) return Vector3;
    else if (0 == strcmp(str, "vector4")) return Vector4;
    else if (0 == strcmp(str, "matrix44")) return Matrix44;
    else if (0 == strcmp(str, "string"))  return String;
    else
    {
        n_error("nVariant::StringToType(): invalid type string '%s'!", str);
        return Void;
    }
}

//------------------------------------------------------------------------------
#endif