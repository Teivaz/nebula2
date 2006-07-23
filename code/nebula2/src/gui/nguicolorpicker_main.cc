//------------------------------------------------------------------------------
//  nguicolorpicker_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicolorpicker.h"

#include "gui/nguiserver.h"
#include "gui/nguiresource.h"

nNebulaClass(nGuiColorPicker, "gui::nguiwidget");

//------------------------------------------------------------------------------
/**
*/
nGuiColorPicker::nGuiColorPicker():
    intensity(1.0f),
    alpha(1.0f),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    buttonPressedFlag(false),
    sideRatio((float)141/170)
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
nGuiColorPicker::~nGuiColorPicker()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void
nGuiColorPicker::OnShow()
{
    // horizontal size of hexagontexture
    vector2 colorHexSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("colorhex");

    this->SetDefaultBrush("colorhex");
    this->SetMaxSize(colorHexSize);
    this->SetMinSize(colorHexSize);

    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiColorPicker::Render()
{
    if (this->IsShown())
    {
        nGuiBrush* brush = &this->defaultBrush;

        // a null brush pointer is valid, in this case, nothing is rendered
        if (brush->GetName().IsEmpty())
        {
            return false;
        }
        // get gui resource from brush
        nGuiResource* guiResource = brush->GetGuiResource();
        n_assert(guiResource);

        // make sure gui resource is loaded
        if (!guiResource->IsValid())
        {
            bool success = guiResource->Load();
            n_assert(success);
        }

        // set the intensity and alpha value for rendering
        vector4 intensityColor(this->intensity, this->intensity, this->intensity, this->alpha);

        nGuiServer::Instance()->DrawTexture(this->GetScreenSpaceRect(), guiResource->GetRelUvRect(), intensityColor, guiResource->GetTexture());

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiColorPicker::OnButtonDown(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        // if button is pressed first time update color and send changed
        if (!this->buttonPressedFlag)
        {
            this->buttonPressedFlag=true;
            this->UpdateColor(mousePos);
            nGuiEvent event(this, nGuiEvent::SliderChanged);
            nGuiServer::Instance()->PutEvent(event);
        }
        nGuiWidget::OnButtonDown(mousePos);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiColorPicker::OnMouseMoved(const vector2& mousePos)
{
    nGuiWidget::OnMouseMoved(mousePos);
    if (this->buttonPressedFlag && this->Inside(mousePos))
    {
        this->UpdateColor(mousePos);
        nGuiEvent event(this, nGuiEvent::SliderChanged);
        nGuiServer::Instance()->PutEvent(event);
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiColorPicker::OnButtonUp(const vector2& mousePos)
{
    this->buttonPressedFlag = false;
    if (this->Inside(mousePos))
    {
        this->UpdateColor(mousePos);
        nGuiWidget::OnButtonUp(mousePos);;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiColorPicker::Inside(const vector2& p)
{
    if (this->IsShown())
    {
        rectangle screenSpaceRect = this->GetScreenSpaceRect();
        screenSpaceRect.v0 += this->clickRectBorder;
        screenSpaceRect.v1 -= this->clickRectBorder;

        if (screenSpaceRect.inside(p))
        {
            vector2 relMousePos;

            // calculate the relative position in the hexagontexture
            relMousePos.x=(p.x - screenSpaceRect.v0.x) / screenSpaceRect.size().x;
            relMousePos.y=(p.y - screenSpaceRect.v0.y) / screenSpaceRect.size().y;

            // rise of the sides from the hexagon
            const float rise = (float)(n_tan(2*N_PI/3)*1/sideRatio);

            // test if the mouse pointer do not lie in the death edges
            if(     (relMousePos.y >  rise * relMousePos.x + 0.5f )
                &&  (relMousePos.y < -rise * relMousePos.x + 0.5f )
                &&  (relMousePos.y > -rise * relMousePos.x + 0.5f + rise )
                &&  (relMousePos.y <  rise * relMousePos.x + 0.5f - rise ))
            {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
float
nGuiColorPicker::CalculateColorValue(const vector2& relativeCoordinates)
{
    const float texturWidth = 1.0f;
    const float texturHeight = 1.0f;

    vector2 relCoor = relativeCoordinates;
    //x and y have to be between 0.0f and 1.0f
    relCoor.x = ( 0.0f > relCoor.x )? 0.0f: relCoor.x;
    relCoor.x = ( 1.0f < relCoor.x )? 1.0f: relCoor.x;
    relCoor.y = ( 0.0f > relCoor.y )? 0.0f: relCoor.y;
    relCoor.y = ( 1.0f < relCoor.y )? 1.0f: relCoor.y;

    //left offset because of the hexagon texture - startpoint of the colorplane
    float offSetX=0.0f;
    float colorValue;

    //decide if y is over the middle or under - calculate the left offset with y coordinate
    if( texturHeight/2 >  relCoor.y ) offSetX = (texturWidth / 4) * 2 * ( texturHeight / 2-relCoor.y) / texturHeight;
    if( texturHeight/2 <= relCoor.y ) offSetX = (texturWidth / 4) * 2 * (-texturHeight / 2+relCoor.y) / texturHeight;

    //subtract with the offset
    relCoor.x -= offSetX;

    //decide if the color value have to be calculated or not
    if(0>relCoor.x)                                 colorValue=0.0f;
    if(texturWidth/2<relCoor.x)                     colorValue=1.0f;
    if(0<=relCoor.x && texturWidth/2>=relCoor.x)    colorValue=relCoor.x/(texturWidth/2);

    return colorValue;
}

//------------------------------------------------------------------------------
/**
*/
vector4
nGuiColorPicker::CalculateAllColorValues(const vector2& relativeCoordinates)
{
    vector2 relCoor = relativeCoordinates;
    vector4 _color(1.0f, 1.0f, 1.0f, 1.0f);

    // calculate the red value
    // calculate colorvalue
    _color.x = this->CalculateColorValue(relCoor);

    // calculate green colorvalue
    // move texture middle (mouse position) to the middle of the graph
    relCoor -= vector2(0.5f, 0.5f);

    // rotate 2pi/3 for green
    // transforming relative coordinate system in a real coordinate system
    relCoor.y *= this->sideRatio;
    relCoor.rotate(2*N_PI/3);
    relCoor.y /= this->sideRatio;

    // move back from the middle
    relCoor += vector2(0.5f, 0.5f);

    // calculate colorvalue
    _color.y = this->CalculateColorValue(relCoor);

    // calculate blue colorvalue
    // move texture middle (mouse position) to the middle of the graph
    relCoor -= vector2(0.5f, 0.5f);

    // rotate again 2pi/3 for blue
    // transforming relative coordinate system in a real coordinate system
    relCoor.y *= this->sideRatio;
    relCoor.rotate(2*N_PI/3);
    relCoor.y /= this->sideRatio;

    // move back from the middle
    relCoor += vector2(0.5f, 0.5f);

    // calculate green colorvalue
    // move texture middle (mouse position) to the middle of the graph

    // calculate colorvalue
    _color.z = this->CalculateColorValue(relCoor);

    // because of rounding mistakes we have to get save that one value is 1.0f
    float biggestColor  = (_color.x>_color.y)? _color.x: _color.y;
    biggestColor        = (_color.z>biggestColor)? _color.z: biggestColor;

    _color.x *= (float)(1.0f / biggestColor);
    _color.y *= (float)(1.0f / biggestColor);
    _color.z *= (float)(1.0f / biggestColor);

    return _color;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiColorPicker::UpdateColor(const vector2& mousePos)
{
    vector2 relMousePos;

    // calculate the relative position in the hexagontexture
    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    screenSpaceRect.v0 += this->clickRectBorder;
    screenSpaceRect.v1 -= this->clickRectBorder;

    relMousePos.x=(mousePos.x - screenSpaceRect.v0.x) / (screenSpaceRect.size().x);
    relMousePos.y=(mousePos.y - screenSpaceRect.v0.y) / (screenSpaceRect.size().y);

    // implements a tolerance because of rounding mistakes
    const float tolerance = 0.1f;
    relMousePos.y = relMousePos.y * (1.0f + tolerance) - (0.5f * tolerance);
    relMousePos.x = relMousePos.x * (1.0f + tolerance) - (0.5f * tolerance);

    // calculate the colorvalues
    this->color     = this->CalculateAllColorValues(relMousePos);
    this->color.x  *= this->intensity;
    this->color.y  *= this->intensity;
    this->color.z  *= this->intensity;
    this->color.w   = this->alpha;
}
