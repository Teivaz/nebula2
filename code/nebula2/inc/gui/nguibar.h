#ifndef N_GUIBAR_H
#define N_GUIBAR_H
//-----------------------------------------------------------------------------
/**
    @class nGuiBar
    @ingroup Gui

    @brief Fillable GUI bars. May be filled from left to right or vice versa. Examples
    are Healthbars, Progressbars, etc.

    (C) 2005 Radon Labs GmbH
*/
#include "gui/nguiwidget.h"

//-----------------------------------------------------------------------------
class nGuiBar : public nGuiWidget
{
public:
    /// constructor
    nGuiBar();
    /// destructor
    virtual ~nGuiBar();

    enum FillMode
    {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop,
    };

    /// Set fillmode to `mode'. Default is `LeftToRight'.
    void SetFillMode(FillMode mode);
    /// Fillmode (either LeftToRight or RightToLeft).
    FillMode GetFillMode() const;

    /// Set fillstate in percent to `v' (values [0.0 .. 1.0]).
    void SetFillState(float v);
    /// Fillstate in percent (values [0.0 .. 1.0]).
    float GetFillState() const;

    /// Segment fillstate into `v' parts. For v == 0, fillstate is continuous.
    void SetFillStateSegments(int v);
    /// Number of parts fillstate is segmented.
    int GetFillStateSegments() const;
    /// Is fillstate segmented rather than continuous?
    bool IsFillStateSegmented() const;

    /// Rendering.
    virtual bool Render();

private:
    /// Fill state if segmented.
    float SegmentedFillState(float v) const;

    // Data
    FillMode fillMode;
    float fillState;
    int fillStateSegments;

};

//-----------------------------------------------------------------------------
inline
void
nGuiBar::SetFillMode(FillMode mode)
{
    this->fillMode = mode;
}
//-----------------------------------------------------------------------------
inline
nGuiBar::FillMode
nGuiBar::GetFillMode() const
{
    return this->fillMode;
}
//-----------------------------------------------------------------------------
inline
void
nGuiBar::SetFillState(float v)
{
    n_assert(0.0f <= v && v <= 1.0f);
    this->fillState = v;
}
//-----------------------------------------------------------------------------
inline
float
nGuiBar::GetFillState() const
{
    n_assert(0.0f <= this->fillState && this->fillState <= 1.0f);
    return this->fillState;
}
//-----------------------------------------------------------------------------
inline
void
nGuiBar::SetFillStateSegments(int v)
{
    n_assert(v >= 0);
    this->fillStateSegments = v;
}
//-----------------------------------------------------------------------------
inline
int
nGuiBar::GetFillStateSegments() const
{
    n_assert(this->fillStateSegments >= 0);
    return this->fillStateSegments;
}
//-----------------------------------------------------------------------------
inline
bool
nGuiBar::IsFillStateSegmented() const
{
    return this->fillStateSegments > 0;
}
//-----------------------------------------------------------------------------
#endif

