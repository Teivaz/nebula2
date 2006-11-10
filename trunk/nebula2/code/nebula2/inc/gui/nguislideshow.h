#ifndef N_GUI_SLIDESHOW_H
#define N_GUI_SLIDESHOW_H

#include "gui/nguiwidget.h"
#include "gui/nguilabel.h"
//------------------------------------------------------------------------------
/**
    @class nGuiSlideShow
    @ingroup Gui
    @brief A widget which implements a picture slideshow.

    (C) 2005 RadonLabs GmbH
*/

class nGuiSlide;

class nGuiSlideShow : public nGuiWidget
{
public:
    /// Constructor
    nGuiSlideShow();
    /// Constructor
    nGuiSlideShow(nTime seconds);
    /// Destructor
    ~nGuiSlideShow();
    /// Called when widget becomes visible
    virtual void OnShow();
    /// Called when widget becomes invisible
    virtual void OnHide();
    /// Called every frame
    virtual void OnFrame();
    /// Access the array of slideshow pictures
    nArray<nString> GetPictures() const;
    /// Set the array of slideshow pictures
    void SetPictures(nArray<nString> pictures);
    /// Add a picture to the slideshow
    void AddPicture(nString picture);
    /// Set if the slideshow should loop
    void SetLoopMode(bool b);
    /// Get if the slideshow loops
    bool GetLoopMode() const;
    /// Sets the interval between two slides
    void SetInterval(nTime seconds);
    /// Rendering
    virtual bool Render();
    /// Clear pictures array
    void RemoveAllPictures();

protected:
    /// Compute fading effects
    void UpdatePictureColor();

private:
    /// References to the current and next slide
    nArray<nRef<nGuiSlide> > refSlides;
    /// Holds the names of the slideshow pictures
    nArray<nString> pictures;
    /// The interval between two pictures (in milliseconds)
    nTime interval;
    nTime beginTime;
    nTime nextSlideTime;
    nTime time;
    uint numLevels;
    bool firstFrame;
    bool loop;
    bool togglePics;
    int currentPicIndex;
    int nextPicIndex;

    vector4 currentSlideColor;
    vector4 nextSlideColor;

};

//------------------------------------------------------------------------------
/**
*/
inline
nArray<nString>
nGuiSlideShow::GetPictures() const
{
    return this->pictures;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlideShow::SetPictures(nArray<nString> pictures)
{
    this->pictures = pictures;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlideShow::AddPicture(nString picture)
{
    this->pictures.Append(picture);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlideShow::RemoveAllPictures()
{
    this->pictures.Clear();
    this->currentPicIndex=-1;
    this->nextPicIndex=0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGuiSlideShow::GetLoopMode() const
{
    return this->loop;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlideShow::SetLoopMode(bool b)
{
    this->loop = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiSlideShow::SetInterval(nTime seconds)
{
    n_assert(seconds >= 0);
    this->interval = seconds;
}

#endif
