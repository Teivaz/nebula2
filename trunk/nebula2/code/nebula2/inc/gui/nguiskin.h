#ifndef N_GUISKIN_H
#define N_GUISKIN_H
//------------------------------------------------------------------------------
/**
    A GUI skin object holds a user-defined table of gui resources which
    are used by the GUI widgets to render themselves. Each gui resource
    describes a rectangular texture area. That way it is possible to 
    distribute the GUI brushes over any number of textures.
    
    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/narray.h"
#include "mathlib/rectangle.h"

class nGuiResource;

//------------------------------------------------------------------------------
class nGuiSkin : public nRoot
{
public:
    /// constructor
    nGuiSkin();
    /// destructor
    virtual ~nGuiSkin();
    /// set texture prefix
    void SetTexturePrefix(const char* prefix);
    /// get texture prefix
    const char* GetTexturePrefix() const;
    /// set texture postfix 
    void SetTexturePostfix(const char* postfix);
    /// get texture postfix
    const char* GetTexturePostfix() const;
    /// set the active window color
    void SetActiveWindowColor(const vector4& c);
    /// get the active window color
    const vector4& GetActiveWindowColor() const;
    /// set the innactive window color
    void SetInactiveWindowColor(const vector4& c);
    /// get the inactive window color
    const vector4& GetInactiveWindowColor() const;
    /// set button text color
    void SetButtonTextColor(const vector4& c);
    /// get button text color
    const vector4& GetButtonTextColor() const;
    /// set title text color
    void SetTitleTextColor(const vector4& c);
    /// get title text color
    const vector4& GetTitleTextColor() const;
    /// set label text color
    void SetLabelTextColor(const vector4& c);
    /// get label text color
    const vector4& GetLabelTextColor() const;
    /// set window broder size
    void SetWindowBorder(const rectangle& v);
    /// get window border size
    const rectangle& GetWindowBorder() const;
    /// begin adding brushes
    void BeginBrushes();
    /// add a skin brush
    void AddBrush(const char* name, const char* tex, const vector2& uvPos, const vector2& uvSize, const vector4& color);
    /// end adding brushes
    void EndBrushes();
    /// lookup gui resource for a brush
    nGuiResource* FindBrush(const char* name);

private:
    nArray<nGuiResource> brushes;
    nString texPrefix;
    nString texPostfix;
    vector4 activeWindowColor;
    vector4 inactiveWindowColor;
    vector4 titleTextColor;
    vector4 buttonTextColor;
    vector4 labelTextColor;
    rectangle windowBorder;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetLabelTextColor(const vector4& c)
{
    this->labelTextColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetLabelTextColor() const
{
    return this->labelTextColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetTitleTextColor(const vector4& c)
{
    this->titleTextColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetTitleTextColor() const
{
    return this->titleTextColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetButtonTextColor(const vector4& c)
{
    this->buttonTextColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetButtonTextColor() const
{
    return this->buttonTextColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetWindowBorder(const rectangle& r)
{
    this->windowBorder = r;
}

//------------------------------------------------------------------------------
/**
*/
inline
const rectangle&
nGuiSkin::GetWindowBorder() const
{
    return this->windowBorder;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetTexturePrefix(const char* prefix)
{
    this->texPrefix = prefix;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiSkin::GetTexturePrefix() const
{
    return this->texPrefix.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetTexturePostfix(const char* postfix)
{
    this->texPostfix = postfix;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiSkin::GetTexturePostfix() const
{
    return this->texPostfix.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetActiveWindowColor(const vector4& c)
{
    this->activeWindowColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetActiveWindowColor() const
{
    return this->activeWindowColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetInactiveWindowColor(const vector4& c)
{
    this->inactiveWindowColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetInactiveWindowColor() const
{
    return this->inactiveWindowColor;
}

//------------------------------------------------------------------------------
#endif
    
    
