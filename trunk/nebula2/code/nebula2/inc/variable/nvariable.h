#ifndef N_VARIABLE_H
#define N_VARIABLE_H
//------------------------------------------------------------------------------
/**
    @class nVariable

    A variable contains typed data and is identified by a variable handle.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nVariable
{
public:
    /// variable types
    enum Type
    {
        NOTYPE,
        INT,
        FLOAT,
        VECTOR,
        STRING,
        OBJECT,
    };

    typedef uint Handle;
    enum
    {
        INVALID_HANDLE = 0xffffffff,
    };

    /// default constructor
    nVariable();
    /// int constuctor
    nVariable(Handle h, int val);
    /// float constructor
    nVariable(Handle h, float val);
    /// vector constructor
    nVariable(Handle h, const float4& val);
    /// string constructor
    nVariable(Handle h, const char* str);
    /// object constructor
    nVariable(Handle h, void* ptr);
    /// constructor
    nVariable(Type t, Handle h);
    /// copy contstructor
    nVariable(const nVariable& rhs);
    /// destructor
    ~nVariable();
    /// assignment operator
    nVariable& operator=(const nVariable& rhs);
    /// clear content
    void Clear();
    /// set variable type
    void SetType(Type t);
    /// get variable type
    Type GetType() const;
    /// set variable handle
    void SetHandle(Handle h);
    /// get variable handle 
    Handle GetHandle() const;
    /// set int content
    void SetInt(int val);
    /// get int content
    int GetInt() const;
    /// set float content
    void SetFloat(float val);
    /// get float content
    float GetFloat() const;
    /// set vector content
    void SetVector(const float4& v);
    /// get vector content
    const float4& GetVector() const;
    /// set string content (will be copied internally)
    void SetString(const char* str);
    /// get string content
    const char* GetString() const;
    /// set object content
    void SetObject(void* ptr);
    /// get object context
    void* GetObject() const;

private:
    /// delete content
    void Delete();
    /// copy content
    void Copy(const nVariable& from);

    Handle handle;
    Type type;
    union
    {
        int intVal;
        float floatVal;
        float4 vectorVal;
        const char* stringVal;
        void* objectVal;
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable() :
    handle(INVALID_HANDLE),
    type(NOTYPE),
    stringVal(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(Type t, Handle h) :
    handle(h),
    type(t),
    stringVal(0)
{
    // empty
}
   
//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::Delete()
{
    if ((STRING == this->type) && (this->stringVal))
    {
        n_free((void*) this->stringVal);
        this->stringVal = 0;
    }
    this->handle = INVALID_HANDLE;
    this->type = NOTYPE;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::Copy(const nVariable& from)
{
    this->handle   = from.handle;
    this->type     = from.type;
    switch (from.type)
    {
        case INT:
            this->intVal = from.intVal;
            break;

        case FLOAT:
            this->floatVal = from.floatVal;
            break;

        case VECTOR:
            this->vectorVal[0] = from.vectorVal[0];
            this->vectorVal[1] = from.vectorVal[1];
            this->vectorVal[2] = from.vectorVal[2];
            this->vectorVal[3] = from.vectorVal[3];
            break;

        case STRING:
            n_assert(0 == this->stringVal);
            if (from.stringVal)
            {
                this->stringVal = n_strdup(from.stringVal);
            }
            break;

        case OBJECT:
            this->objectVal = from.objectVal;
            break;

        default:
            n_assert(false);
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(const nVariable& rhs)
{
    this->Copy(rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::~nVariable()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable&
nVariable::operator=(const nVariable& rhs)
{
    this->Delete();
    this->Copy(rhs);
    return (*this);
}

//------------------------------------------------------------------------------
/**
    Clear content, this resets the variable type, handle and content.
*/
inline
void
nVariable::Clear()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
    Set the variable's data type. This can only be invoked on a
    new or cleared variable object.
*/
inline
void
nVariable::SetType(Type t)
{
    n_assert(NOTYPE == this->type);
    n_assert(NOTYPE != t);
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::Type
nVariable::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
    Set the variable's handle, this can only be invoked on a new or cleared
    variable object.
*/
inline
void
nVariable::SetHandle(Handle h)
{
    n_assert(INVALID_HANDLE != h);
    n_assert(INVALID_HANDLE == this->handle);

    this->handle = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::Handle
nVariable::GetHandle() const
{
    return this->handle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::SetInt(int val)
{
    n_assert(INT == this->type);
    this->intVal = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nVariable::GetInt() const
{
    n_assert(INT == this->type);
    return this->intVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::SetFloat(float val)
{
    n_assert(FLOAT == this->type);
    this->floatVal = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nVariable::GetFloat() const
{
    n_assert(FLOAT == this->type);
    return this->floatVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::SetVector(const float4& val)
{
    n_assert(VECTOR == this->type);
    this->vectorVal[0] = val[0];
    this->vectorVal[1] = val[1];
    this->vectorVal[2] = val[2];
    this->vectorVal[3] = val[3];
}

//------------------------------------------------------------------------------
/**
*/
inline
const float4&
nVariable::GetVector() const
{
    n_assert(VECTOR == this->type);
    return this->vectorVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::SetString(const char* str)
{
    n_assert(STRING == this->type);
    n_assert(str);
    if (this->stringVal)
    {
        n_free((void*) this->stringVal);
        this->stringVal = 0;
    }
    this->stringVal = n_strdup(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nVariable::GetString() const
{
    n_assert(STRING == this->type);
    return this->stringVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVariable::SetObject(void* ptr)
{
    n_assert(OBJECT == this->type);
    this->objectVal = ptr;
}

//------------------------------------------------------------------------------
/**
*/
inline
void*
nVariable::GetObject() const
{
    n_assert(OBJECT == this->type);
    return this->objectVal;
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(Handle h, int val) :
    handle(h),
    type(INT),
    intVal(val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(Handle h, float val) :
    handle(h),
    type(FLOAT),
    floatVal(val)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(Handle h, const float4& val) :
    handle(h),
    type(VECTOR)
{
    this->SetVector(val);
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(Handle h, const char* str) :
    handle(h),
    type(STRING)
{
    this->SetString(str);
}

//------------------------------------------------------------------------------
/**
*/
inline
nVariable::nVariable(Handle h, void* ptr) :
    handle(h),
    type(OBJECT),
    objectVal(ptr)
{
    // empty
}

//------------------------------------------------------------------------------
#endif

