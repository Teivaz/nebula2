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

//------------------------------------------------------------------------------
class nVariant
{
public:
    /// variant types
    enum Type
    {
        Void,
        Int,
        Float,
        Bool,
        Vector3,
        Vector4,
        String,
        Matrix44,
    };

    /// default constructor
    nVariant();
    /// copy constructor
    nVariant(const nVariant& rhs);
    /// int constructor
    nVariant(int rhs);
    /// float constructor
    nVariant(float rhs);
    /// bool constructor
    nVariant(bool rhs);
    /// string constructor
    nVariant(const char* rhs);
    /// vector3 constructor
    nVariant(const vector3& v);
    /// vector4 constructor
    nVariant(const vector4& v);
    /// matrix44 constructor
    nVariant(const matrix44& m);
    /// destructor
    ~nVariant();
    /// set type of attribute
    void SetType(Type t);
    /// get type
    Type GetType() const;

    /// assignment operator
    void operator=(const nVariant& rhs);
    /// int assignment operator
    void operator=(int val);
    /// float assignment operator
    void operator=(float val);
    /// bool assignment operator
    void operator=(bool val);
    /// string assignment operator
    void operator=(const char* s);
    /// vector3 assignment operator
    void operator=(const vector3& val);
    /// vector4 assignment operator
    void operator=(const vector4& val);
    /// matrix44 assignment operator
    void operator=(const matrix44& val);

    /// equality operator
    bool operator==(const nVariant& rhs) const;
    /// int equality operator
    bool operator==(int rhs) const;
    /// float equality operator
    bool operator==(float rhs) const;
    /// bool equality operator
    bool operator==(bool rhs) const;
    /// string equality operator
    bool operator==(const char* rhs) const;
    /// vector3 equality operator
    bool operator==(const vector3& rhs) const;
    /// vector4 equality operator
    bool operator==(const vector4& rhs) const;

    /// inequality operator
    bool operator!=(const nVariant& rhs) const;
    /// int inequality operator
    bool operator!=(int rhs) const;
    /// float inequality operator
    bool operator!=(float rhs) const;
    /// bool inequality operator
    bool operator!=(bool rhs) const;
    /// string inequality operator
    bool operator!=(const char* rhs) const;
    /// vector3 inequality operator
    bool operator!=(const vector3& rhs) const;
    /// vector4 inequality operator
    bool operator!=(const vector4& rhs) const;

