#ifndef N_GUICOLORPICKER_H
#define N_GUICOLORPICKER_H 
//------------------------------------------------------------------------------
/**
    @class nGuiColorPicker
    @ingroup Gui

    @brief to choose and select a rgb-color

    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiColorPicker : public nGuiWidget
{
public:
    /// constructor
    nGuiColorPicker();
    /// destructor
    virtual ~nGuiColorPicker();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// render the widget
    virtual bool Render();
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up event
    virtual bool OnButtonUp(const vector2& mousePos);
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// is position inside?
    virtual bool Inside(const vector2& p);

    /// get the color from last click      
    const vector4& GetColor() const;
    /// set the intensity (brightness) of the hexagon
    void SetIntensity(const float& intensity);
    /// set the alpha value
    void SetAlpha(const float& alpha);

private:
    /// calculate the colorvalue with relative coordinates
    /// - importent rotate mouse before with the colorangle rgb(0, 2*pi/3, 4*pi/3)
    /// - important mousecoordinates have to lie between 0.0f and 1.0f
    float CalculateColorValue(const vector2& relativeCoordinates);
    /// calculate all rgb color values, alpha is 1.0f
    vector4 CalculateAllColorValues(const vector2& relativeCoordinates);
    /// update the color
    void UpdateColor(const vector2& mousePos);

    /// holds the last clickt color
    vector4 color;
    /// intesity (brightness)
    float intensity;
    /// alphavalue
    float alpha;
    /// flag for button pressed
    bool buttonPressedFlag;

    /// ratio between width and height of the hexagon
    const float sideRatio;
};

//-----------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiColorPicker::GetColor() const
{
    return this->color;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void nGuiColorPicker::SetIntensity(const float& intensity)
{
    this->intensity = intensity;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void nGuiColorPicker::SetAlpha(const float& alpha)
{
    this->alpha = alpha;
}


//-----------------------------------------------------------------------------
#endif
