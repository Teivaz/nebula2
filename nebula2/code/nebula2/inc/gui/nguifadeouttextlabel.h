#ifndef N_GUIFADEOUTTEXTLABEL_H
#define N_GUIFADEOUTTEXTLABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiFadeOutTextLabel
    @ingroup Gui

    @brief FadeOut the Text.

    (C) 2005 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiFadeOutTextLabel : public nGuiTextLabel
{
public:
    /// constructor
    nGuiFadeOutTextLabel();
    /// destructor
    virtual ~nGuiFadeOutTextLabel();
    /// set the text as string
    virtual void SetText(const char* text);
    /// set the full faded in text color
    virtual void SetColor(const vector4& c);
    /// set time out - set to 0.0 for no timeout
    void SetTimeOut(nTime t);
    /// get time out
    nTime GetTimeOut() const;

protected:
    /// render the label text
    virtual void RenderText(bool pressed);

    vector4 fullColor;
    nTime fadeOutTime;
    nTime startTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFadeOutTextLabel::SetTimeOut(nTime t)
{
    n_assert(t >= 0.f);
    this->fadeOutTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nGuiFadeOutTextLabel::GetTimeOut() const
{
    return this->fadeOutTime;
}

//------------------------------------------------------------------------------
#endif


