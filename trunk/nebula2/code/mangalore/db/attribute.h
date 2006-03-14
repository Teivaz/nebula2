#ifndef DB_ATTR_H
#define DB_ATTR_H
//------------------------------------------------------------------------------
/**
    @class Db::Attr
    
    A database subsystem attribute. Attributes associate a name with a typed
    value. Names are implemented using Foundation's Id class, which means
    attribute names are compile-time safe and implemented as C++ objects.

    Things to know:

    - All attributes are readable
    - Attributes can be writable and/or storable, use IsWritable() or IsStorable() to find out.
    - db.xml instance attribute are writable and storable, class attributes are only readable.
    - Attributes can be empty if no value has been set yet, empty attributes can
      still have a data type and id, use Clear() to make an attribute empty
      without changing its type or id, use IsEmpty() to check whether an attribute is
      empty.  
    - Empty db.xml table cells have empty attributes of the correct type and id assigned
      to them.
    - Assigning a value will fail hard if the attribute is not writable or is not of the same type.

    (C) 2005 Radon Labs GmbH
*/
#include "attr/attributeid.h"
#include "attr/voidattributeid.h"
#include "attr/boolattributeid.h"
#include "attr/intattributeid.h"
#include "attr/floatattributeid.h"
#include "attr/stringattributeid.h"
#include "attr/vector3attributeid.h"
#include "attr/vector4attributeid.h"
#include "attr/matrix44attributeid.h"
#include "mathlib/vector.h"
#include "util/nvariant.h"

//------------------------------------------------------------------------------
namespace Db
{
class Attribute
{
public:
    /// attribute data types
    enum Type
    {
        Void     = nVariant::Void,
        Int      = nVariant::Int,
        Float    = nVariant::Float,
        Bool     = nVariant::Bool,
        Vector3  = nVariant::Vector3,
        Vector4  = nVariant::Vector4,
        String   = nVariant::String,
        Matrix44 = nVariant::Matrix44,
    };        

    /// default constructor
    Attribute();
    /// copy constructor
    Attribute(const Attribute& rhs);
    /// constructor with only the attibute id
    Attribute(const Attr::AttributeID& id);
    /// constructor with only the attibute id
    Attribute(const Attr::VoidAttributeID& id);
    /// string constructor
    Attribute(const Attr::StringAttributeID& id, const nString& val);
    /// int constructor
    Attribute(const Attr::IntAttributeID& id, int val);
    /// float constructor
    Attribute(const Attr::FloatAttributeID& id, float val);
    /// bool constructor
    Attribute(const Attr::BoolAttributeID& id, bool val);
    /// vector3 constructor
    Attribute(const Attr::Vector3AttributeID& id, const vector3& val);
    /// vector4 constructor
    Attribute(const Attr::Vector4AttributeID& id, const vector4& val);
    /// matrix44 constructor
    Attribute(const Attr::Matrix44AttributeID& id, const matrix44& val);

    /// set id of attribute
    void SetAttributeID(const Attr::AttributeID& id);
    /// return id of attribute
    const Attr::AttributeID& GetAttributeID() const;
    /// return id name of attribute
    const nString& GetName() const;
    /// return data type of attribute
    Type GetType() const;
    /// return true if attribute is writable
    bool IsWritable() const;
    /// return true if attribute is storable
    bool IsStorable() const;
    /// clear the attribute's value
    void Clear();
    /// return true if the attribute id is valid
    bool IsValid() const;
    /// check if attribute value has been set
    bool IsEmpty() const;

    /// assignment operator
    void operator=(const Attribute& rhs);
    /// string assignment operator
    void operator=(const nString& rhs);
    /// int assignment operator
    void operator=(int rhs);
    /// float assignment operator
    void operator=(float rhs);
    /// bool assignment operator
    void operator=(bool rhs);
    /// vector3 assignment operator
    void operator=(const vector3& rhs);
    /// vector4 assignment operator
    void operator=(const vector4& rhs);
    /// matrix44 assignment operator
    void operator=(const matrix44& rhs);

    /// equality operator
    bool operator==(const Attribute& rhs) const;
    /// string equality operator
    bool operator==(const nString& rhs) const;
    /// int equality operator
    bool operator==(int rhs) const;
    /// float equality operator
    bool operator==(float rhs) const;
    /// bool equality operator
    bool operator==(bool rhs) const;
    /// vector3 equality operator
    bool operator==(const vector3& rhs) const;
    /// vector4 equality operator
    bool operator==(const vector4& rhs) const;

