#ifndef N_SHADERARG_H
#define N_SHADERARG_H
//------------------------------------------------------------------------------
/**
    @class nShaderArg
    @ingroup NebulaGraphicsSystem
    
    Encapsulates an argument for a shader parameter. This is similar 
    to an nArg, but only handles argument types that are relevant 
    for a shader.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

//------------------------------------------------------------------------------
class nShaderArg
{
public:
    /// data types
    enum Type
    {
        Void,
        Bool,
        Int,
        Float,
        Float4,
        Matrix44,
        Texture,
    };

    /// constructor
    nShaderArg();
    /// constructor with fixed type
    nShaderArg(Type t);
    /// destructor
    ~nShaderArg();
    /// equality operator
    bool operator==(const nShaderArg& rhs) const;
    /// assignment operator
    void operator=(const nShaderArg& rhs);
    /// set the data type
    void SetType(Type t);
    /// get data type
    Type GetType() const;
    /// set bool value
    void SetBool(bool val);
    /// get bool value
    bool GetBool() const;
    /// set int value
    void SetInt(int val);
    /// get int value
    int GetInt() const;
    /// set float value
    void SetFloat(float val);
    /// get float value
    float GetFloat() const;
    /// set float4 value
    void SetFloat4(const nFloat4& val);
    /// get float4 value
    const nFloat4& GetFloat4() const;
    /// set matrix value
    void SetMatrix44(const matrix44* val);
    /// get matrix value
    const matrix44* GetMatrix44() const;
    /// set texture
    void SetTexture(nTexture2* val);
    /// get texture
    nTexture2* GetTexture() const;

private:
    Type type;
    union
    {
        bool b;
        int i;
        float f;
        nTexture2* tex;
        nFloat4 f4;
        float m[4][4];
    };
};

//------------------------------------------------------------------------------
/**
*/
inline
nShaderArg::nShaderArg() :
    type(Void),
    tex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderArg::nShaderArg(Type t) :
    type(t),
    tex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderArg::~nShaderArg()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShaderArg::operator==(const nShaderArg& rhs) const
{
    if (this->type == rhs.type)
    {
        switch (this->type)
        {
            case Void:  
                return true;
            
            case Bool:
                return (this->b == rhs.b);

            case Int:
                return (this->i == rhs.i);

            case Float:
                return (this->f == rhs.f);

            case Float4:
                return ((this->f4.x == rhs.f4.x) &&
                        (this->f4.y == rhs.f4.y) &&
                        (this->f4.z == rhs.f4.z) &&
                        (this->f4.w == rhs.f4.w));

            case Matrix44:
                {
                    bool equal = true;
                    int i;
                    for (i = 0; i < 4; i++)
                    {
                        int j;
                        for (j = 0; j < 4; j++)
                        {
                            if (this->m[i][j] != rhs.m[i][j])
                            {
                                equal = false;
                            }
                        }
                    }
                    return equal;
                };

            case Texture:
                return (this->tex == rhs.tex);

            default:
                n_error("nShaderArg::operator==(): Invalid nShaderArg type!");
                break;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::operator=(const nShaderArg& rhs)
{
    this->type = rhs.type;
    switch (rhs.type)
    {
        case Void:
            break;

        case Bool:
            this->b = rhs.b;
            break;

        case Int:
            this->i = rhs.i;
            break;

        case Float:
            this->f = rhs.f;
            break;

        case Float4:
            this->f4 = rhs.f4;
            break;

        case Matrix44:
            memcpy(&(this->m), &(rhs.m), sizeof(this->m));
            break;

        case Texture:
            this->tex = rhs.tex;
            break;

        default:
            n_error("nShaderArg::operator=(): Invalid argument type!");
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetType(Type t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShaderArg::Type
nShaderArg::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetBool(bool val)
{
    this->type = Bool;
    this->b = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShaderArg::GetBool() const
{
    return this->b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetInt(int val)
{
    this->type = Int;
    this->i = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShaderArg::GetInt() const
{
    return this->i;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetFloat(float val)
{
    this->type = Float;
    this->f = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nShaderArg::GetFloat() const
{
    return this->f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetFloat4(const nFloat4& val)
{
    this->type = Float4;
    this->f4 = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nFloat4&
nShaderArg::GetFloat4() const
{
    return this->f4;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetMatrix44(const matrix44* val)
{
    n_assert(val);
    this->type = Matrix44;
    int i;
    for (i = 0; i < 4; i++)
    {
        int j;
        for (j = 0; j < 4; j++)
        {
            this->m[i][j] = val->m[i][j];
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44*
nShaderArg::GetMatrix44() const
{
    return (matrix44*) this->m;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShaderArg::SetTexture(nTexture2* val)
{
    this->type = Texture;
    this->tex = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTexture2*
nShaderArg::GetTexture() const
{
    return this->tex;
}

//------------------------------------------------------------------------------
#endif    
