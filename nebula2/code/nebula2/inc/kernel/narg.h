#ifndef N_ARG_H
#define N_ARG_H
//------------------------------------------------------------------------------
/**
    @class nArg
    @ingroup Kernel

    nArg encapsulates a typed argument (float, int, bool, string
    or object pointer) into an uniform C++ class.

    (C) 2002 RadonLabs GmbH
*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "kernel/ntypes.h"
#include "mathlib/matrix.h"

//------------------------------------------------------------------------------
class nArg
{
public:
    enum Type
    {
        Void,
        Int,
        Float,
        String,
        Bool,
        Object,
        List,
        Float4,
        Matrix44,
    };

    /// the default constructor
    nArg();
    /// the copy constructor
    nArg(const nArg& rhs);
    /// the destructor
    ~nArg();

    /// delete contents (sets type to Void)
    void Delete();
    /// copy content
    void Copy(const nArg& rhs);
    /// copy from va_list
    void Copy(va_list * marker);
    /// the equals operator
    bool operator==(const nArg& rhs) const;
    /// assignment operator
    nArg& operator=(const nArg& rhs);
    /// Set type corresponding to data type character and reset arg contents
    void Reset(char c);

    /// set content to an integer
    void SetI(int i);
    /// set content to a bool
    void SetB(bool b);
    /// set content to a float
    void SetF(float f);
    /// set content to a string
    void SetS(const char* s);
    /// set content to an (object) pointer
    void SetO(void*);
    /// Set content to an array of nArgs
    void SetL(nArg* _l, int len);
    /// set to float4
    void SetF4(const nFloat4& f4);
    /// set to float4, as vector4
    void SetV4(const vector4& v4);
    /// set to float4, as vector3
    void SetV3(const vector3& v3);
    /// set to matrix44
    void SetM44(const matrix44& m44);

    /// get the content data type
    Type GetType() const;
    /// get int content
    int   GetI() const;
    /// get bool content
    bool  GetB() const;
    /// get float content
    float GetF() const;
    /// get string content
    const char* GetS() const;
    /// get object pointer content
    void* GetO() const;
    /// Get an array of nArgs
    int GetL(nArg*&) const;
    /// get float4 content
    const nFloat4& GetF4() const;
    /// get content as vector4
    const vector4& GetV4() const;
    /// get content as vector3
    const vector3& GetV3() const;
    /// get matrix44 content
    const matrix44& GetM44() const;

    /// check if the provided data type character is a valid argument
    static bool IsValidArg(char c);
    /// check if the provided data type character is void
    static bool IsVoid(char c);

private:
    Type type;
    union
    {
        int i;
        bool b;
        float f;
        char *s;
        void *o;
        nArg *l;
        matrix44* m44;
        nFloat4 f4;
    };

    // Number of nArgs in array.
    // Only used for 'list' type
    int listLen;

};

//-----------------------------------------------------------------------------
/**
    Delete content, sets type to Void.
*/
inline
void
nArg::Delete()
{
    if ((this->type == String) && (0 != this->s))
    {
        n_free(this->s);
        this->s = 0;
    }
    else if (this->type == Matrix44)
    {
        n_assert(this->m44);
        n_delete(this->m44);
        this->m44 = 0;
    }
    else if (this->type == List)
    {
        if (this->l)
        {
            n_delete_array(this->l);
            this->l = 0;
            this->listLen = 0;
        }
    }
    this->type = Void;
}

//-----------------------------------------------------------------------------
/**
    Copy contents. Only calls delete if necessary to avoid memory allocation
    overhead.

    @param rhs reference to nArg object to be copied
*/
inline
void
nArg::Copy(const nArg& rhs)
{
    if (this->type != rhs.type)
    {
        this->Delete();
    }
    this->type = rhs.type;
    switch (this->type)
    {
        case Void:
            break;

        case Int:
            this->i = rhs.i;
            break;

        case Float:
            this->f = rhs.f;
            break;

        case String:
            this->Delete();
            // Delete() resets the type back to Void so need to set it again
            this->type = rhs.type;
            if (rhs.s)
            {
                this->s = n_strdup(rhs.s);
            }
            break;

        case Bool:
            this->b = rhs.b;
            break;

        case Object:
            this->o = rhs.o;
            break;

        case List:
            this->Delete();
            // Delete() resets the type back to Void so need to set it again
            this->type = rhs.type;
            if (rhs.l)
            {
                this->l = n_new_array(nArg, rhs.listLen);
                for (int i = 0; i < rhs.listLen; i++)
                {
                    this->l[i].Copy(rhs.l[i]);
                }
                this->listLen = rhs.listLen;
            }
            else
            {
                this->l = 0;
                this->listLen = 0;
            }
            break;

        case Float4:
            this->f4 = rhs.f4;
            break;

        case Matrix44:
            n_assert(rhs.m44);
            if (this->m44)
            {
                *this->m44 = *rhs.m44;
            }
            else
            {
                this->m44 = n_new(matrix44(*rhs.m44));
            }
            break;
    }
}


