#ifndef N_FONT2_H
#define N_FONT2_H
//------------------------------------------------------------------------------
/**
    @class nFont2
    @ingroup Gfx2

    A font resource class.

    (C) 2003 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "gfx2/nfontdesc.h"

//------------------------------------------------------------------------------
class nFont2 : public nResource
{
public:
    /// rendering flags (apply to nGfxServer2::DrawText())
    enum RenderFlags
    {
        Bottom     = (1<<0),    // bottom-align text
        Top        = (1<<1),    // top-align text
        Center     = (1<<2),    // centers text horizontally
        Left       = (1<<3),    // align text to the left
        Right      = (1<<4),    // align text to the right
        VCenter    = (1<<5),    // center text vertically
        NoClip     = (1<<6),    // don't clip text (faster)
        ExpandTabs = (1<<7),    // expand tabs, doh
        WordBreak  = (1<<8),    // break words at end of line
    };

    /// constructor
    nFont2();
    /// destructor
    virtual ~nFont2();
    /// set font description
    void SetFontDesc(const nFontDesc& desc);
    /// get font description
    const nFontDesc& GetFontDesc() const;

    /// convert render flag string to enum
    static RenderFlags StringToRenderFlag(const char* str);
    /// convert render flag enum to string
    static const char* RenderFalgToString(RenderFlags renderFlag);

protected:
    nFontDesc fontDesc;
};

//------------------------------------------------------------------------------
/**
    Convert the render flag to its corresponded string.
*/
inline
const char*
nFont2::RenderFalgToString(nFont2::RenderFlags renderFlag)
{
    switch (renderFlag)
    {
        case nFont2::Bottom:            return "Bottom";
        case nFont2::Top:               return "Top";
        case nFont2::Center:            return "Center";
        case nFont2::Left:              return "Left";
        case nFont2::Right:             return "Right";
        case nFont2::VCenter:           return "VCenter";
        case nFont2::NoClip:            return "NoClip";
        case nFont2::ExpandTabs:        return "ExpandTabs";
        case nFont2::WordBreak:         return "WordBreak";
        default: n_error("nFont2::RenderFalgToString(): invalid render flag!");
    }
}


//------------------------------------------------------------------------------
/**
    Convert the given string to its corresponded render flag.
*/
inline
nFont2::RenderFlags
nFont2::StringToRenderFlag(const char* str)
{
    n_assert(str);
    if (strcmp(str, "Bottom") == 0)     return Bottom;
    if (strcmp(str, "Top") == 0)        return Top;
    if (strcmp(str, "Center") == 0)     return Center;
    if (strcmp(str, "Left") == 0)       return Left;
    if (strcmp(str, "Right") == 0)      return Right;
    if (strcmp(str, "VCenter") == 0)    return VCenter;
    if (strcmp(str, "NoClip") == 0)     return NoClip;
    if (strcmp(str, "ExpandTabs") == 0) return ExpandTabs;
    if (strcmp(str, "WorkBreak") == 0)  return WordBreak;
    return (RenderFlags)0;
}

//------------------------------------------------------------------------------
#endif
