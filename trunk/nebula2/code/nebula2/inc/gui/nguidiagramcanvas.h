#ifndef N_GUIDIAGRAMCANVAS_H
#define N_GUIDIAGRAMCANVAS_H

#include "gui/nguicanvas.h"

class nGuiDiagramCanvas : public nGuiCanvas
{
public:

    struct Border
    {
        float top;
        float bottom;
        float left;
        float right;
    };

    /// Constructor
    nGuiDiagramCanvas();
    /// Destructor
    ~nGuiDiagramCanvas();
    /// Called when the widget becomes visible
    virtual void OnShow();
    /// Called every frame
    virtual void OnFrame();
    /// Called when the widget becomes invisible
    virtual void OnHide();
    /// Render canvas
    virtual bool Render();

    /// Set number of horizontal axis-markers
    void SetNumHMarkers(int num);
    /// Get number of horizontal axis-markers
    int GetNumHMarkers() const;
    /// Set number of vertical axis-markers
    void SetNumVMarkers(int num);
    /// Get number of vertical axis-markers
    int GetNumVMarkers() const;
    /// Set border between canvas-edge and axis
    void SetBorder(float top, float bottom, float left, float right);
    /// Get border between canvas-edge and axis
    nGuiDiagramCanvas::Border GetBorder() const;
    /// Set color of the diagram-axis
    void SetAxisColor(vector4 col);
    /// Get color of the diagram-axis
    vector4 GetAxisColor() const;
    /// Set axis font
    void SetAxisFont(const char* font);
    /// Get axis fong
    const char* GetAxisFont() const;
    /// Set X-Axis label
    void SetXAxisLabel(const char* xlabel);
    /// Get X-Axis label
    const char* GetXAxisLabel() const;
    /// Set Y-Axis label
    void SetYAxisLabel(const char* ylabel);
    /// Get Y-Axis label
    const char* GetYAxisLabel() const;
    /// Set max. X-Axis value
    void SetMaxXAxisValue(const int val);
    /// Get max. X-Axis value
    int GetMaxXAxisValue() const;
    /// Set max. Y-Axis value
    void SetMaxYAxisValue(const int val);
    /// Get max. Y-Axis value
    int GetMaxYAxisValue() const;

protected:
    // empty

private:
    Border border;
    vector4 axisColor;
    nString axisFont;
    nString xLabel;
    nString yLabel;
    int numHMarkers;
    int numVMarkers;
    int maxXValue;
    int maxYValue;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetXAxisLabel(const char* xlabel)
{
    this->xLabel = xlabel;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiDiagramCanvas::GetXAxisLabel() const
{
    return this->xLabel.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetYAxisLabel(const char* ylabel)
{
    this->yLabel = ylabel;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiDiagramCanvas::GetYAxisLabel() const
{
    return this->yLabel.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMaxXAxisValue(const int val)
{
    this->maxXValue = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiDiagramCanvas::GetMaxXAxisValue() const
{
    return this->maxXValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMaxYAxisValue(const int val)
{
    this->maxYValue = val;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiDiagramCanvas::GetMaxYAxisValue() const
{
    return this->maxYValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetAxisFont(const char* font)
{
    this->axisFont=font;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiDiagramCanvas::GetAxisFont() const
{
    return this->axisFont.Get();
}
    
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetNumHMarkers(int num)
{
    this->numHMarkers = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiDiagramCanvas::GetNumHMarkers() const
{
    return this->numHMarkers;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetNumVMarkers(int num)
{
    this->numVMarkers = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiDiagramCanvas::GetNumVMarkers() const
{
    return this->numVMarkers;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetBorder(float top, float bottom, float left, float right)
{
    this->border.top = top;
    this->border.bottom = bottom;
    this->border.left = left;
    this->border.right = right;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiDiagramCanvas::Border
nGuiDiagramCanvas::GetBorder() const
{
    return this->border;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetAxisColor(vector4 col)
{
    this->axisColor = col;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nGuiDiagramCanvas::GetAxisColor() const
{
    return this->axisColor;
}



#endif
