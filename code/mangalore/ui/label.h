#ifndef UI_LABEL_H
#define UI_LABEL_H
//------------------------------------------------------------------------------
/**
    A simple GUI label.

    (C) 2005 Radon Labs GmbH
*/
#include "ui/element.h"

//------------------------------------------------------------------------------
namespace UI
{
class Label : public Element
{
    DeclareRtti;
	DeclareFactory(Label);

public:
    /// constructor
    Label();
    /// destructor
    virtual ~Label();
    /// called before the gui hierarchy is rendered
    virtual void OnRender();
};

RegisterFactory(Label);

} // namespace UI
//------------------------------------------------------------------------------
#endif
