#ifndef N_GUICANVAS_H
#define N_GUICANVAS_H
//------------------------------------------------------------------------------
/**
    @class nGuiCanvas
    @ingroup Gui

    @brief A GuiWidget which enables the user to draw lines and text.

    (C) 2004 RadonLabs GmbH
*/    

#include "gui/nguiwidget.h"
#include "mathlib/vector.h"
#include "mathlib/line.h"
#include "gui/nguitextbutton.h"
#include "gui/nguiserver.h"

class nGuiButton;
class nGuiFormLayout;


class nGuiCanvas : public nGuiWidget
{
public:

    class LineArray;
    class Text;

    /// Constructor
    nGuiCanvas();
    /// Destructor
    virtual ~nGuiCanvas();

    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// render the window and all contained widgets
    virtual bool Render();
    /// Called if the position or size of the canvas has changed
    virtual void OnRectChange(const rectangle& newRect);
    /// Called when the window gets focus
    virtual void OnObtainFocus();

    /// Begin construction a curve
    void BeginCurve(vector4 col);
    /// End construction of a curve
    void EndCurve();
    /// Append new line to curve
    void AppendLineToCurve(line2 line);
    /// Add a new text label
    void AddLabel(const char* text, vector2 pos, vector4 col, const char* font);
    /// Get the rectangle of a label
    rectangle GetLabelRect(int id);

    /// Get number of curves on canvas
    int GetCurveCount() const;
    /// Get number of textlabels on canvas
    int GetLabelCount() const;

    
    // Embedded class, represents a textlabel on the canvas
    class Text
    {
    public:
        /// Constructor
        Text();
        /// Constructor
        Text(const nString& cont, const vector2& pos, const vector4& col, int id, const char* font);
        /// Destructor
        ~Text();
        /// Set the font for this label
        void SetFont(const char* font);
        /// Get the font for this label
        const nString& GetFont() const;
        /// Set the position of this label
        void SetPosition(const vector2& pos);
        /// Get the position of this label
        const vector2& GetPosition() const;
        /// Set the textcolor of this label
        void SetColor(const vector4& col);
        /// Get the textcolor of this label
        const vector4& GetColor() const;
        /// Set the content of this label
        void SetContent(const nString& cont);
        /// Get the content of this label
        const nString& GetContent() const;
        /// Get the id of this label
        const int GetID() const;

    private:
        int id;
        vector4 color;
        vector2 position;
        nString content;
        nString font;

    };

    // Embedded class, represents one curve on the canvas
    class LineArray
    {
    public:
        /// Constructor
        LineArray();
        /// Constructor
        LineArray(line2 line, const vector4 col, int id);
        /// Destructor
        ~LineArray();
        /// Get the ID of this curve
        const int GetID() const;
        /// Set the color of this curve
        void SetColor(const vector4& col);
        /// Get the color of this curve
        const vector4& GetColor() const;
        /// Append a line to the curve
        void AppendLineToCurve(line2 line);
        /// Get a specific curve
        nArray<line2> GetCurve();

    private:
        int id;
        nArray<line2> curve;
        vector4 color;
    };
        

protected:
    /// Update the Canvas
    void Update();

private:

    struct curveDesc
    {
        int first;
        int num;
        vector4 color;
    };

    nArray< LineArray > curvearray;
    nArray< Text > textarray;
    nArray< curveDesc > curveDescArray;

    bool inCurve;
    bool isDirty;
    int activeCurveID;

    vector2* vertexPtr;
    vector2* vertexBasePtr;

    vector4 currentCurveColor;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::nGuiCanvas() :
    isDirty(true),
    vertexBasePtr(0),
    vertexPtr(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::~nGuiCanvas()
{
    n_delete_array(this->vertexBasePtr);
    this->vertexBasePtr = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiCanvas::GetCurveCount() const
{
    return this->curvearray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGuiCanvas::AppendLineToCurve(line2 line)
{
    this->isDirty = true;
    n_assert( this->inCurve );
    n_assert( this->activeCurveID >= 0 );

    if ( this->activeCurveID == this->curvearray.Size() )
    {
        nGuiCanvas::LineArray thecurve(line, this->currentCurveColor, this->activeCurveID);
        this->curvearray.Append(thecurve);
    }
    else
    {
        curvearray.At( this->activeCurveID ).AppendLineToCurve( line );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiCanvas::GetLabelCount() const
{
    return this->textarray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::Text::Text() :
        color(vector4(0.0f, 0.0f, 0.0f, 1.0f)),
        font("GuiDefault")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiCanvas::Text::GetFont() const
{
    return font;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::Text::Text(const nString& cont, const vector2& pos, const vector4& col, int id, const char* font) :
        content(cont),
        position(pos),
        color(col),
        id(id),
        font(font)

{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::Text::~Text()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCanvas::Text::SetFont(const char* font)
{
    this->font=font;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
nGuiCanvas::Text::GetID() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiCanvas::Text::GetPosition() const
{
    return this->position;
}


//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCanvas::Text::SetPosition(const vector2& pos)
{
    this->position = pos;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCanvas::Text::SetColor(const vector4& col)
{
    this->color = col;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiCanvas::Text::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCanvas::Text::SetContent(const nString& cont)
{
    this->content = cont;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nGuiCanvas::Text::GetContent() const
{
    return this->content;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::LineArray::LineArray() :
        color(vector4(0.0f, 0.0f, 0.0f, 1.0f)),
        id(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::LineArray::LineArray(line2 line, const vector4 col, int id) :
        color(col),
        id(id)
{
    this->AppendLineToCurve(line);
    this->SetColor(col);
}


//------------------------------------------------------------------------------
/**
*/
inline
nGuiCanvas::LineArray::~LineArray()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
inline
nArray<line2>
nGuiCanvas::LineArray::GetCurve()
{
    return this->curve;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
nGuiCanvas::LineArray::GetID() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCanvas::LineArray::SetColor(const vector4& col)
{
    this->color = col;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiCanvas::LineArray::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiCanvas::LineArray::AppendLineToCurve(line2 line)
{
    this->curve.Append(line);
}


#endif
