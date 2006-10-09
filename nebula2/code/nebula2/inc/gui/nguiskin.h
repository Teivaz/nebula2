#ifndef N_GUISKIN_H
#define N_GUISKIN_H
//------------------------------------------------------------------------------
/**
    @class nGuiSkin
    @ingroup Gui
    @brief A GUI skin object holds an user-defined table of gui resources which
    are used by the GUI widgets to render themselves.

    Each gui resource describes a rectangular texture area. That way it is
    possible to distribute the GUI brushes over any number of textures.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "util/narray.h"
#include "mathlib/rectangle.h"
#include "util/nfixedarray.h"

class nGuiResource;
class nSound3;

//------------------------------------------------------------------------------
class nGuiSkin : public nRoot
{
public:
    /// constructor
    nGuiSkin();
    /// destructor
    virtual ~nGuiSkin();
    /// unload all gui resources which have not been rendered last frame
    void UnloadUntouchedGuiResources();
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
    /// set the default text color
    void SetTextColor(const vector4& c);
    /// get the default text color
    const vector4& GetTextColor() const;
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
    /// set text entry text color
    void SetEntryTextColor(const vector4& c);
    /// get text entry text color
    const vector4& GetEntryTextColor() const;
    /// set menu entry text color
    void SetMenuTextColor(const vector4& c);
    /// get menu entry text color
    const vector4& GetMenuTextColor() const;
    /// set window broder size
    void SetWindowBorder(const rectangle& v);
    /// get window border size
    const rectangle& GetWindowBorder() const;
    /// set window font
    void SetWindowFont(const char* fnt);
    /// get window font
    const char* GetWindowFont() const;
    /// set button font
    void SetButtonFont(const char* fnt);
    /// get button font
    const char* GetButtonFont() const;
    /// set label font
    void SetLabelFont(const char* fnt);
    /// get label font
    const char* GetLabelFont() const;
    /// begin adding brushes
    void BeginBrushes();
    /// add a skin brush
    void AddBrush(const char* name, const char* tex, const vector2& uvPos, const vector2& uvSize, const vector4& color);
    /// Add dynamic brush with name `name'.
    void AddDynamicBrush(const char* name, int width, int height);
    /// end adding brushes
    void EndBrushes();
    /// lookup gui resource for a brush
    nGuiResource* FindBrush(const char* name);
    /// add a sound object
    void AddSoundObject(const char* name, const char* filename);
    /// get sound object
    nSound3* GetSoundObject(const char* name);
    /// get sound filename associated with the name
    const char* GetSound(const char* name) const;
    /// set volume for sounds objects
    void SetSoundVolume(const char* name, float volume);
    /// get volume for sound object
    float GetSoundVolume(const char* name);

private:
    /// initialize a brush
    void ValidateBrush(nGuiResource* res);

    nArray<nGuiResource> brushes;
    nString texPrefix;
    nString texPostfix;
    vector4 activeWindowColor;
    vector4 inactiveWindowColor;
    vector4 titleTextColor;
    vector4 buttonTextColor;
    vector4 labelTextColor;
    vector4 entryTextColor;
    vector4 textColor;
    vector4 menuTextColor;
    rectangle windowBorder;

    nString windowFont;
    nString buttonFont;
    nString labelFont;

    nArray<nString> soundNames;
    nArray<nRef<nSound3> > sounds;
};


//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetEntryTextColor(const vector4& c)
{
    this->entryTextColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetEntryTextColor() const
{
    return this->entryTextColor;
}

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
nGuiSkin::SetTextColor(const vector4& c)
{
    this->textColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetTextColor() const
{
    return this->textColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetMenuTextColor(const vector4& c)
{
    this->menuTextColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiSkin::GetMenuTextColor() const
{
    return this->menuTextColor;
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
/**
*/
inline
void
nGuiSkin::SetWindowFont(const char* fnt)
{
    n_assert(fnt);
    this->windowFont = fnt;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiSkin::GetWindowFont() const
{
    return this->windowFont.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetButtonFont(const char* fnt)
{
    n_assert(fnt);
    this->buttonFont = fnt;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiSkin::GetButtonFont() const
{
    return this->buttonFont.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSkin::SetLabelFont(const char* fnt)
{
    n_assert(fnt);
    this->labelFont = fnt;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiSkin::GetLabelFont() const
{
    return this->labelFont.Get();
}

//------------------------------------------------------------------------------
#endif

