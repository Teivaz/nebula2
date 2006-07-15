//------------------------------------------------------------------------------
//  nguiformlayout_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguiformlayout.h"

nNebulaScriptClass(nGuiFormLayout, "nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiFormLayout::nGuiFormLayout() :
    attachRules(16, 16)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiFormLayout::~nGuiFormLayout()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
nGuiFormLayout::AttachForm(nGuiWidget* widget, Edge where, float offset)
{
    n_assert(widget);

    Rule newRule;
    newRule.attachType = Form;
    newRule.widget     = widget;
    newRule.other      = 0;
    newRule.edge       = where;
    newRule.offset     = offset;
    this->attachRules.Append(newRule);
    return (this->attachRules.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
int
nGuiFormLayout::AttachWidget(nGuiWidget* widget, Edge where, nGuiWidget* other, float offset)
{
    n_assert(widget && other);

    Rule newRule;
    newRule.attachType = Widget;
    newRule.widget     = widget;
    newRule.other      = other;
    newRule.edge       = where;
    newRule.offset     = offset;
    this->attachRules.Append(newRule);
    return (this->attachRules.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
int
nGuiFormLayout::AttachPos(nGuiWidget* widget, Edge where, float pos)
{
    n_assert(widget);

    Rule newRule;
    newRule.attachType = Pos;
    newRule.widget     = widget;
    newRule.other      = 0;
    newRule.edge       = where;
    newRule.offset     = pos;
    this->attachRules.Append(newRule);
    return (this->attachRules.Size() - 1);
}

//------------------------------------------------------------------------------
/**
    Fix the rectangle inplace so that it honors the widget's minimum and 
    maximum size.
*/
void
nGuiFormLayout::FixMinMaxSize(nGuiWidget* widget, Edge anchor, rectangle& rect)
{
    n_assert(widget);
    const vector2& minSize = widget->GetMinSize();
    const vector2& maxSize = widget->GetMaxSize();
    switch (anchor)
    {
        case Left:
            {
                float w = rect.width();
                if (w < minSize.x)
                {
                    rect.v1.x = rect.v0.x + minSize.x;
                }
                else if (w > maxSize.x)
                {
                    rect.v1.x = rect.v0.x + maxSize.x;
                }
            }
            break;

        case Right:
            {
                float w = rect.width();
                if (w < minSize.x)
                {
                    rect.v0.x = rect.v1.x - minSize.x;
                }
                else if (w > maxSize.x)
                {
                    rect.v0.x = rect.v1.x - maxSize.x;
                }
            }
            break;

        case Top:
            {
                float h = rect.height();
                if (h < minSize.y)
                {
                    rect.v1.y = rect.v0.y + minSize.y;
                }
                else if (h > maxSize.y)
                {
                    rect.v1.y = rect.v0.y + maxSize.y;
                }
            }
            break;

        case Bottom:
            {
                float h = rect.height();
                if (h < minSize.y)
                {
                    rect.v0.y = rect.v1.y - minSize.y;
                }
                else if (h > maxSize.y)
                {
                    rect.v0.y = rect.v1.y - maxSize.y;
                }
            }
            break;

        case HCenter:
            {
                float w = rect.width();
                float midX = rect.midpoint().x;
                if (w < minSize.x)
                {
                    float halfSize = minSize.x * 0.5f;
                    rect.v0.x = midX - halfSize;
                    rect.v1.x = midX + halfSize;
                }
                else if (w > maxSize.x)
                {
                    float halfSize = maxSize.x * 0.5f;
                    rect.v0.x = midX - halfSize;
                    rect.v1.x = midX + halfSize;
                }
            }
            break;

        case VCenter:
            {
                float h = rect.height();
                float midY = rect.midpoint().y;
                if (h < minSize.y)
                {
                    float halfSize = minSize.y * 0.5f;
                    rect.v0.y = midY - halfSize;
                    rect.v1.y = midY + halfSize;
                }
                else if (h > maxSize.y)
                {
                    float halfSize = maxSize.y * 0.5f;
                    rect.v0.y = midY - halfSize;
                    rect.v1.y = midY + halfSize;
                }
            }
            break;
    }
}

//------------------------------------------------------------------------------
/**
    Update the layout of child widgets. Will be called from 
    and OnRectChange().
*/
void
nGuiFormLayout::UpdateLayout(const rectangle& newRect)
{
    // first pass: handle all layout rules which attach the widgets
    // to the form layout and to relative positions
    int i;
    int num = this->attachRules.Size();
    for (i = 0; i < num; i++)
    {
        const Rule& rule = this->attachRules[i];
        if (Form == rule.attachType)
        {
            rectangle widgetRect = rule.widget->GetRect();
            switch (rule.edge)
            {
                case Left:
                    // attach the widget's left edge to the form's left edge
                    widgetRect.v0.x = rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Left, widgetRect);
                    break;

                case Right:
                    // attach the widget's right edge to the form's right edge
                    widgetRect.v1.x = newRect.width() - rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Right, widgetRect);
                    break;

                case Top:
                    // attach the widget's top edge to the form's top edge
                    widgetRect.v0.y = rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Top, widgetRect);
                    break;

                case Bottom:
                    // attach the widget's right edge to the form's right edge
                    widgetRect.v1.y = newRect.height() - rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Bottom, widgetRect);
                    break;
            }
            rule.widget->SetRect(widgetRect);
        }
        else if (Pos == rule.attachType)
        {
            rectangle widgetRect = rule.widget->GetRect();
            switch (rule.edge)
            {
                case Left:
                    widgetRect.v0.x = newRect.width() * rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Left, widgetRect);
                    break;

                case Right:
                    widgetRect.v1.x = newRect.width() * rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Right, widgetRect);
                    break;

                case Top:
                    widgetRect.v0.y = newRect.height() * rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Top, widgetRect);
                    break;

                case Bottom:
                    widgetRect.v1.y = newRect.height() * rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Bottom, widgetRect);
                    break;

                case HCenter:
                    {
                        float center = newRect.width() * rule.offset;
                        float halfSize = widgetRect.width() * 0.5f;
                        widgetRect.v0.x = center - halfSize;
                        widgetRect.v1.x = center + halfSize;
                        this->FixMinMaxSize(rule.widget.get(), HCenter, widgetRect);
                    }
                    break;

                case VCenter:
                    {
                        float center = newRect.height() * rule.offset;
                        float halfSize = widgetRect.height() * 0.5f;
                        widgetRect.v0.y = center - halfSize;
                        widgetRect.v1.y = center + halfSize;
                        this->FixMinMaxSize(rule.widget.get(), VCenter, widgetRect);
                    }
                    break;
            }
            rule.widget->SetRect(widgetRect);
        }
    }

    // second pass: handle hcenter and vcenter
    for (i = 0; i < num; i++)
    {
        const Rule& rule = this->attachRules[i];
        if (Pos == rule.attachType)
        {
            rectangle widgetRect = rule.widget->GetRect();
            switch (rule.edge)
            {
                case HCenter:
                    {
                        float center = newRect.width() * rule.offset;
                        float halfSize = widgetRect.width() * 0.5f;
                        widgetRect.v0.x = center - halfSize;
                        widgetRect.v1.x = center + halfSize;
                        this->FixMinMaxSize(rule.widget.get(), HCenter, widgetRect);
                    }
                    break;

                case VCenter:
                    {
                        float center = newRect.height() * rule.offset;
                        float halfSize = widgetRect.height() * 0.5f;
                        widgetRect.v0.y = center - halfSize;
                        widgetRect.v1.y = center + halfSize;
                        this->FixMinMaxSize(rule.widget.get(), VCenter, widgetRect);
                    }
                    break;
            }
            rule.widget->SetRect(widgetRect);
        }
    }

    // third pass: handle all layout rules which attach the widgets
    // to other widgets.
    for (i = 0; i < num; i++)
    {
        const Rule& rule = this->attachRules[i];
        if (Widget == rule.attachType)
        {
            rectangle widgetRect = rule.widget->GetRect();
            const rectangle& otherRect = rule.other->GetRect();
            switch (rule.edge)
            {
                case Left:
                    // attach widget's left edge to right edge of other widget
                    widgetRect.v0.x = otherRect.v1.x + rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Left, widgetRect);
                    break;

                case Right:
                    // attach widget's right edge to left edge of other widget
                    widgetRect.v1.x = otherRect.v0.x - rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Right, widgetRect);
                    break;

                case Top:
                    // attach widget's top edge to bottom edge of other widget
                    widgetRect.v0.y = otherRect.v1.y + rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Top, widgetRect);
                    break;

                case Bottom:
                    // attach widget's bottom edge to top edge of other widget
                    widgetRect.v1.y = otherRect.v0.y - rule.offset;
                    this->FixMinMaxSize(rule.widget.get(), Bottom, widgetRect);
                    break;
            }
            rule.widget->SetRect(widgetRect);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Called when the size of the widget changes. Will re-apply the layout
    rules on its children widgets.
*/
void
nGuiFormLayout::OnRectChange(const rectangle& newRect)
{
    if (this->IsShown())
    {
        this->UpdateLayout(newRect);
    }
    nGuiWidget::OnRectChange(newRect);
}
