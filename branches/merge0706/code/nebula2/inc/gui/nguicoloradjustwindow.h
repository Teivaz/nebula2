#ifndef N_GUICOLORADJUSTWINDOW_H
#define N_GUICOLORADJUSTWINDOW_H
//------------------------------------------------------------------------------
/**
    @class nGuiColorAdjustWindow
    @ingroup Gui

    @brief To select a rgb-color.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiclientwindow.h"
#include "gui/nguicoloradjust.h"

//------------------------------------------------------------------------------
class nGuiColorAdjustWindow : public nGuiClientWindow
{
public:
    /// constructor
    nGuiColorAdjustWindow();
    /// destructor
    virtual ~nGuiColorAdjustWindow();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called when event is emitted
    virtual void OnEvent(const nGuiEvent& event);
    /// set color
    void SetColor(const vector4& color);
    /// get color
    const vector4& GetColor() const;

private:
    nRef<nGuiColorAdjust> refColorAdjust;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiColorAdjustWindow::SetColor(const vector4& color)
{
    this->refColorAdjust->SetColor(color);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiColorAdjustWindow::GetColor() const
{
    return this->refColorAdjust->GetColor();
}

#endif
