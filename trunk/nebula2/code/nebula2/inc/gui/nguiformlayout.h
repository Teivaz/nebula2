#ifndef N_GUIFORMLAYOUT_H
#define N_GUIFORMLAYOUT_H
//------------------------------------------------------------------------------
/**
    @class nGuiFormLayout
    @ingroup Gui
    @brief Lays out size and position of child widgets based on own position.

    This works similar to a MEL formLayout.
    
    Relations between child widgets and the form layout are defined
    by connecting their edges either to the form, to other widgets,
    or to a (relative) position:

     - Attach To Form:
       Attaches the edge of a child widget to an edge of the form

     - Attach To Widget:
       Attaches the edge of a child widget to the closest edge of another
       child widget.

     - Attach To Position:
       Attaches the edge of a child widget to a relative position, expressed
       in percent of the form layout's size.

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiFormLayout : public nGuiWidget
{
public:
    /// edge types
    enum Edge
    {
        InvalidEdge,
        Top,
        Bottom,
        Left,
        Right,
        HCenter,
        VCenter,
    };

    /// constructor
    nGuiFormLayout();
    /// destructor
    virtual ~nGuiFormLayout();
    /// clear attachment rules
    void ClearAttachRules();
    /// add an "Attach To Form" layout rule
    int AttachForm(nGuiWidget* widget, Edge where, float offset);
    /// add an "Attach To Widget" layout rule
    int AttachWidget(nGuiWidget* widget, Edge where, nGuiWidget* other, float offset);
    /// add an "Attach To Position" layout rule
    int AttachPos(nGuiWidget* widget, Edge where, float pos);
    /// called when widget position or size changes
    virtual void OnRectChange(const rectangle& newRect);
    /// convert string enum to Edge
    static Edge StringToEdge(const char* str);

protected:
    /// update the layout of child widgets
    void UpdateLayout(const rectangle& newRect);
    /// fix rectangle size based on widget's min/max size
    void FixMinMaxSize(nGuiWidget* widget, Edge anchor, rectangle& rect);

    /// attachment types
    enum AttachType
    {
        Form,
        Widget,
        Pos,
    };

    /// holds an attachment rule
    struct Rule
    {
        AttachType attachType;
        nRef<nGuiWidget> widget;
        nRef<nGuiWidget> other;
        Edge edge;
        float offset;
    };

    nArray<Rule> attachRules;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFormLayout::ClearAttachRules()
{
    this->attachRules.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiFormLayout::Edge
nGuiFormLayout::StringToEdge(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "top"))         return Top;
    else if (0 == strcmp(str, "bottom")) return Bottom;
    else if (0 == strcmp(str, "left"))   return Left;
    else if (0 == strcmp(str, "right"))  return Right;
    else
    {
        n_error("nGuiFormLayout: invalid Edge type string '%s'\n", str);
        return InvalidEdge;
    }
}

//------------------------------------------------------------------------------
#endif

