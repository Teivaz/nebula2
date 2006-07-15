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

protected:
    nFontDesc fontDesc;
};

//------------------------------------------------------------------------------
#endif
