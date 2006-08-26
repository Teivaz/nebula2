#ifndef N_GUITICKERWIDGET_H
#define N_GUITICKERWIDGET_H
//------------------------------------------------------------------------------
/**
    @class nGuiTickerWidget
    @ingroup Gui

    @brief Ticker widget (for Genius). Not very useful for the general case.

    WARNING: becuase of a general bug in the GUI system the ticker must be placed
    directly under the top window, or the right border will be false.

    A text label that moves from left to right.

    @verbatim
                |-----------------|
    |----------|| Parent Widget   ||------------|
    | End Pos. ||           <-----|| Start Pos. |
    |----------||-----------------||------------|
                Î                  Î
                |                  Text will be clipped to right border.
                Must be left screen border!
    @endverbatim

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguitextlabel.h"
#include "kernel/ntimeserver.h"

//------------------------------------------------------------------------------
class nGuiTickerWidget : public nGuiTextLabel
{
public:
    /// constructor
    nGuiTickerWidget();
    /// destructor
    virtual ~nGuiTickerWidget();
    /// render the widget
    virtual bool Render();
    /// set the text as string
    virtual void SetText(const char* text);
    /// set how many seconds a char needs to move over the full screen
    void SetScrollSpeed(float scrollSpeed);
    /// set in how many seconds a char of the text move over the full screen
    float GetScrollSpeed() const;
    /// restart
    void Restart();

    /// has finished the display of the last message
    bool IsDone() const;

protected:
    void RenderText();

    float scrollSpeed;
    nAutoRef<nTimeServer> refTimeServer;
    bool timeIsInvalid;
    nTime startTime;
    bool textIsInvalid;
    bool isDone;
    vector2 textExtend;
};

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiTickerWidget::IsDone() const
{
    return this->isDone;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTickerWidget::SetText(const char* text)
{
    this->textIsInvalid = true;
    nGuiTextLabel::SetText(text);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTickerWidget::SetScrollSpeed(float speed)
{
    n_assert(speed > 0.0f);
    this->scrollSpeed = speed;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiTickerWidget::GetScrollSpeed() const
{
    return this->scrollSpeed;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiTickerWidget::Restart()
{
    this->isDone = false;
    this->timeIsInvalid = false;
    this->startTime = this->refTimeServer->GetTime();
}
//------------------------------------------------------------------------------
#endif

