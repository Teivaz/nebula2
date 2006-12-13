#ifndef N_DISPLAYMODE2_H
#define N_DISPLAYMODE2_H
//------------------------------------------------------------------------------
/**
    @class nDisplayMode2
    @ingroup Gfx2

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
        Fullscreen,     // full-screen mode
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
    nDisplayMode2(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync, bool dialogBoxMode, const char* iconResName);
    /// set display mode
    void Set(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync, bool dialogBoxMode, const char* iconResName);
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
    void SetWindowTitle(const nString& t);
    /// get window title
    const nString& GetWindowTitle() const;
    /// set vertical sync flag
    void SetVerticalSync(bool b);
    /// get vertical sync flag
    bool GetVerticalSync() const;
    /// set optional window icon resource
    void SetIcon(const nString& resName);
    /// get optional window icon resource
    const nString& GetIcon() const;
    /// make this mode compatible with host system dialog boxes
    void SetDialogBoxMode(bool b);
    /// get dialog box mode
    bool GetDialogBoxMode() const;
    /// set antialiasing mode
    void SetAntiAliasSamples(int s);
    /// get antialiasing mode
    int GetAntiAliasSamples() const;
    /// convert type to string
    static const char* TypeToString(Type t);
    /// convert string to type
    static Type StringToType(const char* str);

private:
    nString windowTitle;
    nString iconName;
    Type type;
    int antiAliasSamples;
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
    windowTitle("Nebula2 Window"),
    iconName("Icon"),
    type(Windowed),
    antiAliasSamples(0),
    xpos(0),
    ypos(0),
    width(640),
    height(480),
    bitsPerPixel(Bpp32),
    verticalSync(false),
    dialogBoxMode(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDisplayMode2::nDisplayMode2(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync, bool dialogBoxMode, const char* iconResName) :
    windowTitle(winTitle),
    type(t),
    xpos(x),
    ypos(y),
    width(w),
    height(h),
    bitsPerPixel(Bpp32),
    verticalSync(vSync),
    dialogBoxMode(dialogBoxMode),
    iconName(iconResName)
{
    n_assert(w > 0);
    n_assert(h > 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::Set(const char* winTitle, Type t, ushort x, ushort y, ushort w, ushort h, bool vSync, bool dialogBoxMode, const char* iconResName)
{
    n_assert(w > 0);
    n_assert(h > 0);

    this->windowTitle = winTitle;
    this->type   = t;
    this->xpos   = x;
    this->ypos   = y;
    this->width  = w;
    this->height = h;
    this->verticalSync = vSync;
    this->dialogBoxMode = dialogBoxMode;
    this->iconName = iconResName;
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
    n_assert(w > 0);
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetWidth() const
{
    n_assert(this->width > 0);
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetHeight(int h)
{
    n_assert(h > 0);
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetHeight() const
{
    n_assert(this->height > 0);
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
nDisplayMode2::SetWindowTitle(const nString& t)
{
    this->windowTitle = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nDisplayMode2::GetWindowTitle() const
{
    return this->windowTitle;
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
        default:            return "fullscreen";
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
    if (0 == strcmp(str, "windowed")) return Windowed;
    if (0 == strcmp(str, "alwaysontop")) return AlwaysOnTop;
    if (0 == strcmp(str, "childwindow")) return ChildWindow;
    return Fullscreen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nDisplayMode2::SetIcon(const nString& resName)
{
    this->iconName = resName;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nDisplayMode2::GetIcon() const
{
    return this->iconName;
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
/**
*/
inline
void
nDisplayMode2::SetAntiAliasSamples(int s)
{
    n_assert((s >= 0) && (s != 1) && (s <= 16));
    this->antiAliasSamples = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nDisplayMode2::GetAntiAliasSamples() const
{
    return this->antiAliasSamples;
}

//------------------------------------------------------------------------------
#endif
