#ifndef N_DISPLAYMODE2_H
#define N_DISPLAYMODE2_H
//------------------------------------------------------------------------------
/**
    Contains display mode parameters.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

//------------------------------------------------------------------------------
class nDisplayMode2
{
public:
    /// display mode types
    enum Type
    {
        WINDOWED,
        FULLSCREEN,
    };

    /// constructor
    nDisplayMode2();
    /// constructor
    nDisplayMode2(Type t, ushort w, ushort h);
    /// set display mode
    void Set(Type t, ushort w, ushort h);
    /// get display width
    int GetWidth() const;
    /// get display mode height
    int GetHeight() const;
    /// get display mode type
    Type GetType() const;
    /// convert type to string
    static const char* TypeToString(Type t);
    /// convert string to type
    static Type StringToType(const char* str);

private:
    Type type;
    ushort width;
    ushort height;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2() :
    type(WINDOWED),
    width(640),
    height(480)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2(Type t, ushort w, ushort h) :
    type(t),
    width(w),
    height(h)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::Set(Type t, ushort w, ushort h)
{
    this->type   = t;
    this->width  = w;
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::Type
nDisplayMode2::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDisplayMode2::TypeToString(Type t)
{
    switch (t)
    {
        case WINDOWED:   return "windowed";
        default:         return "fullscreen";
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::Type
nDisplayMode2::StringToType(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "windowed"))
    {
        return WINDOWED;
    }
    else
    {
        return FULLSCREEN;
    }
}

//------------------------------------------------------------------------------
#endif
