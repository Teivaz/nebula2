//------------------------------------------------------------------------------
//  nguislideshow_main.cc
//  (C) 2005 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "gui/nguislideshow.h"
#include "gui/nguislide.h"
#include "gui/nguiwidget.h"
#include "gui/nguiformlayout.h"
#include "gui/nguiserver.h"
#include "kernel/ntimeserver.h"
#include "gui/nguiresource.h"

nNebulaScriptClass(nGuiSlideShow, "nguiwidget");

//------------------------------------------------------------------------------
/** Constructor
*/
nGuiSlideShow::nGuiSlideShow() :
    loop(true),
    firstFrame(true),
    interval(2.0f),
    currentPicIndex(-1),
    nextPicIndex(0),
    numLevels(5),
    togglePics(false)
{
    this->currentSlideColor = vector4(1.0f, 1.0f, 1.0f, 0.0f);
    this->nextSlideColor = vector4(1.0f, 1.0f, 1.0f, 0.0f);
    this->beginTime = nTimeServer::Instance()->GetTime();
    this->time = nTimeServer::Instance()->GetTime();
}

//------------------------------------------------------------------------------
/** Constructor which takes the interval between two pictures as an argument
*/
nGuiSlideShow::nGuiSlideShow(nTime seconds)
{
    nGuiSlideShow();
    this->interval = seconds;
}

//------------------------------------------------------------------------------
/** Destructor
*/
nGuiSlideShow::~nGuiSlideShow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void nGuiSlideShow::OnHide()
{
    if(this->refSlides.At(0).isvalid())
    {
        this->refSlides.At(0)->Release();
        n_assert(!this->refSlides.At(0).isvalid());
    }

    if(this->refSlides.At(1).isvalid())
    {
        this->refSlides.At(1)->Release();
        n_assert(!this->refSlides.At(1).isvalid());
    }
}

//------------------------------------------------------------------------------
/**
*/
void nGuiSlideShow::OnShow()
{
    n_assert(this->pictures.Size() > 0);

    this->firstFrame = true;

    // Assuming all slides have the same size
    vector2 size = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(this->pictures.At(0).Get());
    this->SetMinSize(size);
    this->SetMaxSize(size);

    // Create current slide label
    nGuiSlide* slide = (nGuiSlide*) kernelServer->New("nguislide", "currentslide");
    n_assert(slide);
    slide->SetMinSize(size);
    slide->SetMaxSize(size);
    slide->SetRect( this->GetRect() );
    slide->OnShow();
    this->refSlides.At(0) = slide;

    // Create next slide label
    slide = (nGuiSlide*) kernelServer->New("nguislide", "nextslide");
    n_assert(slide);
    slide->SetMinSize(size);
    slide->SetMaxSize(size);
    slide->SetRect( this->GetRect() );
    slide->OnShow();
    this->refSlides.At(1) = slide;

    this->currentSlideColor.set(1.0f, 1.0f, 1.0f, 1.0f);
    this->nextSlideColor.set(1.0f, 1.0f, 1.0f, 0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void nGuiSlideShow::OnFrame()
{
    if(this->pictures.Size() > 0)
    {
        if(this->firstFrame)
        {
            this->currentSlideColor.w = 1.0;
            this->nextSlideColor.w = 0.0;
            // this->pictures[this->currentPicIndex].Get()
            this->refSlides[0]->SetDefaultBrush( 0 );
            this->refSlides[1]->SetDefaultBrush( this->pictures[this->nextPicIndex].Get() );
            this->firstFrame = false;
            this->beginTime = nTimeServer::Instance()->GetTime();
        }

        // Check if it's time for the next slide
        if(time > (this->beginTime + interval) || !this->refSlides[0]->HasPicture())
        {
            // Set new picture indices
            if((this->currentPicIndex + 1) >= this->pictures.Size())
            {
                if(this->loop)
                {
                    this->currentPicIndex = 0;
                }
            }
            else
            {
                this->currentPicIndex += 1;
            }

            if((this->nextPicIndex + 1) >= this->pictures.Size())
            {
                if(this->loop)
                {
                    this->nextPicIndex = 0;
                }
            }
            else
            {
                this->nextPicIndex += 1;
            }

            this->UpdatePictureColor();

            // Flip the brushes, if needed
            if(this->currentPicIndex != this->nextPicIndex && this->togglePics )
            {
                this->refSlides.At(0).get()->SetDefaultBrush( this->pictures[this->currentPicIndex].Get() );
                this->refSlides.At(1).get()->SetDefaultBrush( this->pictures[this->nextPicIndex].Get() );
                this->currentSlideColor.w = 1.0f;
                this->nextSlideColor.w = 0.0f;
                this->togglePics=false;
            }
        }
        this->time = nTimeServer::Instance()->GetTime();
    }
}

////------------------------------------------------------------------------------
///**
//*/
bool
nGuiSlideShow::Render()
{
    if(this->IsShown() && this->pictures.Size() > 0)
    {
        this->refSlides[0]->SetRenderColor(this->currentSlideColor);
        this->refSlides[1]->SetRenderColor(this->nextSlideColor);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiSlideShow::UpdatePictureColor()
{
    this->currentSlideColor.w = n_saturate( this->currentSlideColor.w -= 0.01f );

    this->nextSlideColor.w = n_saturate( this->nextSlideColor.w += 0.01f );

    if(this->nextSlideColor.w == 1.0f && this->currentSlideColor.w == 0.0f)
    {
        // flip the slides
        this->beginTime = nTimeServer::Instance()->GetTime();
        this->togglePics = true;
    }
}
