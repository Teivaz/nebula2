#ifndef INPUT_MOUSESOURCE_H
#define INPUT_MOUSESOURCE_H
//------------------------------------------------------------------------------
/**
    @class Input::MouseSource

    A special Input::Source, that convert the mouse move messages that the
    where generated in the win proc.

    (C) 2006 RadonLabs GmbH
*/
#include "input/source.h"

namespace Input
{

class MouseSource : public Source
{
    DeclareRtti;
    DeclareFactory(MouseSource);
public:
    /// constructor 
    MouseSource();

    /// begin frame (reset states as needed)
    virtual void BeginFrame();
    /// consume raw event
    virtual void Consume(nInputEvent* rawEvent);
    /// end frame (combine states as needed)
    virtual void EndFrame();

private:
    bool mouseMoved;
    vector2 relMousePosition;
    int absMousePositionX;
    int absMousePositionY;
};

}; // namespace Input
//------------------------------------------------------------------------------
#endif