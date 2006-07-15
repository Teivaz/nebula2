//------------------------------------------------------------------------------
//  ui/factorymanager.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "ui/factorymanager.h"
#include "foundation/factory.h"
#include "ui/canvas.h"
#include "ui/button.h"
#include "ui/label.h"

namespace UI
{
ImplementRtti(UI::FactoryManager, Foundation::RefCounted);
ImplementFactory(UI::FactoryManager);

FactoryManager* FactoryManager::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
FactoryManager::FactoryManager()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
FactoryManager::~FactoryManager()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Create a user interface element from a type string.
*/
Element*
FactoryManager::CreateElement(const nString& type) const
{
    if (type == "Canvas")       return (Element*) Canvas::Create();
    else if (type == "Button")  return (Element*) Button::Create();
    else if (type == "Label")   return (Element*) Label::Create();
    else
    {
        n_error("Invalid UI element type: '%s'!", type.Get());
        return 0;
    }
}

} // namespace UI
