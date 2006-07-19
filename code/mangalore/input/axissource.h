#ifndef INPUT_AXISSOURCE_H
#define INPUT_AXISSOURCE_H
//------------------------------------------------------------------------------
/**
    @class Input::AxisSource

    A special Input::Source, that emit axis moved events keep track of
    the current axis slider values.

    (C) 2006 RadonLabs GmbH
*/
#include "input/source.h"
#include "input/naxisfilter.h"

namespace Input
{

class AxisSource : public Source
{
    DeclareRtti;
    DeclareFactory(AxisSource);
public:
    /// constructor 
    AxisSource();

    /// get the axis this source listens to
    int GetAxis() const;
    /// set the axis this source listens to
    void SetAxis(int axis);

    /// begin frame (reset states as needed)
    virtual void BeginFrame();
    /// consume raw event
    virtual void Consume(nInputEvent* rawEvent);
    /// end frame (combine states as needed)
    virtual void EndFrame();

    /// get axis value
    virtual float GetAxisValue() const;

private:
    int axis;
    bool axisMoved;
    float axisValue;

    enum
    {
        AxisFilterSize = 5,
    };

    nAxisFilter axisFilter;
};

}; // namespace Input
//------------------------------------------------------------------------------
#endif