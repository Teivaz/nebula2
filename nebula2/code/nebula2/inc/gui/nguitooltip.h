#ifndef N_GUITOOLTIP_H
#define N_GUITOOLTIP_H
//------------------------------------------------------------------------------
/**
    @class nGuiToolTip
    @ingroup NebulaGuiSystem
    @brief A tooltip widget with text which always positions itself
    relative to the mouse.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

class nInputServer;

//------------------------------------------------------------------------------
class nGuiToolTip : public nGuiTextLabel
{
public:
    /// constructor
    nGuiToolTip();
    /// destructor
    virtual ~nGuiToolTip();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);

private:
    /// make sure rectangle is within screen area
    void ClipRect(rectangle& r) const;

    nAutoRef<nInputServer> refInputServer;
};

//------------------------------------------------------------------------------
#endif

    
