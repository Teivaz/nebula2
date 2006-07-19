#ifndef UI_PROGRESSBARWINDOW_H
#define UI_PROGRESSBARWINDOW_H
//------------------------------------------------------------------------------
/**
    @class UI::ProgressBarWindow
    
    Implements a progress bar window with its own render loop.
    
    (C) 2006 Radon Labs GmbH
*/
#include "ui/window.h"
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace UI
{
class ProgressBarWindow : public Window
{
    DeclareRtti;
    DeclareFactory(ProgressBarWindow);
public:
    /// constructor
    ProgressBarWindow();
    /// destructor
    virtual ~ProgressBarWindow();

    /// enable/disable the debug text output
    void SetDebugTextEnabled(bool b);
    /// get debug text output enable flag
    bool GetDebugTextEnabled() const;

    /// open the window (will attach the window to the UI server)
    virtual void Open();
    /// close the window (will remove the window from the UI server)
    virtual void Close();

    /// set the progress max value
    void SetMaxProgressValue(int v);
    /// get the progress max value
    int GetMaxProgressValue() const;
    /// advance the progress by some value
    void AdvanceProgress(int amount);
    
    /// set an optional text message
    void SetText(const nString& t);
    /// get text message
    const nString& GetText() const;

    /// do a full render loop (necessary for load/save screens)
    void Present();

private:
    /// setup the message of the day string from data:messageoftheday.txt
    void SetupMessageOfTheDay();

    Ptr<Graphics::CameraEntity> cameraEntity;
    int rangeMaxValue;
    int rangeCurValue;
    nString text;
    nString messageOfTheDay;
    bool debugTextEnabled;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ProgressBarWindow::SetDebugTextEnabled(bool b)
{
    this->debugTextEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ProgressBarWindow::GetDebugTextEnabled() const
{
    return this->debugTextEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ProgressBarWindow::SetMaxProgressValue(int v)
{
    n_assert(v > 0);
    this->rangeMaxValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ProgressBarWindow::GetMaxProgressValue() const
{
    return this->rangeMaxValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ProgressBarWindow::AdvanceProgress(int amount)
{
    this->rangeCurValue += amount;
    this->rangeCurValue = n_min(this->rangeCurValue, this->rangeMaxValue);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ProgressBarWindow::SetText(const nString& t)
{
    this->text = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
ProgressBarWindow::GetText() const
{
    return this->text;
}

}; // namespace UI
//------------------------------------------------------------------------------
#endif
    