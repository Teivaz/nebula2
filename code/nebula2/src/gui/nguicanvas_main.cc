//------------------------------------------------------------------------------
//  nguicanvas_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "gui/nguicanvas.h"
#include "gfx2/ngfxserver2.h"
#include "resource/nresourceserver.h"
#include "gui/nguiwindow.h"

nNebulaClass(nGuiCanvas, "gui::nguiwidget");

//------------------------------------------------------------------------------
/**
*/
void
nGuiCanvas::AddLabel(const char* text, vector2 pos, vector4 col, const char* font)
{
    int id;
    for (id = 0; id < this->textarray.Size(); id++);

    nString txt=text;

    rectangle widgetRect = this->rect;

    pos.x = pos.x * (widgetRect.v1.x - widgetRect.v0.x);
    pos.y = pos.y * (widgetRect.v1.y - widgetRect.v0.y);

    nGuiCanvas::Text thetext(txt, pos, col, id, font);
    this->textarray.Append(thetext);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCanvas::BeginCurve(vector4 col)
{
    this->inCurve = true;

    // FIXME: Consistency
    int id;
    for (id = 0; id < this->curvearray.Size(); id++);

    this->activeCurveID = id;
    this->currentCurveColor = col;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCanvas::EndCurve()
{
    n_assert(inCurve);
    n_assert(this->activeCurveID >= 0);

    this->activeCurveID = -1;
    this->inCurve = false;
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
rectangle
nGuiCanvas::GetLabelRect(int id)
{
    if (id < textarray.Size() )
    {
        const Text& label = this->textarray.At(id);
        vector2 pos = label.GetPosition();

        nFont2* textfont = (nFont2*) nResourceServer::Instance()->FindResource(label.GetFont().Get(), nResource::Font);
        nFont2* oldfont = nGfxServer2::Instance()->GetFont();
        nGfxServer2::Instance()->SetFont(textfont);
        vector2 extent = nGfxServer2::Instance()->GetTextExtent(label.GetContent().Get());
        nGfxServer2::Instance()->SetFont(oldfont);

        rectangle labelrect = rectangle(pos, pos+extent);
        return labelrect;
    }
    else
    {
        n_printf("textlabel with ID %d not found!", id);
        return rectangle(vector2(0.0f, 0.0f), vector2(0.0f, 0.0f));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCanvas::OnShow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCanvas::OnHide()
{

    nGuiWidget::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiCanvas::Render()
{
    if (this->IsShown())
    {
        const vector4& activeWindowColor = ((nGuiWindow*) this->GetOwnerWindow())->GetWindowColor();
        rectangle screenSpaceRect = this->GetScreenSpaceRect();
        rectangle textRect;
        vector2 textextent;

        const int renderFlags = nFont2::Left | nFont2::ExpandTabs;

        nGuiServer::Instance()->DrawBrush(screenSpaceRect, this->defaultBrush);

        int i;
        for (i = 0; i < this->textarray.Size(); i++)
        {
            // Set the font for the textlabel
            nFont2* textfont = (nFont2*) nResourceServer::Instance()->FindResource(this->textarray.At(i).GetFont().Get(), nResource::Font);
            n_assert(textfont);
            nGfxServer2::Instance()->SetFont(textfont);


            // Determine how much screenspace the textlabel needs
            textextent = nGfxServer2::Instance()->GetTextExtent(this->textarray.At(i).GetContent().Get());

            textRect = rectangle( vector2(
                                        screenSpaceRect.v0.x + textarray.At(i).GetPosition().x,
                                        screenSpaceRect.v0.y + textarray.At(i).GetPosition().y),
                                  vector2(
                                        screenSpaceRect.v0.x + textarray.At(i).GetPosition().x + textextent.x,
                                        screenSpaceRect.v0.y + textarray.At(i).GetPosition().y + textextent.y )
                                );

            // Draw the content of the textlabel
            vector4 textcolor = textarray.At(i).GetColor();
            textcolor.w = activeWindowColor.w;
            nGuiServer::Instance()->DrawText( textarray.At(i).GetContent().Get(),
                                              textcolor,
                                              textRect,
                                              renderFlags );
        }

        // Do we have to update the line-coordinates?
        if(this->isDirty)
        {
            this->Update();
        }

        // Render the curves
        nGfxServer2::Instance()->BeginLines();

        for(i = 0; i < this->curveDescArray.Size(); i++)
        {
            curveDesc cd = this->curveDescArray.At(i);
            cd.color.w = activeWindowColor.w;
            nGfxServer2::Instance()->DrawLines2d( vertexPtr + cd.first, cd.num, cd.color);
        }
        nGfxServer2::Instance()->EndLines();

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCanvas::Update()
{
    if (this->isDirty)
    {
        const nDisplayMode2& dispMode = nGfxServer2::Instance()->GetDisplayMode();
        float dispWidth  = (float) dispMode.GetWidth();
        float dispHeight = (float) dispMode.GetHeight();

        rectangle screenSpaceRect = this->GetScreenSpaceRect();

        float topLeftX = screenSpaceRect.v0.x * dispWidth;
        float topLeftY = screenSpaceRect.v0.y * dispHeight;
        vector2 scale;
        scale.x = screenSpaceRect.v1.x - screenSpaceRect.v0.x;
        scale.y = screenSpaceRect.v1.y - screenSpaceRect.v0.y;

        int i;
        int num = this->curvearray.Size();
        for(i = 0; i < this->curvearray.Size(); i++)
        {
            num += this->curvearray.At(i).GetCurve().Size() + 1;
        }

        if ( this->vertexBasePtr != 0 )
        {
            n_delete_array(this->vertexBasePtr);
            this->vertexBasePtr = 0;
        }

        this->vertexBasePtr = n_new_array(vector2, num);
        n_assert(this->vertexBasePtr);
        this->vertexPtr = this->vertexBasePtr;

        int numVertices = 0;

        for (i = 0; i < this->curvearray.Size(); i++)
        {
            curveDesc cd;
            cd.first = numVertices;
            cd.color = this->curvearray.At(i).GetColor();
            cd.num = this->curvearray.At(i).GetCurve().Size() + 1;

            // store pointer to first vertex, number of vertices and color of each curve
            this->curveDescArray.PushBack(cd);

            int j;

            for (j = 0; j < this->curvearray.At(i).GetCurve().Size(); j++)
            {
                line2 theline = this->curvearray.At(i).GetCurve().At(j);

                vertexPtr[ numVertices ].x = topLeftX + scale.x * theline.b.x * dispWidth;
                vertexPtr[ numVertices ].y = topLeftY + scale.y * theline.b.y * dispHeight;
                numVertices++;

                if ((this->curvearray.At(i).GetCurve().Size() -1) == j)
                {
                    vertexPtr[ numVertices ].x = topLeftX + scale.x * (theline.b.x + theline.m.x) * dispWidth;
                    vertexPtr[ numVertices ].y = topLeftY + scale.y * (theline.b.y + theline.m.y) * dispHeight;
                    numVertices++;
                }
            }
        }
        this->isDirty=false;
        vertexPtr = vertexBasePtr;
    }
    else
    {
        n_printf("Canvas not dirty!");
    }
}

//-----------------------------------------------------------------------------
/**
    This method is called when the rectangle of the canvas is going to change.
*/
void
nGuiCanvas::OnRectChange(const rectangle& newRect)
{
    nGuiWidget::OnRectChange(newRect);
    this->isDirty = true;
}

//-----------------------------------------------------------------------------
/**
    This method is called when the canvas gets focus.
*/
void
nGuiCanvas::OnObtainFocus()
{
    nGuiWidget::OnObtainFocus();
    this->isDirty = true;
}
