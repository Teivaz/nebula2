#ifndef N_GUIDIAGRAMCANVAS_H
#define N_GUIDIAGRAMCANVAS_H
//------------------------------------------------------------------------------
/**
    @class nGuiDiagramCanvas
    @ingroup Gui

    @brief A GuiWidget which creates a diagram canvas.

    (C) 2004 RadonLabs GmbH
*/    

#include "gui/nguiformlayout.h"

class nGuiCanvas;
class nGuiTextLabel;

class nGuiDiagramCanvas : public nGuiFormLayout
{
public:
    /// Constructor
    nGuiDiagramCanvas();
    /// Destructor
    virtual ~nGuiDiagramCanvas();
    /// Called when widget becomes visible
    virtual void OnShow();
    /// Called when the widget becomes invisible
    virtual void OnHide();

    /// Get the Canvas member
    nGuiCanvas* GetCanvas() const;

    /// Set if the diagram-axis have labels
    void SetHasAxisLabels(bool b);
    /// Set if the x-axis markers have labels
    void SetHasXAxisLabels(bool b);
    /// Set if the y-axis markers have labels
    void SetHasYAxisLabels(bool b);
    /// Do the axis have labels
    bool HasAxisLabels() const;
    /// Does the x-axis have labels
    bool HasXAxisLabels() const;
    /// Does the y-axis have labels
    bool HasYAxisLabels() const;
    /// Set axis labels
    void SetAxisLabels(const char* x, const char* y);
    /// Set axis label font
    void SetAxisFont(const char* font);
    /// Set axis textcolor
    void SetAxisTextColor(vector4 col);
    /// Set min. x-axis value
    void SetMinXAxisValue(int val);
    /// Set max. x-axis value
    void SetMaxXAxisValue(int val);
    /// Set min. y-axis value
    void SetMinYAxisValue(int val);
    /// Set max. y-axis value
    void SetMaxYAxisValue(int val);
    /// Get max. x-axis value
    int GetMaxXAxisValue() const;
    /// Get min. x-axis value
    int GetMinXAxisValue() const;
    /// Get max y-axis value
    int GetMaxYAxisValue() const;
    /// Get min y-axis value
    int GetMinYAxisValue() const;
    /// Set number of X-axis markers
    void SetNumXMarkers(int num);
    /// Set number of Y-axis markers
    void SetNumYMarkers(int num);

    /// Set min. y-axis text
    void SetMinYAxisText(const nString& text);
    /// Get min y-axis text
    const nString& GetMinYAxisText() const;
    /// Set max. y-axis text
    void SetMaxYAxisText(const nString& text);
    /// Get max y-axis text
    const nString& GetMaxYAxisText() const;

    /// Set min. x-axis text
    void SetMinXAxisText(const nString& text);
    /// Get min x-axis text
    const nString& GetMinXAxisText() const;
    /// Set max. x-axis text
    void SetMaxXAxisText(const nString& text);
    /// Get max x-axis text
    const nString& GetMaxXAxisText() const;

    /// Optional Diagram header
    void SetDiagramHeader(const char* header);
    /// Set if the diagram has a header
    void SetHasHeader(bool b);
    /// Does the diagram have a header?
    bool HasHeader() const;


protected:

    enum offset
    {
        Top = 0,
        Bottom = 1,
        Left = 2,
        Right = 3,
        ArrowLength = 4,

        numOffsets = 5
    };

    float curveOffset[numOffsets];

private:

    enum labels
    {
        XLabel = 0,
        YLabel = 1,
        Xmin = 2,
        Xhalf = 3, 
        Xmax = 4,
        Ymin = 5,
        Yhalf = 6,
        Ymax = 7,

        numTextLabels = 8
    };  

    nArray< nRef < nGuiTextLabel > > refTextLabel;
    nRef<nGuiCanvas> refCanvas;

    bool hasHeader;
    nString header;
    bool hasAxisLabels;
    bool hasXAxisLabels;
    bool hasYAxisLabels;
    nString xLabel;
    nString yLabel;
    nString axisFont;
    vector4 axisTextColor;
    int minXValue;
    int maxXValue;
    int minYValue;
    int maxYValue;
    nString minYText;
    nString maxYText;
    nString minXText;
    nString maxXText;
    int numXMarkers;
    int numYMarkers;

    // Some colors
    vector4 blackColor;
    vector4 whiteColor;
    vector4 grayColor;
    vector4 redColor;
    vector4 greenColor;

};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas*
nGuiDiagramCanvas::GetCanvas() const
{
    return this->refCanvas.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetHasAxisLabels(bool b)
{
    this->hasXAxisLabels = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDiagramCanvas::HasAxisLabels() const
{
    return this->hasAxisLabels;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetHasXAxisLabels(bool b)
{
    this->hasXAxisLabels = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetHasYAxisLabels(bool b)
{
    this->hasYAxisLabels = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDiagramCanvas::HasYAxisLabels() const
{
    return this->hasYAxisLabels;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDiagramCanvas::HasXAxisLabels() const
{
    return this->hasXAxisLabels;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetAxisLabels(const char* x, const char* y)
{
    this->xLabel = x;
    this->yLabel = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetAxisFont(const char* font)
{
    this->axisFont = font;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetAxisTextColor(vector4 col)
{
    this->axisTextColor = col;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMinXAxisValue(int val)
{
    this->minXValue=val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMaxXAxisValue(int val)
{
    this->maxXValue=val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMinYAxisValue(int val)
{
    this->minYValue=val;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMaxYAxisValue(int val)
{
    this->maxYValue=val;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiDiagramCanvas::GetMinXAxisValue() const
{
    return this->minXValue;
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
int
nGuiDiagramCanvas::GetMinYAxisValue() const
{
    return this->minYValue;
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
nGuiDiagramCanvas::SetNumXMarkers(int num)
{
    this->numXMarkers=num;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetNumYMarkers(int num)
{
    this->numYMarkers=num;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetDiagramHeader(const char* header)
{
    this->header = header;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetHasHeader(bool b)
{
    this->hasHeader = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiDiagramCanvas::HasHeader() const
{
    return this->hasHeader;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMinYAxisText(const nString& text)
{
    this->minYText = text;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMaxYAxisText(const nString& text)
{
    this->maxYText = text;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiDiagramCanvas::GetMaxYAxisText() const
{
    return this->maxYText;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiDiagramCanvas::GetMinYAxisText() const
{
    return this->minYText;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMinXAxisText(const nString& text)
{
    this->minXText = text;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiDiagramCanvas::SetMaxXAxisText(const nString& text)
{
    this->maxXText = text;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiDiagramCanvas::GetMaxXAxisText() const
{
    return this->maxXText;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiDiagramCanvas::GetMinXAxisText() const
{
    return this->minXText;
}

#endif
