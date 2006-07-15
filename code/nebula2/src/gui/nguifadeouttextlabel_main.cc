//------------------------------------------------------------------------------
//  nguifadeouttextlabel_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguifadeouttextlabel.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"
#include "kernel/ntimeserver.h"

nNebulaClass(nGuiFadeOutTextLabel, "nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiFadeOutTextLabel::nGuiFadeOutTextLabel() :
    fadeOutTime(10.0),
    startTime(0.0),
    fullColor(vector4(1.f, 1.f, 1.f, 1.f))
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiFadeOutTextLabel::~nGuiFadeOutTextLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiFadeOutTextLabel::SetText(const char* text)
{
    this->startTime = nTimeServer::Instance()->GetTime();
    nGuiTextLabel::SetText(text);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiFadeOutTextLabel::SetColor(const vector4& c)
{
    this->fullColor = c;
}

//------------------------------------------------------------------------------
/**
    Render the faded text only.
*/
void
nGuiFadeOutTextLabel::RenderText(bool pressed)
{
    nTime curTime = nTimeServer::Instance()->GetTime();
    
    // update color
    if (curTime > this->startTime + this->fadeOutTime)
    {
        // text timed out do nothing
        return;
    }
    else
    {
        // update colors
        this->color = this->fullColor;
        if (this->fadeOutTime > 0.0f)
        {
            float fade = (float) (1 - 1 / this->fadeOutTime * (curTime - this->startTime));
            this->color.w *= fade;
        }

        nGuiTextLabel::RenderText(pressed);
    }
}
