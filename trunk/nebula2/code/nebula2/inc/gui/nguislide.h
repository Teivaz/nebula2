#ifndef N_GUISLIDE_H
#define N_GUISLIDE_H

//-----------------------------------------------------------------------------
/**
    @class nGuiSlide
    @ingroup Gui

    @brief A slide class for use with the nGuiSlideShow widget

    (C) 2005 RadonLabs GmbH
*/

#include "gui/nguilabel.h"

class nGuiSlide : public nGuiLabel
{
public:
    /// Constructor
    nGuiSlide();
    /// Destructor
    ~nGuiSlide();
    /// Sets the render color
    void SetRenderColor(vector4 color);
    /// Check if the slide has a picture
    bool HasPicture();

protected:
    // empty

private:
    // empty
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiSlide::HasPicture()
{
    if(this->defaultBrush.GetName().IsEmpty())
    {
        return false;
    }

    return true;
}

#endif
