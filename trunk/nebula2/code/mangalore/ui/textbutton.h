#ifndef UI_TEXTBUTTON_H
#define UI_TEXTBUTTON_H
//------------------------------------------------------------------------------
/**
    @class UI::TextButton

    A text button ui element.

    (C) 2006 Radon Labs GmbH
*/
#include "ui/button.h"

//------------------------------------------------------------------------------
namespace UI
{
class TextButton : public Button
{
    DeclareRtti;
	DeclareFactory(TextButton);
public:
    /// set button text
    void SetText(const nString& t);
    /// get button text
    const nString& GetText() const;

protected:
    nString text;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
TextButton::SetText(const nString& t)
{
    this->text = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
TextButton::GetText() const
{
    return this->text;
}

};
//------------------------------------------------------------------------------
#endif