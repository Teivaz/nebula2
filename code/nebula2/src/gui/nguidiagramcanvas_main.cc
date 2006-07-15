//------------------------------------------------------------------------------
//  nguidiagramcanvas_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "gui/nguidiagramcanvas.h"
#include "gui/nguicanvas.h"
#include "gui/nguitextlabel.h"

nNebulaClass(nGuiDiagramCanvas, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiDiagramCanvas::nGuiDiagramCanvas() :
    hasXAxisLabels(false),
    hasYAxisLabels(false),
    hasAxisLabels(false),
    xLabel("x"),
    yLabel("y"),
    hasHeader(false),
    numXMarkers(5),
    numYMarkers(5)
{
    this->refTextLabel.SetFixedSize(nGuiDiagramCanvas::numTextLabels);

    this->blackColor = vector4(0.0f, 0.0f, 0.0f, 1.0f);
    this->whiteColor = vector4(1.0f, 1.0f, 1.0f, 1.0f);
    this->grayColor = vector4(0.7f, 0.7f, 0.7f, 1.0f);
    this->redColor = vector4(1.0f, 0.0f, 0.0f, 1.0f);
    this->greenColor = vector4(0.0f, 1.0f, 0.0f, 1.0f);

    this->curveOffset[Left] = 0.01f;
    this->curveOffset[Right] = 0.00f;
    this->curveOffset[Top] = 0.00f;
    this->curveOffset[Bottom] = 0.01f;
    this->curveOffset[ArrowLength] = 0.01f;
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
    n_assert(!this->refCanvas.isvalid());

    kernelServer->PushCwd(this);

    nGuiTextLabel* label;
    vector2 size;
    nString line;

    // create optional axis labels

    // create Y-Axis label
    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "yaxislabel");
    n_assert(label);
    
    if ( this->HasAxisLabels() )
    {
        label->SetText(this->yLabel.Get());
    }

    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    label->SetColor(this->axisTextColor);
    this->AttachForm(label, nGuiFormLayout::Top, 0.005f);
    this->AttachForm(label, nGuiFormLayout::Left, 0.005f);
    label->OnShow();
    this->refTextLabel[YLabel] = label;

    // create X-Axis label
    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "xaxislabel");
    n_assert(label);
    
    if ( this->HasAxisLabels() )
    {
        label->SetText(this->xLabel.Get());
    }
    
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent(); 
    label->SetMinSize(size); 
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    label->SetColor(this->axisTextColor);
    this->AttachForm(label, nGuiFormLayout::Bottom, 0.0f);
    this->AttachForm(label, nGuiFormLayout::Right, 0.005f);
    label->OnShow();
    this->refTextLabel[XLabel] = label;
    

    // create Y-Axis value-labels
    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "yminlabel");
    n_assert(label);
    line = this->GetMinYAxisText();
    if (line.IsEmpty())
    {
        // fallback to value
        line.AppendInt( this->GetMinYAxisValue() );
    }

    if (this->HasYAxisLabels())
    {    
        label->SetText(line.Get());
    }
    else
    {
        label->SetText(" ");
    }


    label->SetColor(this->axisTextColor);
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    this->AttachForm(label, nGuiFormLayout::Left, 0.0005f);
    this->AttachForm(label, nGuiFormLayout::Bottom, 0.0005f + size.y);
    label->OnShow();
    this->refTextLabel[Ymin] = label;

    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "yhalflabel");
    n_assert(label);
    line="";
    if (this->GetMinYAxisText().IsEmpty())
    {
        // fallback to value
        line.AppendInt( this->GetMaxYAxisValue() / 2);
    }

    if (this->HasYAxisLabels() && !(line == this->refTextLabel[Ymin]->GetText()))
    {    
        label->SetText(line.Get());
    }
    else
    {
        label->SetText(" ");
    }

    label->SetColor(this->axisTextColor);
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    this->AttachForm(label, nGuiFormLayout::Left, 0.0005f);
    // FIXME: This is just the vertical center of the widget
    // and does not fit to the canvas marker in the middle
    this->AttachPos(label, nGuiFormLayout::VCenter, 0.5f);
    label->OnShow();
    this->refTextLabel[Yhalf] = label;

    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "ymaxlabel");
    n_assert(label);
    line = this->GetMaxYAxisText();
    if (line.IsEmpty())
    {
        // fallback to value
        line.AppendInt( this->GetMaxYAxisValue() );
    }
    
    if (this->HasYAxisLabels())
    {    
        label->SetText(line.Get());
    }
    else
    {
        label->SetText(" ");
    }

    label->SetColor(this->axisTextColor);
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    this->AttachForm(label, nGuiFormLayout::Left, 0.0005f);
    this->AttachWidget(label, nGuiFormLayout::Top, this->refTextLabel[YLabel].get(), 0.0005f);
    label->OnShow();
    this->refTextLabel[Ymax] = label;

    // create X-Axis value-labels
    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "xminlabel");
    n_assert(label);
    line = this->GetMinXAxisText();
    if (line.IsEmpty())
    {
        // fallback to value
        line.AppendInt( this->GetMinXAxisValue() );
    }
    
    if (this->HasXAxisLabels())
    {    
        label->SetText(line.Get());
    }
    else
    {
        label->SetText(" ");
    }

    label->SetColor(this->axisTextColor);
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    this->AttachForm(label, nGuiFormLayout::Bottom, 0.0f);
    this->AttachWidget(label, nGuiFormLayout::Left, this->refTextLabel[Ymax].get(), 0.005f);
    label->OnShow();
    this->refTextLabel[Xmin] = label;

    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "xhalflabel");
    n_assert(label);
    line="";
    if (this->GetMinXAxisText().IsEmpty())
    {
        // fallback to value
        line.AppendInt( this->GetMinXAxisValue() + ( (this->GetMaxXAxisValue() - this->GetMinXAxisValue()) / 2));
    }
    
    if (this->HasXAxisLabels() && !(line == this->refTextLabel[Xmin]->GetText()))
    {    
        label->SetText(line.Get());
    }
    else
    {
        label->SetText(" ");
    }

    label->SetColor(this->axisTextColor);
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    this->AttachForm(label, nGuiFormLayout::Bottom, 0.0f);
    // FIXME: This is just the horizontal center of the widget
    // and does not fit to the canvas marker in the middle
    this->AttachPos(label, nGuiFormLayout::HCenter, 0.5f);
    label->OnShow();
    this->refTextLabel[Xhalf] = label;

    label = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "xmaxlabel");
    n_assert(label);
    line = this->GetMaxXAxisText();
    if (line.IsEmpty())
    {
        // fallback to value
        line.AppendInt( this->GetMaxXAxisValue() );
    }
    
    if (this->HasXAxisLabels())
    {    
        label->SetText(line.Get());
    }
    else
    {
        label->SetText(" ");
    }
    
    label->SetColor(this->axisTextColor);
    label->SetFont(this->axisFont.Get());
    size = label->GetTextExtent();
    label->SetMinSize(size);
    label->SetMaxSize(size);
    label->SetBorder(vector2(0.0f, 0.0f));
    this->AttachForm(label, nGuiFormLayout::Bottom, 0.0f);
    this->AttachWidget(label, nGuiFormLayout::Right, this->refTextLabel[XLabel].get(), 0.005f);
    label->OnShow();
    this->refTextLabel[Xmax] = label;
    

    if ( this->HasHeader() )
    {
        // FIXME: create an optional headerlabel
    }
    
    // create Canvas
    nGuiCanvas* canvas = (nGuiCanvas*) kernelServer->New("nguicanvas", "diagramcanvas");
    n_assert(canvas);

    this->AttachWidget(canvas, nGuiFormLayout::Top, this->refTextLabel[YLabel].get(), 0.005f);
    this->AttachWidget(canvas, nGuiFormLayout::Bottom, this->refTextLabel[Xmin].get(), 0.005f);
    this->AttachWidget(canvas, nGuiFormLayout::Left, this->refTextLabel[Ymax].get(), 0.005f);
    this->AttachWidget(canvas, nGuiFormLayout::Right, this->refTextLabel[XLabel].get(), 0.005f);

    // Y-Axis
    canvas->BeginCurve(this->axisTextColor);
    canvas->AppendLineToCurve( line2(
                            /* start */ vector2(this->curveOffset[Left], this->curveOffset[Top]),
                            /* end */ vector2(this->curveOffset[Left], 1.0f - this->curveOffset[Bottom]) ));
    canvas->EndCurve();

    // X-Axis
    canvas->BeginCurve(this->axisTextColor);
    canvas->AppendLineToCurve( line2(
                          /* start */ vector2(this->curveOffset[Left], 1.0f - this->curveOffset[Bottom]),
                          /* end */ vector2(1.0f - this->curveOffset[Right], 1.0f - this->curveOffset[Bottom]) ));
    canvas->EndCurve();

    // Y-Arrow
    canvas->BeginCurve(this->axisTextColor);
    canvas->AppendLineToCurve( line2(
                          /* start */ vector2(0.0f, this->curveOffset[ArrowLength]),
                            /* end */ vector2(this->curveOffset[ArrowLength], 0.0f) ));
    canvas->AppendLineToCurve( line2(
                          /* start */ vector2(this->curveOffset[ArrowLength], 0.0f),
                            /* end */ vector2(2 * this->curveOffset[ArrowLength], this->curveOffset[ArrowLength]) ));
    canvas->EndCurve();

    // X-Arrow
    canvas->BeginCurve(this->axisTextColor);
    canvas->AppendLineToCurve( line2(
                          /* start */ vector2(1.0f - this->curveOffset[ArrowLength], 1.0f - ( 2 * this->curveOffset[ArrowLength]) ),
                            /* end */ vector2(1.0f, 1.0f - this->curveOffset[ArrowLength] ) ));
    canvas->AppendLineToCurve( line2(
                          /* start */ vector2(1.0f, 1.0f - this->curveOffset[ArrowLength]),
                            /* end */ vector2(1.0f - this->curveOffset[ArrowLength], 1.0f) ));
    canvas->EndCurve();

    int i;
    rectangle canvasrect = this->GetRect();

    // Markers on the Y-Axis
    float yStride = (1.0f - this->curveOffset[Bottom] - this->curveOffset[Top]) / (numYMarkers + 1);
    for (i = 0; i < numYMarkers; i++ )
    {
        float yPos = yStride * (i + 1);
        line2 marker = line2(
                        /* start */ vector2(0.0f,  yPos),
                          /* end */ vector2(2 * this->curveOffset[Left], yPos));

        canvas->BeginCurve(this->axisTextColor);
        canvas->AppendLineToCurve(marker);
        canvas->EndCurve();
    }

    // Markers on the X-Axis
    float xStride = (1.0f - this->curveOffset[Left] - this->curveOffset[Right]) / (numXMarkers + 1);
    for (i = 0; i < numXMarkers; i++ )
    {
        float xPos = xStride * (i + 1);
        line2 marker = line2(
                        /* start */ vector2(xPos, 1.0f),
                          /* end */ vector2(xPos, 1.0f - ( 2 * this->curveOffset[Bottom])));

        canvas->BeginCurve(this->axisTextColor);
        canvas->AppendLineToCurve(marker);
        canvas->EndCurve();
    }

    this->refCanvas = canvas;
    canvas->OnShow();

    this->UpdateLayout(this->rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDiagramCanvas::OnHide()
{
    if(this->refCanvas.isvalid())
    {
        this->refCanvas->Release();
        n_assert(!this->refCanvas.isvalid());
    }

    int i;
    for (i = 0; i < nGuiDiagramCanvas::numTextLabels; i++)
    {
        if(this->refTextLabel[i].isvalid())
        {
            this->refTextLabel[i]->Release();
            n_assert(!this->refTextLabel[i].isvalid());
        }
    }
    this->ClearAttachRules();
}