    /// set integer content
    void SetInt(int val);
    /// get integer content
    int GetInt() const;
    /// set float content
    void SetFloat(float val);
    /// get float content
    float GetFloat() const;
    /// set bool content
    void SetBool(bool val);
    /// get bool content
    bool GetBool() const;
    /// set string content
    void SetString(const char* val);
    /// get string content
    const char* GetString() const;
    /// set vector3 content
    void SetVector3(const vector3& val);
    /// get vector3 content
    const vector3& GetVector3() const;
    /// set vector4 content
    void SetVector4(const vector4& val);
    /// get vector4 content
    const vector4& GetVector4() const;
    /// set matrix44 content
    void SetMatrix44(const matrix44& val);
    /// get matrix44 content
    const matrix44& GetMatrix44() const;

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
        int i;
        bool b;
        const char* str;
        float f[4];
        matrix44* m;
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
nVariant::nVariant() :
    type(Void),
    str(0)
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
    if (String == this->type)
    {
        n_assert(this->str);
        n_free((void*)this->str);
        this->str = 0;
    }
    else if (Matrix44 == this->type)
    {
        n_assert(this->m);
        n_delete(this->m);
        this->m = 0;
    }
    this->type = Void;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::Copy(const nVariant& rhs)
{
    n_assert(Void == this->type);
    this->type = rhs.type;
    switch (rhs.type)
    {
        case Void:
            break;
        case Int:
            this->i = rhs.i;
            break;
        case Float:
            this->f[0] = rhs.f[0];
            break;
        case Bool:
            this->b = rhs.b;
            break;
        case Vector3:
            this->f[0] = rhs.f[0];
            this->f[1] = rhs.f[1];
            this->f[2] = rhs.f[2];
            break;
        case Vector4:
            this->f[0] = rhs.f[0];
            this->f[1] = rhs.f[1];
            this->f[2] = rhs.f[2];
            this->f[3] = rhs.f[3];
            break;
        case String:
            this->str = n_strdup(rhs.str);
            break;
        case Matrix44:
            this->m = n_new(matrix44(*rhs.m));
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
nVariant::nVariant(const char* rhs) :
    type(String)
{
    this->str = n_strdup(rhs);
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
    this->m = n_new(matrix44(rhs));
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
void
nVariant::SetType(Type t)
{
    this->type = t;
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
nVariant::operator=(int val)
{
    this->Delete();
    this->type = Int;
    this->i = val;
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
nVariant::operator=(bool val)
{
    this->Delete();
    this->type = Bool;
    this->b = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const char* s)
{
    this->Delete();
    this->type = String;
    this->str = n_strdup(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const vector3& val)
{
    this->Delete();
    this->type = Vector3;
    this->f[0] = val.x;
    this->f[1] = val.y;
    this->f[2] = val.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const vector4& val)
{
    this->Delete();
    this->type = Vector4;
    this->f[0] = val.x;
    this->f[1] = val.y;
    this->f[2] = val.z;
    this->f[3] = val.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::operator=(const matrix44& val)
{
    if (Matrix44 == this->type)
    {
        *this->m = val;
    }
    else
    {
        this->Delete();
        this->m = n_new(matrix44(val));
    }
    this->type = Matrix44;
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
            case Int:
                return (this->i == rhs.i);
            case Bool:
                return (this->b == rhs.b);
            case Float:
                return (this->f[0] == rhs.f[0]);
            case String:
                return (0 == strcmp(this->str, rhs.str));
            case Vector3:
                return ((this->f[0] == rhs.f[0]) &&
                        (this->f[1] == rhs.f[1]) &&
                        (this->f[2] == rhs.f[2]));
            case Vector4:
                return ((this->f[0] == rhs.f[0]) &&
                        (this->f[1] == rhs.f[1]) &&
                        (this->f[2] == rhs.f[2]) &&
                        (this->f[3] == rhs.f[3]));
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
            (this->f[3] == rhs.w));
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
            case Int:
                return (this->i != rhs.i);
            case Float:
                return (this->f[0] != rhs.f[0]);
            case Bool:
                return (this->b != rhs.b);
            case String:
                return (0 != strcmp(this->str, rhs.str));
            case Vector3:
                return ((this->f[0] != rhs.f[0]) ||
                        (this->f[1] != rhs.f[1]) ||
                        (this->f[2] != rhs.f[2]));
            case Vector4:
                return ((this->f[0] != rhs.f[0]) ||
                        (this->f[1] != rhs.f[1]) ||
                        (this->f[2] != rhs.f[2]) ||
                        (this->f[3] != rhs.f[3]));
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
            (this->f[3] != rhs.w));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariant::SetInt(int val)
{
    *this = val;
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
void
nVariant::SetFloat(float val)
{
    *this = val;
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
void
nVariant::SetBool(bool val)
{
    *this = val;
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
void
nVariant::SetString(const char* val)
{
    *this = val;
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
void
nVariant::SetVector3(const vector3& val)
{
    *this = val;
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
void
nVariant::SetVector4(const vector4& val)
{
    *this = val;
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
void
nVariant::SetMatrix44(const matrix44& val)
{
    *this = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nVariant::GetMatrix44() const
{
    n_assert(Matrix44 == this->type);
    return *(this->m);
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
        case Int:       return "int";
        case Float:     return "float";
        case Bool:      return "bool";
        case Vector3:   return "vector3";
        case Vector4:   return "vector4";
        case String:    return "string";
        case Matrix44:  return "matrix44";
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
    if (0 == strcmp(str, "void")) return Void;
    if (0 == strcmp(str, "int")) return Int;
    if (0 == strcmp(str, "float")) return Float;
    if (0 == strcmp(str, "bool")) return Bool;
    if (0 == strcmp(str, "vector3")) return Vector3;
    if (0 == strcmp(str, "vector4")) return Vector4;
    if (0 == strcmp(str, "color")) return Vector4; // NOT A BUG!
    if (0 == strcmp(str, "string")) return String;
    if (0 == strcmp(str, "matrix44")) return Matrix44;
    n_error("nVariant::StringToType(): invalid type string '%s'!", str);
    return Void;
}

//------------------------------------------------------------------------------
#endif
