#ifndef N_FONT2_H
#define N_FONT2_H
//------------------------------------------------------------------------------
/**
    @class nFont2

    A font resource class.
    
    (C) 2003 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "gfx2/nfontdesc.h"

//------------------------------------------------------------------------------
class nFont2 : public nResource
{
public:
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
