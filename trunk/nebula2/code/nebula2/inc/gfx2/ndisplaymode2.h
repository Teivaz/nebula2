#ifndef N_DISPLAYMODE2_H
#define N_DISPLAYMODE2_H
//------------------------------------------------------------------------------
/**
    @class nDisplayMode2
    @ingroup NebulaGraphicsSystem

    Contains display mode parameters.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nDisplayMode2
{
public:
    /// display mode types
    enum Type
    {
        Windowed,       // windowed mode
        Fullscreen,     // fullscreen mode
        AlwaysOnTop,    // windowed, always on top
        ChildWindow,    // windowed, as child window
    };

    /// bit depths
    enum Bpp
    {
        Bpp16,
        Bpp32,
    };

    /// constructor
    nDisplayMode2();
    /// constructor
    nDisplayMode2(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync);
    /// set display mode
    void Set(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync);
    /// set x position
    void SetXPos(int x);
    /// get x position
    int GetXPos() const;
    /// set y position
    void SetYPos(int y);
    /// get y position
    int GetYPos() const;
    /// set display width
    void SetWidth(int w);
    /// get display width
    int GetWidth() const;
    /// set display height
    void SetHeight(int h);
    /// get display height
    int GetHeight() const;
    /// set display type
    void SetType(Type t);
    /// get display type
    Type GetType() const;
    /// set bit depth
    void SetBpp(Bpp depth);
    /// get bit depth
    Bpp GetBpp() const;
    /// set window title
    void SetWindowTitle(const char* t);
    /// get window title
    const char* GetWindowTitle() const;
    /// set vertical sync flag
    void SetVerticalSync(bool b);
    /// get vertical sync flag
    bool GetVerticalSync() const;
    /// set optional window icon resource
    void SetIcon(const char* resName);
    /// get optional window icon resource
    const char* GetIcon() const;
    /// make this mode compatible with host system dialog boxes
    void SetDialogBoxMode(bool b);
    /// get dialog box mode
    bool GetDialogBoxMode() const;
    /// convert type to string
    static const char* TypeToString(Type t);
    /// convert string to type
    static Type StringToType(const char* str);

private:
    nString windowTitle;
    nString iconName;
    Type type;
    ushort xpos;
    ushort ypos;
    ushort width;
    ushort height;
    Bpp bitsPerPixel;
    bool verticalSync;
    bool dialogBoxMode;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2() :
    windowTitle("Nebula2 Viewer"),
    type(Windowed),
    xpos(0),
    ypos(0),
    width(640),
    height(480),
    bitsPerPixel(Bpp32),
    verticalSync(true),
    dialogBoxMode(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync) :
    windowTitle(winTitle),
    type(t),
    xpos(x),
    ypos(y),
    width(w),
    height(h),
    bitsPerPixel(Bpp32),
    verticalSync(vSync),
    dialogBoxMode(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::Set(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync)
{
    this->windowTitle = winTitle;
    this->type   = t;
    this->xpos   = x;
    this->ypos   = y;
    this->width  = w;
    this->height = h;
    this->verticalSync = vSync;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetXPos(int x)
{
    this->xpos = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetXPos() const
{
    return this->xpos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetYPos(int y)
{
    this->ypos = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetYPos() const
{
    return this->ypos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetWidth(int w)
{
    this->width = w;
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
void
nDisplayMode2::SetHeight(int h)
{
    this->height = h;
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
nDisplayMode2::SetType(Type t)
{
    this->type = t;
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
void
nDisplayMode2::SetWindowTitle(const char* t)
{
    this->windowTitle = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDisplayMode2::GetWindowTitle() const
{
    return this->windowTitle.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetVerticalSync(bool b)
{
    this->verticalSync = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nDisplayMode2::GetVerticalSync() const
{
    return this->verticalSync;
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
        case Windowed:      return "windowed";
        case AlwaysOnTop:   return "alwaysontop";
        case ChildWindow:   return "childwindow";
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
        return Windowed;
    }
    else if (0 == strcmp(str, "alwaysontop"))
    {
        return AlwaysOnTop;
    }
    else if (0 == strcmp(str, "childwindow"))
    {
        return ChildWindow;
    }
    else
    { 
        return Fullscreen;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetIcon(const char* resName)
{
    this->iconName = resName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nDisplayMode2::GetIcon() const
{
    return this->iconName.IsEmpty() ? 0 : this->iconName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetDialogBoxMode(bool b)
{
    this->dialogBoxMode = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nDisplayMode2::GetDialogBoxMode() const
{
    return this->dialogBoxMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetBpp(Bpp depth)
{
    this->bitsPerPixel = depth;
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::Bpp
nDisplayMode2::GetBpp() const
{
    return this->bitsPerPixel;
}

//------------------------------------------------------------------------------
#endif
