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
    static RenderFlags StringToRenderFlag(const nString& str);
    /// convert render flag enum to string
    static nString RenderFalgToString(RenderFlags renderFlag);

protected:
    nFontDesc fontDesc;
};

//------------------------------------------------------------------------------
/**
    Convert the render flag to its corresponded string.
*/
inline
nString
nFont2::RenderFalgToString(nFont2::RenderFlags renderFlag)
{
    switch (renderFlag)
    {
        case nFont2::Bottom:         return nString("Bottom"); break;
        case nFont2::Top:            return nString("Top"); break;
        case nFont2::Center:         return nString("Center"); break;
        case nFont2::Left:           return nString("Left"); break;
        case nFont2::Right:          return nString("Right"); break;
        case nFont2::VCenter:        return nString("VCenter"); break;
        case nFont2::NoClip:         return nString("NoClip"); break;
        case nFont2::ExpandTabs:     return nString("ExpandTabs"); break;
        case nFont2::WordBreak:      return nString("WordBreak"); break;
        default:
            n_error("nFont2::RenderFalgToString(): invalid render flag!");
    }
}


//------------------------------------------------------------------------------
/**
    Convert the given string to its corresponded render flag.
*/
inline
nFont2::RenderFlags
nFont2::StringToRenderFlag(const nString& str)
{
    n_assert(!str.IsEmpty());
    if ("Bottom" == str)
    {
        return Bottom;
    }
    else if ("Top" == str)
    {
        return Top;
    }
    else if ("Center" == str)
    {
        return Center;
    }
    else if ("Left" == str)
    {
        return Left;
    }
    else if ("Right" == str)
    {
        return Right;
    }
    else if ("VCenter" == str)
    {
        return VCenter;
    }
    else if ("NoClip" == str)
    {
        return NoClip;
    }
    else if ("ExpandTabs" == str)
    {
        return ExpandTabs;
    }
    else if ("WordBreak" == str)
    {
        return WordBreak;
    }
    else
    {
        return (RenderFlags)0;
    }
}

//------------------------------------------------------------------------------
#endif
