//------------------------------------------------------------------------------
//  nguidiagramcanvas_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "gui/nguidiagramcanvas.h"
#include "gui/nguiwindow.h"

nNebulaClass(nGuiDiagramCanvas, "nguicanvas");

//------------------------------------------------------------------------------
/**
*/
nGuiDiagramCanvas::nGuiDiagramCanvas() :
    axisColor(0.8f, 0.8f, 0.8f, 1.0f),
    numHMarkers(10),
    numVMarkers(10)
{
    border.top = 0.015f;
    border.bottom = 0.015f;
    border.left = 0.015f;
    border.right = 0.015f;
}

//------------------------------------------------------------------------------
/**
*/
nGuiDiagramCanvas::~nGuiDiagramCanvas()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDiagramCanvas::OnShow()
{
    nFont2* textfont = (nFont2*) nResourceServer::Instance()->FindResource(this->axisFont.Get(), nResource::Font);
    nGfxServer2::Instance()->SetFont(textfont);
    
    vector2 textextentBottom = nGfxServer2::Instance()->GetTextExtent(this->xLabel.Get());

    // Setup up the X- und Y-Axis 

    line2 arrowUp1 = line2( vector2(this->border.left - (1.0f - this->border.left   - this->border.right) / 65, 
                                    this->border.top  + (1.0f - this->border.bottom - border.top)         / 65),
                            vector2(this->border.left, this->border.top));

    line2 arrowUp2 = line2( vector2(this->border.left, this->border.top), 
                            vector2(this->border.left + (1.0f - this->border.left - this->border.right) / 65, 
                                    this->border.top  + (1.0f - border.bottom     - border.top)         / 65));

    line2 arrowRight1 = line2 ( vector2(1.0f - this->border.right  - (1.0f - this->border.left - this->border.right) / 65, 
                                        1.0f - this->border.bottom - textextentBottom.y - (1.0f - this->border.bottom - this->border.top) / 65 ),
                                vector2(1.0f - this->border.right, 1.0f - this->border.bottom - textextentBottom.y) );

    line2 arrowRight2 = line2 ( vector2(1.0f - this->border.right, 1.0f - this->border.bottom - textextentBottom.y),
                                vector2(1.0f - this->border.right  - (1.0f - this->border.left - this->border.right) / 65, 
                                        1.0f - this->border.bottom - textextentBottom.y + (1.0f - this->border.top - this->border.bottom) / 65 ));

    line2 axisX = line2( vector2(0.0f + this->border.left, 1.0f - this->border.bottom - textextentBottom.y),
                         vector2(1.0f - this->border.right, 1.0f - this->border.bottom - textextentBottom.y));

    line2 axisY = line2( vector2(0.0f + this->border.left, 0.0f + this->border.top),
                         vector2(0.0f + this->border.left, 1.0f - this->border.bottom - textextentBottom.y) );

    // Append Lines to Curves
    this->BeginCurve(this->axisColor);
    this->AppendLineToCurve(arrowUp1);
    this->AppendLineToCurve(arrowUp2);
    this->EndCurve();
    
    this->BeginCurve(this->axisColor);
    this->AppendLineToCurve(axisY);
    this->AppendLineToCurve(axisX);
    this->EndCurve();

    this->BeginCurve(this->axisColor);
    this->AppendLineToCurve(arrowRight1);
    this->AppendLineToCurve(arrowRight2);
    this->EndCurve();
    
    // Setup X-Axis
    int i;

    for (i = 1; i < this->numHMarkers; i++ )
    {
        line2 marker = line2( vector2( this->border.left + ((1.0f - this->border.left     - this->border.right) / (numHMarkers)) * i,
                                       1.0f - this->border.bottom - textextentBottom.y -(1.0f - border.bottom  - this->border.top)   / 75),
                              vector2( this->border.left + ((1.0f - this->border.left     - this->border.right) / (numHMarkers)) * i,
                                       1.0f - this->border.bottom + (1.0f - border.bottom - this->border.top)   / 75 - textextentBottom.y) );
        
        this->BeginCurve(this->axisColor);
        this->AppendLineToCurve(marker);
        this->EndCurve();
    }

    // Setup Y-Axis
    for (i = 1; i < this->numVMarkers; i++)
    {
        line2 marker = line2( vector2( 0.5f * this->border.left, 
                                       1.0f - this->border.bottom - ((1.0f - this->border.bottom - this->border.top) / (numVMarkers)) * i),
                              vector2( 1.5f * this->border.left, 
                                       1.0f - this->border.bottom - ((1.0f - this->border.bottom - this->border.top) / (numVMarkers)) * i) );
        
        this->BeginCurve(this->axisColor);
        this->AppendLineToCurve(marker);
        this->EndCurve();
    }

    nGuiCanvas::Update();

}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiDiagramCanvas::Render()
{
    if(this->IsShown())
    {
        nGuiCanvas::Render();

        rectangle textRect;
        vector2 textextent;
        rectangle screenSpaceRect = this->GetScreenSpaceRect();;

        const vector4& activeWindowColor = ((nGuiWindow*) this->GetOwnerWindow())->GetWindowColor();
        const int renderFlags = nFont2::Left | nFont2::ExpandTabs;
        vector4 textcolor = this->axisColor;

        // Label for the X-Axis
        nFont2* textfont = (nFont2*) nResourceServer::Instance()->FindResource(this->axisFont.Get(), nResource::Font);
        nGfxServer2::Instance()->SetFont(textfont);
        textextent = nGfxServer2::Instance()->GetTextExtent(this->xLabel.Get());
            
        textRect = rectangle( vector2(
                                    screenSpaceRect.v1.x - (screenSpaceRect.v1.x - screenSpaceRect.v0.x) * this->border.right - textextent.x,
                                    screenSpaceRect.v1.y - (screenSpaceRect.v1.y - screenSpaceRect.v0.y) * this->border.bottom - 0.3f * textextent.y),
                                vector2(
                                    screenSpaceRect.v1.x - (screenSpaceRect.v1.x - screenSpaceRect.v0.x) * this->border.right,
                                    screenSpaceRect.v1.y - (screenSpaceRect.v1.y - screenSpaceRect.v0.y) * this->border.bottom + 0.7f * textextent.y)
                    );

        textcolor.w = activeWindowColor.w;
        nGuiServer::Instance()->DrawText( this->xLabel.Get(),
                                        textcolor,
                                        textRect,
                                        renderFlags );

        // Label for the Y-Axis
        textextent = nGfxServer2::Instance()->GetTextExtent(this->yLabel.Get());
            
        textRect = rectangle( vector2(
                                    screenSpaceRect.v0.x + (screenSpaceRect.v1.x - screenSpaceRect.v0.x) / 30,
                                    screenSpaceRect.v0.y + (screenSpaceRect.v1.y - screenSpaceRect.v0.y) / 30),
                                vector2(
                                    screenSpaceRect.v0.x + (screenSpaceRect.v1.x - screenSpaceRect.v0.x) / 30 + textextent.x,
                                    screenSpaceRect.v0.y + (screenSpaceRect.v1.y - screenSpaceRect.v0.y) / 30 + textextent.y )
                    );

        // Draw the content of the textlabel
        textcolor.w = activeWindowColor.w;
        nGuiServer::Instance()->DrawText( this->yLabel.Get(),
                                        this->axisColor,
                                        textRect,
                                        renderFlags );

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDiagramCanvas::OnHide()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDiagramCanvas::OnFrame()
{
    // empty
}

