#ifndef N_DISPLAYMODE2_H
#define N_DISPLAYMODE2_H
//------------------------------------------------------------------------------
/**
    @class nDisplayMode2
    @ingroup NebulaGraphicsSystem

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
        CHILDWINDOWED
    };

    /// constructor
    nDisplayMode2();
    /// constructor
    nDisplayMode2(Type t, ushort w, ushort h, bool vs);
    /// set display mode
    void Set(Type t, ushort w, ushort h, bool vs);
    /// get display width
    int GetWidth() const;
    /// get display mode height
    int GetHeight() const;
    /// set display width
    void SetWidth(ushort w);
    /// set display mode height
    void SetHeight(ushort h);   
    /// get display mode type
    Type GetType() const;
    /// return vsync value
    bool GetVerticalSync() const;
    /// convert type to string
    static const char* TypeToString(Type t);
    /// convert string to type
    static Type StringToType(const char* str);

private:
    Type type;
    ushort width;
    ushort height;
    int parentHWnd;
    bool vSync;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2() :
    type(WINDOWED),
    width(640),
    height(480),
    vSync(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2(Type t, ushort w, ushort h, bool vs) :
    type(t),
    width(w),
    height(h),
    vSync(vs)
{
    // empty
}

//------------------------------------------------------------------------------
/**

*/
inline
void
nDisplayMode2::Set(Type t, ushort w, ushort h, bool vs)
{
    this->type   = t;
    this->width  = w;
    this->height = h;
    this->vSync = vs;
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
void
nDisplayMode2::SetWidth(ushort w) 
{
   this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetHeight(ushort h)
{
    this->height = h;
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
bool nDisplayMode2::GetVerticalSync() const
{
    return this->vSync;
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
        case CHILDWINDOWED: return "child";
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
    else if (0 == strcmp(str, "child"))
    {
        return CHILDWINDOWED;
    }
    else
    { 
        return FULLSCREEN;
    }
}

//------------------------------------------------------------------------------
#endif
