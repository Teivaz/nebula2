//------------------------------------------------------------------------------
//  nguitickerwidget_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitickerwidget.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiTickerWidget, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiTickerWidget::nGuiTickerWidget() :
    scrollSpeed(3.0f),
    refTimeServer("/sys/servers/time"),
    textIsInvalid(true),
    isDone(true),
    timeIsInvalid(true),
    startTime(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTickerWidget::~nGuiTickerWidget()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTickerWidget::RenderText()
{
    if (this->text.IsEmpty() || this->isDone)
    {
        // no text, nothing to render
        return;
    }

    // (re-)validate the font object
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get()); 
        }
        else
        {
            this->refFont->AddRef();
        }
    }

    // compute the text position
    nGfxServer2::Instance()->SetFont(this->refFont.get());
    uint renderFlags = nFont2::VCenter | nFont2::Left;

    // get screen rect
    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    
    if (this->timeIsInvalid)
    {
        this->startTime = this->refTimeServer->GetTime();
        this->timeIsInvalid = false;
    }

    if (this->textIsInvalid)
    {
        this->textExtend = nGfxServer2::Instance()->GetTextExtent(this->text.Get());
        this->textIsInvalid = false;
    }

    float deltaTime = (float)(this->refTimeServer->GetTime() - this->startTime);   
    //compute current drawRect
    if (deltaTime > 0.0f && this->scrollSpeed > 0.0f)
    {
        rectangle drawRect = screenSpaceRect;
        float unitsPerSecond = this->scrollSpeed;//1.0f / this->scrollSpeed;
        
        float endPositon = -this->textExtend.x;//screenSpaceRect.v0.x - this->textExtend.x;
        float startPosition = screenSpaceRect.v1.x;
        float distance = startPosition - endPositon;
        
        float deltaDistance = unitsPerSecond * deltaTime;

        if (startPosition - deltaDistance < endPositon)
        {
            //done
            this->isDone = true;
        }
        else
        {
            // draw text
            float drawStart = screenSpaceRect.v1.x - deltaDistance;
            float drawEnd   = (drawStart + this->textExtend.x) < screenSpaceRect.v1.x ? (drawStart + this->textExtend.x) : screenSpaceRect.v1.x;
            drawRect.v0.x = drawStart;
            drawRect.v1.x = drawEnd;
            nGuiServer::Instance()->DrawText(this->GetText(), this->color, drawRect, renderFlags);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiTickerWidget::Render()
{
    if (this->IsShown())
    {
        // render the text
        this->RenderText();
        return true;
    }
    return false;
}