    /// equality operator
    bool operator!=(const Attribute& rhs) const;
    /// string equality operator
    bool operator!=(const nString& rhs) const;
    /// int equality operator
    bool operator!=(int rhs) const;
    /// float equality operator
    bool operator!=(float rhs) const;
    /// bool equality operator
    bool operator!=(bool rhs) const;
    /// vector3 equality operator
    bool operator!=(const vector3& rhs) const;
    /// vector4 equality operator
    bool operator!=(const vector4& rhs) const;

    /// set string content
    void SetString(const nString& s);
    /// set int content
    void SetInt(int i);
    /// set float content
    void SetFloat(float f);
    /// set bool content
    void SetBool(bool b);
    /// set vector3 content
    void SetVector3(const vector3& v);
    /// set vector4 content
    void SetVector4(const vector4& v);
    /// set matrix44 content
    void SetMatrix44(const matrix44& m);
    /// set generic attribute value
    void SetValue(const nVariant& val);

    /// get string content
    nString GetString() const;
    /// get int content
    int GetInt() const;
    /// get float content
    float GetFloat() const;
    /// get bool content
    bool GetBool() const;
    /// get vector3 content
    const vector3& GetVector3() const;
    /// get vector4 content
    const vector4& GetVector4() const;
    /// get matrix44 content
    const matrix44& GetMatrix44() const;
    /// get generic attribute value
    const nVariant& GetValue() const;

    /// return the contents of the attribute as a string, this may involve a conversion
    nString AsString() const;
    /// convert string to data type
    static Type StringToType(const nString& s);
    /// convert data type to string
    static nString TypeToString(Type t);

private:
    /// put a read-only error message
    void PutWriteError() const;
        