//-----------------------------------------------------------------------------
/**
    Copy contents from variable argument list marker. The type of the variable
    is known by the this->type.

    @param marker va_list parameter
*/
inline
void
nArg::Copy(va_list * marker)
{
    switch (this->type)
    {
        case Void:
            break;

        case Int:
            this->i = va_arg(*marker, int);
            break;

        case Float:
            // Compiler passes floats as double to variable argument functions
            this->f = float(va_arg(*marker, double));
            break;

        case String:
            {
                this->Delete();
                const char * str = va_arg(*marker, char *);
                if (str)
                {
                    this->s = n_strdup(str);
                }
            }
            break;

        case Bool:
            // bool is promoted to int when passed through ...
            {
                int tmp = va_arg(*marker, int);
                if (tmp)
                {
                    this->b = true;
                }
                else
                {
                    this->b = false;
                }
            }
            break;

        case Object:
            this->o = va_arg(*marker, void *);
            break;

        case List:
            n_assert(0);
            /* not possible to implement from va_list */
            break;

        case Float4:
            this->f4 = *(va_arg(*marker, nFloat4 *));
            break;

        case Matrix44:
            const matrix44 * mat = va_arg(*marker, matrix44 *);
            n_assert(mat);
            if (this->m44)
            {
                *this->m44 = *mat;
            }
            else
            {
                this->m44 = n_new(matrix44(*mat));
            }
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    The default constructor will initialize the arg type to Void
*/
inline
nArg::nArg() :
    type(Void),
    s(0),
    listLen(0)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    The copy constructor simply calls nArg::Copy()

    @param rhs reference to nArg object to be copied
*/
inline
nArg::nArg(const nArg& rhs) :
    type(Void),
    s(0),
    listLen(0)
{
    this->Copy(rhs);
}

//-----------------------------------------------------------------------------
/**
*/
inline nArg::~nArg()
{
    this->Delete();
}

//-----------------------------------------------------------------------------
/**
*/
inline
nArg&
nArg::operator=(const nArg& rhs)
{
    this->Copy(rhs);
    return *this;
}

//-----------------------------------------------------------------------------
/**
    Checks whether data type and contents of the object are identical.
*/
inline
bool
nArg::operator==(const nArg& rhs) const
{
    if (this->type == rhs.type)
    {
        switch (this->type)
        {
            case Void:
                return true;

            case Int:
                return (this->i == rhs.i);

            case Float:
                return (this->f == rhs.f);

            case String:
                n_assert(this->s && rhs.s);
                return (0 == strcmp(this->s, rhs.s));

            case Bool:
                return (this->b == rhs.b);

            case Object:
                return (this->o == rhs.o);

            case List:
                if ((this->l && rhs.l) && (this->listLen == rhs.listLen))
                {
                    for (int i = 0; i < this->listLen; i++)
                    {
                        if (!(this->l[i] == rhs.l[i]))
                            return false;
                    }
                    return true;
                }

            case Float4:
                return ((this->f4.x == rhs.f4.x) &&
                        (this->f4.y == rhs.f4.y) &&
                        (this->f4.z == rhs.f4.z) &&
                        (this->f4.w == rhs.f4.w));

            case Matrix44:
                n_error("nArg::operator==(): Cannot compare matrix44 objects!");
                return false;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to an integer, and sets the arg type to Int.

    @param _i the integer
*/
inline
void
nArg::SetI(int _i)
{
    n_assert((Void == this->type) || (Int == this->type));
    this->type = Int;
    this->i = _i;
}
//-----------------------------------------------------------------------------
/**
    Sets the contents to a bool value, and set the arg type to Bool.

    @param _b the bool value
*/
inline
void
nArg::SetB(bool _b)
{
    n_assert((Void == this->type) || (Bool == this->type));
    this->type = Bool;
    this->b = _b;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to a float value, and sets the arg type to Float.

    @param _f the float value
*/
inline
void
nArg::SetF(float _f)
{
    n_assert((Void == this->type) || (Float == this->type));
    this->type = Float;
    this->f = _f;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to a string, and sets the arg type to String.
    The string is duplicated internally.

    @param _s the string
*/
inline
void
nArg::SetS(const char *_s)
{
    n_assert((Void == this->type) || (String == this->type));
    this->Delete();
    this->type = String;
    if (_s)
    {
        this->s = n_strdup(_s);
    }
}


//-----------------------------------------------------------------------------
/**
    Sets the contents to an object pointer, and sets the arg type to
    Object. The pointer is NOT safe (if the object is destroyed,
    the pointer points to Nirvana).

    @param _o the object pointer
*/
inline
void
nArg::SetO(void *_o)
{
    n_assert((Void == this->type) || (Object == this->type));
    this->type = Object;
    this->o = _o;
}

//-----------------------------------------------------------------------------
/**
    Sets the contents to an array of other nArgs, and sets the arg type
    to List.
    The array is NOT duplicated.

    @param _l pointer to array of nArg
    @param len length of array
*/
inline void nArg::SetL(nArg* _l, int len)
{
    n_assert((Void == this->type) || (List == this->type));
    this->Delete();
    this->type = List;
    if (_l)
    {
        this->l = _l;
        this->listLen = len;
    }
    else
    {
        this->l = 0;
        this->listLen = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Set the content to a float4 value.
*/
inline
void
nArg::SetF4(const nFloat4& _f4)
{
    n_assert((Void == this->type) || (Float4 == this->type));
    this->type = Float4;
    this->f4 = _f4;
}

//-----------------------------------------------------------------------------
/**
    Set the content to a Float4 value, with vector4 as parameter.
*/
inline
void
nArg::SetV4(const vector4& v)
{
    n_assert((Void == this->type) || (Float4 == this->type));
    this->type = Float4;
    this->f4 = *(nFloat4*)&v;
}

//-----------------------------------------------------------------------------
/**
    Set the content to a vector3 value.
*/
inline
void
nArg::SetV3(const vector3& v)
{
    n_assert((Void == this->type) || (Float4 == this->type));
    this->type = Float4;
    this->f4.x = v.x;
    this->f4.y = v.y;
    this->f4.z = v.z;
    this->f4.w = 0.0f;
}

//-----------------------------------------------------------------------------
/**
    Set the content to a matrix44 value.
*/
inline
void
nArg::SetM44(const matrix44& _m44)
{
    n_assert((Void == this->type) || (Matrix44 == this->type));
    if (this->m44)
    {
        *this->m44 = _m44;
    }
    else
    {
        this->m44 = n_new(matrix44(_m44));
    }
    this->type = Matrix44;
}

//-----------------------------------------------------------------------------
/**
    Returns the argument type of the object, which is one of
    Void        - unitialized
    Int
    Bool
    Float
    List
    Float4
    Matrix44

    @return the arg type
*/
inline
nArg::Type
nArg::GetType() const
{
    return this->type;
}

//-----------------------------------------------------------------------------
/**
    Returns the integer content of the object. Throws assertion if
    not set to an integer.

    @return the integer value
*/
inline
int
nArg::GetI() const
{
    n_assert(Int == this->type);
    return this->i;
}

//-----------------------------------------------------------------------------
/**
    Returns the bool content of the object. Throws assertion if
    not set to a bool.

    @return the bool value
*/
inline
bool
nArg::GetB() const
{
    n_assert(Bool == this->type);
    return this->b;
}

//-----------------------------------------------------------------------------
/**
    Returns the float content of the object. Throws assertion if
    not set to a float.

    @return the float value
*/
inline
float
nArg::GetF() const
{
    n_assert(Float == this->type);
    return this->f;
}

//-----------------------------------------------------------------------------
/**
    Returns the string content of the object. Throws assertion if
    not set to a string.

    @return the string pointer
*/
inline
const char*
nArg::GetS() const
{
    n_assert(String == this->type);
    return this->s;
}

//-----------------------------------------------------------------------------
/**
    Returns the object pointer content of the object. Throws assertion if
    not set to an object pointer.

    @return the object pointer
*/
inline
void*
nArg::GetO() const
{
    n_assert(Object == this->type);
    return this->o;
}

//-----------------------------------------------------------------------------
/**
    Returns the nArg array content of the object. Throws assertion if
    not set to a list.

    @param _l Pointer to nArg array. Will be set by this function

    @return the length of the array
*/
inline
int
nArg::GetL(nArg*& _l) const
{
    n_assert(List == this->type);
    _l = this->l;
    return this->listLen;
}

//-----------------------------------------------------------------------------
/**
    Return float4 content.
*/
inline
const nFloat4&
nArg::GetF4() const
{
    n_assert(Float4 == this->type);
    return this->f4;
}

//-----------------------------------------------------------------------------
/**
    Return float4 content as vector4
*/
inline
const vector4&
nArg::GetV4() const
{
    n_assert(Float4 == this->type);
    return *(vector4*)&this->f4;
}

//-----------------------------------------------------------------------------
/**
    Return float4 content as vector3
*/
inline
const vector3&
nArg::GetV3() const
{
    n_assert(Float4 == this->type);
    return *(vector3*)&this->f4;
}

//-----------------------------------------------------------------------------
/**
    Return matrix44 content.
*/
inline
const matrix44&
nArg::GetM44() const
{
    n_assert(Matrix44 == this->type);
    n_assert(this->m44);
    return *(this->m44);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nArg::IsValidArg(char c)
{
    switch (c)
    {
        case 'i':
        case 'f':
        case 's':
        case 'v':
        case 'b':
        case 'o':
        case 'l':
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nArg::IsVoid(char c)
{
    if ('v' == c)
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nArg::Reset(char c)
{
    switch (c)
    {
        case 'i':   this->SetI(0);      break;
        case 'f':   this->SetF(0.0f);   break;
        case 'b':   this->SetB(false);  break;
        case 's':   this->SetS(0);      break;
        case 'o':   this->SetO(0);      break;
        case 'l':   this->SetL(0,0);    break;
        default:    break;
    }
}

//--------------------------------------------------------------------
#endif