    Attr::AttributeID attrId;
    nVariant value;
    bool empty;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::PutWriteError() const
{
    n_error("Trying to write read-only attribute '%s'!", this->GetName());
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute() :
    attrId(),
    empty(true)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute(const Attribute& rhs) :
    attrId(rhs.attrId),
    empty(false),
    value(rhs.value)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute(const Attr::AttributeID& id) :
    attrId( id ),
    empty( true )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute(const Attr::VoidAttributeID& i) :
    attrId( i ),
    empty(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::StringAttributeID& i, const nString& v) :
    attrId( i ),
    empty(false),
    value(v.Get())
{
    n_assert(this->GetType() == String);
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::IntAttributeID& i, int v) :
    attrId( i ),
    empty(false),
    value(v)
{
    n_assert(this->GetType() == Int);
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::FloatAttributeID& i, float v) :
    attrId( i ),
    empty(false),
    value(v)
{
    n_assert(this->GetType() == Float);
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::BoolAttributeID& i, bool v) :
    attrId( i ),
    empty(false),
    value(v)
{
    n_assert(this->GetType() == Bool);
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::Vector3AttributeID& i, const vector3& v) :
    attrId( i ),
    empty(false),
    value(v)
{
    n_assert(this->GetType() == Vector3);
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::Vector4AttributeID& i, const vector4& v) :
    attrId( i ),
    empty(false),
    value(v)
{
    n_assert(this->GetType() == Vector4);
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Attribute( const Attr::Matrix44AttributeID& i, const matrix44& m) :
    attrId( i ),
    empty(false),
    value(m)
{
    n_assert(this->GetType() == Matrix44);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(const Attribute& rhs)
{
    this->attrId = rhs.attrId;
    this->empty = rhs.empty;
    this->value = rhs.value;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(const nString& rhs)
{
    n_assert(this->GetType() == String);
    this->value = rhs.Get();
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(int rhs)
{
    n_assert(this->GetType() == Int);
    this->value = rhs;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(float rhs)
{
    n_assert(this->GetType() == Float);
    this->value = rhs;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(bool rhs)
{
    n_assert(this->GetType() == Bool);
    this->value = rhs;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(const vector3& rhs)
{
    n_assert(this->GetType() == Vector3);
    this->value = rhs;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(const vector4& rhs)
{
    n_assert(this->GetType() == Vector4);
    this->value = rhs;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::operator=(const matrix44& rhs)
{
    n_assert(this->GetType() == Matrix44);
    this->value = rhs;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(const Attribute& rhs) const
{
    return (this->attrId == rhs.attrId) && (this->value == rhs.value);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(const nString& rhs) const
{
    n_assert(this->GetType() == String);
    return (this->value == rhs.Get());
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(int rhs) const
{
    n_assert(this->GetType() == Int);
    return (this->value == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(float rhs) const
{
    n_assert(this->GetType() == Float);
    return (this->value == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(bool rhs) const
{
    n_assert(this->GetType() == Bool);
    return (this->value == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(const vector3& rhs) const
{
    n_assert(this->GetType() == Vector3);
    return (this->value == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator==(const vector4& rhs) const
{
    n_assert(this->GetType() == Vector4);
    return (this->value == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(const Attribute& rhs) const
{
    return (this->attrId != rhs.attrId) || (this->value != rhs.value);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(const nString& rhs) const
{
    n_assert(this->GetType() == String);
    return (this->value != rhs.Get());
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(int rhs) const
{
    n_assert(this->GetType() == Int);
    return (this->value != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(float rhs) const
{
    n_assert(this->GetType() == Float);
    return (this->value != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(bool rhs) const
{
    n_assert(this->GetType() == Bool);
    return (this->value != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(const vector3& rhs) const
{
    n_assert(this->GetType() == Vector3);
    return (this->value != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::operator!=(const vector4& rhs) const
{
    n_assert(this->GetType() == Vector4);
    return (this->value != rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetAttributeID(const Attr::AttributeID& i)
{
    this->attrId = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Attr::AttributeID&
Attribute::GetAttributeID() const
{
    return this->attrId;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
Attribute::GetName() const
{
    n_assert2( this->attrId.IsValid(), "Valid attribute id" );
    return this->attrId.GetName();
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Type
Attribute::GetType() const
{
    n_assert2( this->attrId.IsValid(), "Valid attribute id" );
    return (Attribute::Type) this->attrId.GetType();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::IsWritable() const
{
    n_assert2( this->attrId.IsValid(), "Valid attribute id" );
    return this->attrId.IsWritable();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::IsStorable() const
{
    n_assert2( this->attrId.IsValid(), "Valid attribute id" );
    return this->attrId.IsStorable();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::Clear()
{
    this->empty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::IsEmpty() const
{
    return this->empty;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::IsValid() const
{
    return this->attrId.IsValid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetString(const nString& s)
{
    n_assert(this->GetType() == String);
    this->value = s.Get();
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetInt(int i)
{
    n_assert(this->GetType() == Int);
    this->value = i;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetFloat(float f)
{
    n_assert(this->GetType() == Float);
    this->value = f;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetBool(bool b)
{
    n_assert(this->GetType() == Bool);
    this->value = b;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetVector3(const vector3& v)
{
    n_assert(this->GetType() == Vector3);
    this->value = v;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetVector4(const vector4& v)
{
    n_assert(this->GetType() == Vector4);
    this->value = v;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetMatrix44(const matrix44& m)
{
    n_assert(this->GetType() == Matrix44);
    this->value = m;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Attribute::SetValue(const nVariant& val)
{
    this->value = val;
    this->empty = false;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
Attribute::GetString() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == String));
    return nString(this->value.GetString());
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Attribute::GetInt() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == Int));
    return this->value.GetInt();
}

//------------------------------------------------------------------------------
/**
*/
inline
float
Attribute::GetFloat() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == Float));
    return this->value.GetFloat();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
Attribute::GetBool() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == Bool));
    return this->value.GetBool();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
Attribute::GetVector3() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == Vector3));
    return this->value.GetVector3();
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
Attribute::GetVector4() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == Vector4));
    return this->value.GetVector4();
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
Attribute::GetMatrix44() const
{
    n_assert(!this->IsEmpty() && (this->GetType() == Matrix44));
    return this->value.GetMatrix44();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nVariant&
Attribute::GetValue() const
{
    n_assert(!this->IsEmpty());
    return this->value;
}

//------------------------------------------------------------------------------
/**
*/
inline
Attribute::Type
Attribute::StringToType(const nString& s)
{
    return (Attribute::Type) nVariant::StringToType(s.Get());
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
Attribute::TypeToString(Type t)
{
    return nString(nVariant::TypeToString((nVariant::Type) t));
}

//------------------------------------------------------------------------------
/**
    This returns the content of the attribute as a string which may involve
    an expensive conversion.
*/
inline
nString
Attribute::AsString() const
{
    n_assert(this->GetType() != Void);
    nString str;
    switch (this->GetType())
    {
        case Int:
            str.SetInt(this->GetInt());
            break;

        case Float:
            str.SetFloat(this->GetFloat());
            break;

        case Bool:
            str.SetBool(this->GetBool());
            break;

        case Vector3:
            str.SetVector3(this->GetVector3());
            break;

        case Vector4:
            str.SetVector4(this->GetVector4());
            break;

        case String:
            str = this->GetString();
            break;

        case Matrix44:
            str.SetMatrix44(this->GetMatrix44());
            break;
    }
    return str;
}

}; // namespace Db
//------------------------------------------------------------------------------
#endif
